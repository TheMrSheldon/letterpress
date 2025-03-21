#pragma once

#include "../document/idocclass.hpp"

#include <any>
#include <memory>
#include <string>
#include <vector>

class asIScriptEngine;
class asIScriptContext;

namespace lp::doc {
	class Document;
}

namespace lp::script {
	class Module;
	class ScriptEngine;

	class Context final {
	private:
		asIScriptContext* context;

		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

	public:
		explicit Context(asIScriptContext* context);
		Context(Context&& other);
		~Context();

		Context& operator=(Context&& other);

		void destroy();

		std::shared_ptr<lp::doc::IDocClass> instantiateDocumentClass(std::string name, Module& module);
		bool invokeMethod(std::string name, std::vector<std::any>& arguments, Module& module);
		bool
		invokeMethod(std::string name, std::vector<std::any>& arguments, lp::doc::Document& doc, ScriptEngine& engine);
	};
} // namespace lp::script