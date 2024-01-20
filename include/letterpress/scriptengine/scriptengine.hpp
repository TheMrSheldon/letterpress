#pragma once

#include "./context.hpp"
#include "./module.hpp"

#include <filesystem>
#include <string>
#include <vector>

class asIScriptEngine;

namespace lp::script {
	class ScriptEngine final {
	private:
		asIScriptEngine* angelscript;

		ScriptEngine(ScriptEngine& other) = delete;
		ScriptEngine(const ScriptEngine& other) = delete;
		ScriptEngine& operator=(ScriptEngine& other) = delete;
		ScriptEngine& operator=(const ScriptEngine& other) = delete;
		ScriptEngine& operator=(ScriptEngine&& other) = delete;
	public:
		ScriptEngine();
		ScriptEngine(ScriptEngine&& other);
		~ScriptEngine();
		bool init();
		void deinit();

		Module createModule(std::string name, std::vector<std::filesystem::path> files);
		Context createContext();
		Module loadModule(std::filesystem::path path);
	};
}