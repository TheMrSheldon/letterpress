#pragma once

#include "../document/idocclass.hpp"

#include <memory>
#include <string>

class asIScriptEngine;
class asIScriptContext;

namespace lp::script {
	class Module;

	class Context final {
	private:
		asIScriptContext* context;

		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

		void destroy();
	public:
		Context(asIScriptContext* context);
		Context(Context&& other);
		~Context();

		Context& operator=(Context&& other);

		std::shared_ptr<lp::doc::IDocClass> instantiateDocumentClass(std::string name, Module& module);
	};
}