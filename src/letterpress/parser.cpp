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

// https://en.wikipedia.org/wiki/UTF-8#Encoding
/**
 * \brief Extracts the first UTF-8 codepoint from the given string
 */
inline char32_t from_utf8(std::string str) {
	uint8_t* bytes = (uint8_t*)str.c_str();
	if (bytes[0] < 0b10000000u) { /** 1 Byte UTF-8 **/
		return bytes[0];
	} else if (bytes[0] < 0b11100000u) { /** 2 Byte UTF-8 **/
		return (bytes[0] & 0b00011111u) << 6 | (bytes[1] & 0b00111111u);
	} else if (bytes[0] < 0b11110000u) { /** 3 Byte UTF-8 **/
		return (bytes[0] & 0b00001111u) << 12 | (bytes[1] & 0b00111111u) << 6 | (bytes[2] & 0b00111111u);
	} else if (bytes[0] < 0b11111000u) { /** 4 Byte UTF-8 **/
		return (bytes[0] & 0b00000111u) << 18 | (bytes[1] & 0b00111111u) << 12 | (bytes[2] & 0b00111111u) << 6 |
			   (bytes[3] & 0b00111111u);
	}
	throw std::runtime_error("UTF-8 codepoint out of range");
}

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
		engine.init(&document);
		scriptctx = std::move(engine.createContext());
		/** \todo remove hardcoded font **/
		document.pushFont("cmr12");
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
		/** \todo remove hardcoded module to load docclass from **/
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

	void invokeCommand(std::string name, std::vector<std::string> args) {
		/** \todo: don't hardcode the module to load from **/
		if (scriptctx.invokeMethod(name, args, loadedModules[0])) {
		} else if (scriptctx.invokeMethod(name, args, document, engine)) {
		} else {
			logger->critical("Could not invoke {}", name);
			abort(); /** \todo more graceful shutdown **/
		}
	}

	virtual std::any visitContent_part(lpparser::Content_partContext* ctx) override {
		switch (ctx->getStart()->getType()) {
		case lpparser::SPACE:
			document.addWhitespace();
			break;
		case lpparser::CHARACTER: {
			char32_t character = from_utf8(ctx->getText());
			document.addCharacter(character);
			break;
		}
		case lpparser::PAR:
			invokeCommand("par", {});
			break;
		case lpparser::COMMAND: {
			std::string command = ctx->COMMAND()->getText().substr(1);
			std::vector<std::string> params;
			for (auto&& param : ctx->param()) {
				auto tmp = param->getText();
				params.push_back(tmp.substr(1, tmp.length() - 2));
			}
			invokeCommand(command, params);
			break;
		}
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

#if 0
#include <letterpress/parser/parser.hpp>
class Visitor final : public lp::parser::Visitor {
private:
	using DoctypeArgs = std::map<std::string, std::any>;

	lp::log::LoggerPtr logger;
	ScriptEngine engine;
	Context scriptctx;
	std::vector<path> includeDirs;

	std::vector<Module> loadedModules;
	std::shared_ptr<IDocClass> doctype = nullptr;

	Visitor(const Visitor& other) = delete;
	Visitor(Visitor&& other) = delete;

public:
	Document document;

	Visitor(lp::Driver& driver, std::vector<path> includeDirs)
			: logger(lp::log::getLogger("parser")), document({driver}), includeDirs(includeDirs), scriptctx(nullptr) {
		engine.init(&document);
		scriptctx = std::move(engine.createContext());
		/** \todo remove hardcoded font **/
		document.pushFont("cmr12");
	}

	virtual void visitCommand(std::string command) override {
		std::string command = command;
		std::vector<std::string> params;
		for (auto&& param : ctx->param()) {
			auto tmp = param->getText();
			params.push_back(tmp.substr(1, tmp.length() - 2));
		}
		/** \todo: don't hardcode the module to load from **/
		if (scriptctx.invokeMethod(command, params, loadedModules[0])) {
		} else if (scriptctx.invokeMethod(command, params, document, engine)) {
		} else {
			logger->critical("Could not invoke {}", command);
			abort(); /** \todo more graceful shutdown **/
		}
	}
	virtual void visitCharacter(char32_t codepoint) override { document.addCharacter(codepoint); }
	virtual void visitSpace() override { document.addWhitespace(); }
};
#endif

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
	/*logger->info("Parsing");
	auto start = std::chrono::steady_clock::now();

	lp::parser::Lexer lexer(input);
	Visitor visitor(driver, includeDirs);
	lp::parser::Parser parser(lexer, visitor);

	auto stop = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	logger->info("Parsing done (took {} ms)", time.count());

	return visitor.document;*/
}