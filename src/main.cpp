#include <letterpress/letterpress.hpp>
#include <letterpress/logging.hpp>
#include <letterpress/parser.hpp>
#include <letterpress/scriptengine/scriptengine.hpp>

#include <qpdf/QPDF.hh>
#include <angelscript.h>

#include <CLI/CLI.hpp>

#include <assert.h>
#include <iostream>
#include <format>
#include <fstream>
#include <memory>
#include <optional>

using namespace lp::script;

static inline std::filesystem::path replaceExtension(std::filesystem::path path, std::string extension) noexcept {
	return path.replace_extension(extension);
}

struct LoggerConf final {
	bool quiet = false;
	/**
	 * @brief Sets the verbosity of the logger. Per default (verbosity=0) only critical, error and warnings are printed.
	 * If verbosity is...
	 *  - 1: info and above is printed
	 *  - 2: debug and above is printed
	 *  - 3+: trace and above is printed
	 */
	int verbosity = 0;

	lp::log::Verbosity getVerbosity() const noexcept {
		if (quiet)
			return lp::log::Verbosity::Off;
		auto verb = static_cast<int>(lp::log::Verbosity::Warning) + verbosity;
		verb = std::min(verb, static_cast<int>(lp::log::Verbosity::Trace));
		return static_cast<lp::log::Verbosity>(verb);
	}
};

struct PDFAppArgs {
	std::filesystem::path input;
	std::optional<std::filesystem::path> output;
	std::vector<std::filesystem::path> includeDirs;
	LoggerConf logConf;
};
struct CompileAppArgs {
	std::vector<std::filesystem::path> inputs;
	std::optional<std::filesystem::path> output;
	LoggerConf logConf;
};

static void runPDFCommand(const PDFAppArgs args) {
	lp::log::setVerbosity(args.logConf.getVerbosity());
	auto logger = lp::log::getLogger("lp");
	auto output = args.output.value_or(replaceExtension(args.input, "pdf"));
	logger->info("Translating {} to {}", args.input, output);
	logger->debug("Include directories");
	for (auto&& dir : args.includeDirs)
		logger->debug("    {}", dir);
	lp::Parser parser;
	auto document = parser.parse(args.input, args.includeDirs);
	// document.save(outPath);
}

static void runCompileCommand(CompileAppArgs args) {
	lp::log::setVerbosity(args.logConf.getVerbosity());
	std::filesystem::path output = args.output.value_or("a.lpbin");

	ScriptEngine engine;
	engine.init();
	auto module = engine.createModule(output.filename(), args.inputs);
	module.saveToFile(output);
	engine.deinit();
}

void setupLoggerArgs(CLI::App& app, LoggerConf& conf) {
	app.add_flag(
			"-v,--verbose", conf.verbosity,
			"Sets the logger's verbosity. Passing it multiple times increases verbosity."
	);
	app.add_flag("-q,--quiet", conf.quiet, "Supresses all outputs");
}

int main(int argc, char* argv[]) {
	CLI::App app("TODO");
	auto versionString = std::format(
			"letterpress v.{}\nqpdf v.{}\nangelscript v.{}", lp::version, QPDF::QPDFVersion(), asGetLibraryVersion()
	);
	app.set_version_flag("-v,--version", versionString);

	// PDF Command
	PDFAppArgs pdfArgs;
	CLI::App& pdfApp = *app.add_subcommand("pdf", "TODO");
	setupLoggerArgs(pdfApp, pdfArgs.logConf);
	pdfApp.add_option("input", pdfArgs.input, "The file to be processed")
		->check(CLI::ExistingFile)
		->required();
	pdfApp.add_option("-o,--out", pdfArgs.output, "The output path.");
	pdfApp.add_option("-I", pdfArgs.includeDirs, "Include directories.")->allow_extra_args(false);
	pdfApp.callback([&pdfArgs]() { runPDFCommand(pdfArgs); });

	// Compile Script Command
	CompileAppArgs compileArgs;
	CLI::App& compileApp = *app.add_subcommand("compile", "TODO");
	setupLoggerArgs(compileApp, compileArgs.logConf);
	compileApp.add_option("input", compileArgs.inputs, "The files to be compiled")
		->check(CLI::ExistingFile)
		->required();
	compileApp.add_option("-o,--out", compileArgs.output, "The output path.");
	compileApp.callback([&compileArgs]() { runCompileCommand(compileArgs); });

    CLI11_PARSE(app, argc, argv);

	return 0;

	/*
	PDF pdf;
	Font font1(pdf, "/home/shelly/documents/repos/letterpress/res/fonts/computer-modern/cmunrm.ttf");
	Font font2(pdf, "/home/shelly/documents/repos/letterpress/res/fonts/LatinmodernmathRegular.otf");
	Font font3(pdf, "/home/shelly/documents/repos/letterpress/res/fonts/baskervaldx/type1/Baskervaldx-Reg.pfb", "/home/shelly/documents/repos/letterpress/res/fonts/baskervaldx/afm/Baskervaldx-Reg.afm");
	auto& page = pdf.addPage();
	page.setMediaBox(0, 0, 612, 392);

	auto& stream = page.getContentStream();
	stream.setFont(font1, 24)
		.beginText()
			.setTextLeading(24)
			.moveText(72, 320)
			.showKernedText("The quick brown fox jumped over the sleazy dog")
			.nextLine()
			.setFont(font2, 24)
			.showKernedText("The quick brown fox jumped over the sleazy dog")
			.nextLine()
			.setFont(font3, 24)
			.showKernedText("The quick brown fox jumped over the sleazy dog")
			.nextLine()
			.setFont(font3, 12)
			.showKernedText("The quick brown fox jumped over the sleazy dog")
			.nextLine()
			.showKernedText("Σ")
		.endText();

	pdf.save("test.pdf");
	*/


	/*
	lp::doc::Document doc;
	doc.pushPageStyle();
	doc.pushLayout();
	doc.pushFont("cmunrm.ttf", 12);
	doc.writeText(R"(
		Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et
		dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet
		clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet,
		consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed
		diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea
		takimata sanctus est Lorem ipsum dolor sit amet.
	)");
	doc.popFont();
	doc.popLayout();
	doc.popPageStyle();
	doc.save("test.pdf");
	*/
}