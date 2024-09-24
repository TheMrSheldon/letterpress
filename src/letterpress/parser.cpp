#include <letterpress/parser.hpp>

#include <letterpress/parser/lpparser.hpp>

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

	lp::log::LoggerPtr logger;
	ScriptEngine engine;
	Context scriptctx;
	std::vector<path> includeDirs;

	std::vector<Module> loadedModules;
	std::shared_ptr<IDocClass> doctype = nullptr;

	NewVisitor(const NewVisitor& other) = delete;
	NewVisitor(NewVisitor&& other) = delete;

public:
	Document document;

	NewVisitor(lp::Driver& driver, std::vector<path> includeDirs)
			: logger(lp::log::getLogger("parser")), document({driver}), includeDirs(includeDirs), scriptctx(nullptr) {
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

	virtual void visitDoctype(const std::string& doctypename, const std::map<std::string, std::any>& args) override {
		if (doctype) {
			logger->critical("Found a second doctype declaration");
			abort(); /** \todo more graceful shutdown */
		}
		logger->trace("Loading document type {}", doctypename);
		/** \todo remove hardcoded module to load docclass from **/
		doctype = scriptctx.instantiateDocumentClass(doctypename, loadedModules[0]);
		if (doctype == nullptr) {
			logger->critical("Document type {} was not found", doctypename);
			abort(); /** \todo more graceful shutdown */
		}
		if (!args.empty()) {
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
	}

	virtual void visitCommand(const std::string& command, std::vector<LPValue> args) {
		/** \todo: don't hardcode the module to load from **/
		if (scriptctx.invokeMethod(command, args, loadedModules[0])) {
		} else if (scriptctx.invokeMethod(command, args, document, engine)) {
		} else {
			logger->critical("Could not invoke {}", command);
			abort(); /** \todo more graceful shutdown **/
		}
	}
	virtual void visitWhitespace() { document.addWhitespace(); }
	virtual void visitCharacter(char32_t c) { document.addCharacter(c); }
};

Document lp::Parser::parse(std::istream& input, std::vector<path> includeDirs) noexcept {
	logger->info("Parsing");
	auto start = std::chrono::steady_clock::now();
	NewVisitor visitor(driver, includeDirs);
	lp::LPParser parser{input, visitor};
	parser.parseFile();
	visitor.document.flush();

	auto stop = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	logger->info("Parsing done (took {} ms)", time.count());
	return visitor.document;
}
