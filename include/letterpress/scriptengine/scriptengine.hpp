#pragma once

#include "../logging.hpp"
#include "./context.hpp"
#include "./module.hpp"

#include <filesystem>
#include <string>
#include <vector>

class asIScriptEngine;
class asSMessageInfo;

namespace lp::doc {
	class Document;
}

namespace lp::script {

	class ScriptEngine final {
		friend class Context;

	private:
		lp::log::LoggerPtr logger = lp::log::getLogger("Script");

		asIScriptEngine* angelscript;

		ScriptEngine(ScriptEngine& other) = delete;
		ScriptEngine(const ScriptEngine& other) = delete;
		ScriptEngine& operator=(ScriptEngine& other) = delete;
		ScriptEngine& operator=(const ScriptEngine& other) = delete;
		ScriptEngine& operator=(ScriptEngine&& other) = delete;

		void logCallback(const asSMessageInfo* msg, void* param) noexcept;

	public:
		ScriptEngine();
		ScriptEngine(ScriptEngine&& other);
		~ScriptEngine();
		bool init(lp::doc::Document* doc);
		void deinit();

		Module createModule(std::string name, std::vector<std::filesystem::path> files);
		Context createContext();
		Module loadModule(std::filesystem::path path);
	};
} // namespace lp::script