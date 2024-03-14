#include <letterpress/parser.hpp>
#include <letterpress/scriptengine/scriptengine.hpp>

#include <antlr4-runtime.h>
#include <lplexer.h>
#include <lpparser.h>
#include <lpparserBaseVisitor.h>

#include <chrono>
#include <optional>
#include <sstream>

using namespace antlr4;
using namespace lp;
using namespace lp::doc;
using namespace lp::script;

using path = std::filesystem::path;

class Visitor final : public lpparserBaseVisitor {
private:
	using DoctypeArgs = std::map<std::string, std::any>;

	lp::log::LoggerPtr logger;
	Document document;
	ScriptEngine engine;
	Context scriptctx;
	std::vector<path> includeDirs;

	std::vector<Module> loadedModules;
	std::shared_ptr<IDocClass> doctype = nullptr;

	Visitor(const Visitor& other) = delete;
	Visitor(Visitor&& other) = delete;

public:
	Visitor(lp::Driver& driver, std::vector<path> includeDirs)
			: logger(lp::log::getLogger("parser")), document({driver}), includeDirs(includeDirs), scriptctx(nullptr) {
		engine.init();
		scriptctx = std::move(engine.createContext());
		/** \todo remove hardcoded font **/
		// auto font = std::make_shared<lp::pdf::utils::FontFile>("res/fonts/computer-modern/cmunrm.ttf");
		// auto font = std::make_shared<lp::pdf::utils::FontFile>("res/fonts/domine/Domine-Regular.ttf");
		auto font = std::make_shared<lp::pdf::utils::FontFile>("res/fonts/futura-renner/FuturaRenner-Light.otf");
		document.pushFont(font);
	}

	virtual ~Visitor() {
		doctype = nullptr;	 /* Destroy doctype */
		scriptctx = nullptr; /* Destroy context */
		engine.deinit();
	}

	std::optional<path> getModulePath(std::string moduleName) const noexcept {
		for (auto&& dir : includeDirs) {
			auto path = dir / (moduleName + ".lpbin");
			if (std::filesystem::is_regular_file(path))
				return path;
		}
		return std::nullopt;
	}

	virtual std::any visitFile(lpparser::FileContext* ctx) override {
		visitChildren(ctx);
		document.flush();
		return document;
	}

	virtual std::any visitP_import_expr(lpparser::P_import_exprContext* ctx) override {
		logger->trace("Importing module: {}", ctx->IDENT()->getText());
		auto modpath = getModulePath(ctx->IDENT()->getText());
		if (!modpath.has_value()) {
			logger->critical("Failed to load module {}: Not found", ctx->IDENT()->getText());
			return defaultResult();
		} else {
			loadedModules.push_back(engine.loadModule(modpath.value()));
			return defaultResult();
		}
	}

	virtual std::any visitP_doctype_expr(lpparser::P_doctype_exprContext* ctx) override {
		if (doctype) {
			logger->critical("Found a second doctype declaration");
			abort(); /** \todo more graceful shutdown */
		}
		auto doctypename = ctx->IDENT()->getText();
		logger->trace("Loading document type {}", doctypename);
		doctype = scriptctx.instantiateDocumentClass(doctypename, loadedModules[0]);
		if (doctype == nullptr) {
			logger->critical("Document type {} was not found", doctypename);
			abort(); /** \todo more graceful shutdown */
		}
		auto arguments = std::any_cast<DoctypeArgs>(visitP_args(ctx->p_args()));
		logger->debug("Arguments:");
		for (auto&& [key, value] : arguments) {
			if (value.type() == typeid(std::string)) {
				logger->debug("    {:10}:    {}", key, std::any_cast<std::string>(value));
			} else {
				logger->debug("    {:10}:    ???", key);
			}
		}
		return defaultResult();
	}

	virtual std::any visitP_args(lpparser::P_argsContext* ctx) override {
		DoctypeArgs arguments;
		for (auto&& child : ctx->p_arg()) {
			std::any childResult = child->accept(this);
			auto result = std::any_cast<DoctypeArgs::value_type>(childResult);
			arguments.insert(result);
		}
		return arguments;
	}

	virtual std::any visitP_arg(lpparser::P_argContext* ctx) override {
		auto key = ctx->IDENT()->getText();
		auto value = ctx->p_arg_val()->getText(); /** \todo visit child instead to get the correct datatype **/
		return DoctypeArgs::value_type(key, value);
	}

	virtual std::any visitContent(lpparser::ContentContext* ctx) override {
		logger->trace("Processing Content");
		return visitChildren(ctx);
	}

	virtual std::any visitContent_part(lpparser::Content_partContext* ctx) override {
		/** \todo distinguish character, whitespace and new-paragraph*/
		switch (ctx->getStart()->getType()) {
		case lpparser::SPACE:
			document.addWhitespace();
			break;
		case lpparser::CHARACTER:
			/** \todo iterate codepoints and add appropriate kerning etc. **/
			for (auto c : ctx->getText())
				document.addCharacter(c);
			break;
		case lpparser::PAR:
			document.writeParagraph();
			break;
		default:
			logger->critical(
					"Unexpected token type in content: {}. This is a bug and should never happen.",
					ctx->getStart()->getType()
			);
			abort(); /** \todo more graceful shutdown */
		}
		return defaultResult();
	}
};

lp::Parser::Parser(Driver& driver) : logger(lp::log::getLogger("parser")), driver(driver) {}

Document lp::Parser::parse(const path& input, std::vector<path> includeDirs) noexcept {
	std::ifstream stream(input, std::ios::binary);
	return parse(stream, includeDirs);
}

Document lp::Parser::parse(const std::string& string, std::vector<path> includeDirs) noexcept {
	std::istringstream stream(string);
	return parse(stream, includeDirs);
}

Document lp::Parser::parse(std::istream& input, std::vector<path> includeDirs) noexcept {
	logger->info("Parsing");
	auto start = std::chrono::steady_clock::now();
	ANTLRInputStream stream(input);
	lplexer lexer(&stream);
	CommonTokenStream tokens(&lexer);
	lpparser parser(&tokens);

	Visitor visitor(driver, includeDirs);
	auto doc = std::any_cast<Document>(visitor.visit(parser.file()));

	auto stop = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	logger->info("Parsing done (took {} ms)", time.count());
	
	return doc;
}