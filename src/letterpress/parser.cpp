#include <letterpress/parser.hpp>

#include <antlr4-runtime.h>

#include <jointparserBaseVisitor.h>
#include <jointparserLexer.h>
#include <jointparserParser.h>

using namespace antlr4;
using namespace lp;
using namespace lp::doc;
using namespace lp::script;

using path = std::filesystem::path;

// https://en.wikipedia.org/wiki/UTF-8#Encoding
/**
 * \brief Extracts the first UTF-8 codepoint from the given string
 */
inline char32_t from_utf8(const std::string& str) {
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

class Visitor final : public jointparserBaseVisitor {
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
		scriptctx.destroy(); /* Destroy context */
		engine.deinit();
	}

	std::optional<path> getModulePath(const std::string& moduleName) const noexcept {
		for (auto&& dir : includeDirs) {
			auto path = dir / (moduleName + ".lpbin");
			if (std::filesystem::is_regular_file(path))
				return path;
		}
		return std::nullopt;
	}

	virtual std::any visitFile(jointparserParser::FileContext* ctx) override {
		visitChildren(ctx);
		document.flush();
		return document;
	}

	virtual std::any visitP_import_expr(jointparserParser::P_import_exprContext* ctx) override {
		auto modname = std::any_cast<std::string>(visitIdent(ctx->name));
		logger->trace("Importing module: {}", modname);
		auto modpath = getModulePath(modname);
		if (!modpath.has_value()) {
			logger->critical("Failed to load module {}: Not found", modname);
			return defaultResult();
		} else {
			loadedModules.push_back(engine.loadModule(modpath.value()));
			return defaultResult();
		}
	}

	virtual std::any visitP_doctype_expr(jointparserParser::P_doctype_exprContext* ctx) override {
		if (doctype) {
			logger->critical("Found a second doctype declaration");
			abort(); /** \todo more graceful shutdown */
		}
		auto doctypename = std::any_cast<std::string>(visitIdent(ctx->name));
		logger->trace("Loading document type {}", doctypename);
		/** \todo remove hardcoded module to load docclass from **/
		doctype = scriptctx.instantiateDocumentClass(doctypename, loadedModules[0]);
		if (doctype == nullptr) {
			logger->critical("Document type {} was not found", doctypename);
			abort(); /** \todo more graceful shutdown */
		}
		if (ctx->args != nullptr) {
			auto args = std::any_cast<std::map<std::string, std::any>>(visitDict(ctx->args));
			logger->debug("Arguments:");
			for (auto&& [key, value] : args) {
				if (value.type() == typeid(std::string)) {
					logger->debug("    {:10}:    {}", key, std::any_cast<std::string>(value));
				} else if (value.type() == typeid(float)) {
					logger->debug("    {:10}:    {}", key, std::any_cast<float>(value));
				} else if (value.type() == typeid(std::vector<std::any>)) {
					logger->debug("    {:10}:    <>", key);
				} else {
					logger->debug("    {:10}:    ???", key);
				}
			}
		} else {
			logger->debug("Arguments: <empty>");
		}
		return defaultResult();
	}

	virtual std::any visitContent(jointparserParser::ContentContext* ctx) override { return visitChildren(ctx); }

	virtual std::any visitContent_part(jointparserParser::Content_partContext* ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitCharacter(jointparserParser::CharacterContext* ctx) override {
		char32_t character = from_utf8(ctx->getText());
		document.addCharacter(character);
		return defaultResult();
	}

	virtual std::any visitSpace(jointparserParser::SpaceContext* ctx) override {
		document.addWhitespace();
		return defaultResult();
	}

	virtual std::any visitActive_seq(jointparserParser::Active_seqContext* ctx) override {
		/** For now only one active sequence is supported: \n\n */
		document.writeParagraph();
		return defaultResult();
	}

	virtual std::any visitCommand(jointparserParser::CommandContext* ctx) override { return visitChildren(ctx); }

	virtual std::any visitValue(jointparserParser::ValueContext* ctx) override { return visitChildren(ctx); }

	virtual std::any visitText(jointparserParser::TextContext* ctx) override { return ctx->getText(); }

	virtual std::any visitNumber(jointparserParser::NumberContext* ctx) override { return std::stof(ctx->getText()); }

	virtual std::any visitArray(jointparserParser::ArrayContext* ctx) override {
		std::vector<std::any> vec;
		for (auto child : ctx->entries)
			vec.push_back(visitValue(child));
		return vec;
	}

	virtual std::any visitDict(jointparserParser::DictContext* ctx) override {
		std::map<std::string, std::any> dict;
		for (auto entry : ctx->entries)
			dict.insert(std::any_cast<decltype(dict)::value_type>(visitDict_entry(entry)));
		return dict;
	}

	virtual std::any visitDict_entry(jointparserParser::Dict_entryContext* ctx) override {
		auto key = std::any_cast<std::string>(visitIdent(ctx->key));
		auto value = visitValue(ctx->val);
		return std::pair<const std::string, std::any>(std::move(key), std::move(value));
	}

	virtual std::any visitIdent(jointparserParser::IdentContext* ctx) override { return ctx->getText(); }

	//virtual std::any visitDigit(jointparserParser::DigitContext* ctx) override { return std::stoi(ctx->getText()); }

	//virtual std::any visitAlpha(jointparserParser::AlphaContext* ctx) override { return ctx->getText(); }
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
	jointparserLexer lexer(&stream);
	CommonTokenStream tokens(&lexer);
	jointparserParser parser(&tokens);

	Visitor visitor(driver, includeDirs);
	auto doc = std::any_cast<Document>(visitor.visit(parser.file()));

	auto stop = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	logger->info("Parsing done (took {} ms)", time.count());

	return doc;
}