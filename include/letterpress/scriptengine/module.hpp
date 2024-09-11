#pragma once

#include "../document/idocclass.hpp"

#include <filesystem>

class asIScriptEngine;
class asIScriptModule;

namespace lp::script {
	class Module final {
		friend class Context;

	private:
		asIScriptModule* module;

	public:
		explicit Module(asIScriptModule* module);

		void saveToFile(std::filesystem::path path);
		static Module LoadFromFile(asIScriptEngine* engine, std::filesystem::path path);
	};
} // namespace lp::script