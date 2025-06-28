#include <letterpress/parser.hpp>

#include <letterpress/parser/lpparser.hpp>
#include <letterpress/scriptengine/method.hpp>
#include <letterpress/utils/overloaded.hpp>

#include <angelscript.h>

#include <fstream>

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

lp::Parser::Parser(Driver& driver) : logger(lp::log::getLogger("parser")), driver(driver) {}

Document lp::Parser::parse(const path& input, std::vector<path> includeDirs) noexcept {
	std::ifstream stream(input, std::ios::binary);
	return parse(stream, includeDirs);
}

Document lp::Parser::parse(const std::string& string, std::vector<path> includeDirs) noexcept {
	std::istringstream stream(string);
	return parse(stream, includeDirs);
}

class NewVisitor : public LPVisitor {

private:
	using DoctypeArgs = std::map<std::string, std::any>;
	LPParser* parser;

	lp::log::LoggerPtr logger;
	ScriptEngine engine;
	Context scriptctx;
	std::vector<path> includeDirs;

	std::vector<Module> loadedModules;
	asIScriptObject* doctypeScriptObject;
	std::shared_ptr<IDocClass> doctype = nullptr;

	NewVisitor(const NewVisitor& other) = delete;
	NewVisitor(NewVisitor&& other) = delete;

public:
	Document document;

	NewVisitor(lp::Driver& driver, std::vector<path> includeDirs)
			: parser(nullptr), logger(lp::log::getLogger("parser")), document({driver}), includeDirs(includeDirs),
			  scriptctx(nullptr) {
		engine.init(&document);
		scriptctx = std::move(engine.createContext());
		/** \todo remove hardcoded font **/
		document.pushFont("cmr12");
	}

	virtual ~NewVisitor() {
		doctype = nullptr;	 /* Destroy doctype */
		scriptctx.destroy(); /* Destroy context */
		engine.deinit();
	}

	void setParser(LPParser& parser) { this->parser = &parser; }

	std::optional<path> getModulePath(const std::string& moduleName) const noexcept {
		for (auto&& dir : includeDirs) {
			auto path = dir / (moduleName + ".lpbin");
			if (std::filesystem::is_regular_file(path))
				return path;
		}
		return std::nullopt;
	}

	virtual void visitImport(const std::string& moduleName) override {
		logger->trace("Importing module: {}", moduleName);
		auto modpath = getModulePath(moduleName);
		if (!modpath.has_value()) {
			logger->critical("Failed to load module {}: Not found", moduleName);
		} else {
			loadedModules.push_back(engine.loadModule(modpath.value()));
		}
	}

	virtual void visitDoctype(const std::string& doctypename, const LPDict& args) override {
		if (doctype) {
			logger->critical("Found a second doctype declaration: {}", doctypename);
			/** \todo Instead of shutting down here, we should add a flag (e.g., -fdouble-doctype) that toggles between
			 *  failing when the method does not exist or simply warning about it. **/
			abort(); /** \todo more graceful shutdown */
		}
		logger->trace("Loading document type {}", doctypename);
		/** \todo remove hardcoded module to load docclass from **/
		doctype = scriptctx.instantiateDocumentClass(doctypename, args, loadedModules[0], doctypeScriptObject);
		if (doctype == nullptr) {
			logger->critical("Document type {} was not found", doctypename);
			/** \todo Instead of shutting down here, we should add a flag (e.g., -fmissing-doctype) that toggles between
			 *  failing when the method does not exist or simply warning about it. **/
			abort(); /** \todo more graceful shutdown */
		}
		if (!args.empty()) {
			logger->debug("Arguments:");
			for (auto&& [key, value] : args) {
				std::visit(
						lp::utils::overloaded{
								[this, key](const LPArray& array) { logger->debug("    {:10}:    <>", key); },
								[this, key](const LPDict& dict) { logger->debug("    {:10}:    ???", key); },
								[this, key](const Object& value) { logger->debug("    {:10}:    ???", key); },
								[this, key](const auto& value) { logger->debug("    {:10}:    {}", key, value); },
						},
						value
				);
			}
		} else {
			logger->debug("Arguments: <empty>");
		}
	}

	virtual void visitCommand(const std::string& command) {
		// Order of finding Method: 1) Docclass, 2) Module, 3) Globals
		/** \todo: don't hardcode the module to load from **/
		auto method =
				scriptctx
						.getMethod(command, doctypeScriptObject)									   // 1) Docclass
						.or_else([&](auto) { return scriptctx.getMethod(command, loadedModules[0]); }) // 2) Modules
						.or_else([&](auto) { return scriptctx.getMethod(command, engine); });		   // 3) Globals
		if (method) {
			// read args
			logger->trace("Reading parameters for {}:", method->name());
			LPArray args;
			for (auto&& arg : method->params()) {
				readArgument(arg, args);
			}
			/*while (lookAhead(0) == '{') {
				advance();
				args.emplace_back(readValue([](char32_t c) { return c == '}'; }));
				if (advance() != '}')
					abort();
			}*/
			method->invoke(args);
		} else {
			logger->critical("Could not find {}", command);
			/** \todo Instead of shutting down here, we should add a flag (e.g., -fmissing-method) that toggles between
			 *  failing when the method does not exist or simply warning about it. **/
			std::abort(); /** \todo more graceful shutdown **/
		}
	}
	virtual void visitWhitespace() { document.addWhitespace(); }
	virtual void visitCharacter(char32_t c) { document.addCharacter(c); }

	/**
	 * @brief Parses the parameter described by \p param and appends it to \p args .
	 * @details The parameter is parsed by looking up a constructor that takes the Parser as its argument. I.e., if the
	 * parameter has type `MyType`, then the argument is constructed through the `MyType(LPParser&)` constructor.
	 * 
	 * @param param 
	 * @param args 
	 */
	void readArgument(const lp::script::Param& param, LPArray& args) {
		const auto& typeId = param.typeId;
		if ((typeId & asTYPEID_MASK_OBJECT) != 0) {
			LPArray tmp{};
			tmp.push_back(LPValue{static_cast<void*>(parser)});
			auto x = scriptctx.construct(param.typeId, "string(parser&inout)", tmp);
			assert(x.has_value());
			args.push_back(x.value());
			/*if (auto value = scriptctx.construct(param.typeId, "(Parser)", {parser})) {
				args.emplace_back(std::move(value.value()));
			} else {
				logger->critical("Failed to parse argument: {}", static_cast<unsigned>(value.error()));
				std::abort(); /** \todo handle more gracefully **/
			/*} */
		} else {
			if (typeId == asTYPEID_INT32) {
				auto tmp = parser->readBetween('{', '}');
				args.push_back(lp::LPValue{std::atoi(tmp.c_str())});
				return;
			}
			abort();
		}
	}
};

Document lp::Parser::parse(std::istream& input, std::vector<path> includeDirs) noexcept {
	logger->info("Parsing");
	auto start = std::chrono::steady_clock::now();
	NewVisitor visitor(driver, includeDirs);
	lp::LPParser parser{input, visitor};
	visitor.setParser(parser);
	parser.parseFile();
	visitor.document.flush();

	auto stop = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	logger->info("Parsing done (took {} ms)", time.count());
	return visitor.document;
}
