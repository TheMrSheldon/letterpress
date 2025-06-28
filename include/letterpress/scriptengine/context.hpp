#pragma once

#include "../document/idocclass.hpp"
#include "../parser/types.hpp"
#include "./error.hpp"

#include <any>
#include <expected>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

class asIScriptContext;
class asIScriptEngine;
class asIScriptObject;

namespace lp::script {
	class Module;
	class ScriptEngine;
	class Method;

	class Context final {
		friend Method;

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

		std::shared_ptr<lp::doc::IDocClass> instantiateDocumentClass(
				const std::string& name, const lp::LPDict& args, Module& module, asIScriptObject*& obj
		);

		std::expected<Method, Error> getMethod(const std::string& name, asIScriptObject* obj);
		std::expected<Method, Error> getMethod(const std::string& name, Module& module);
		std::expected<Method, Error> getMethod(const std::string& name, ScriptEngine& engine);

		/**
		 * @brief Constructs a new type of the given typeid by calling a constructor that takes the types given in
		 * \p decl .
		 * 
		 * @param typeId 
		 * @param paramTypes 
		 * @param params 
		 */
		std::expected<lp::LPValue, Error> construct(int typeId, const std::string& decl, lp::LPArray& params);
	};
} // namespace lp::script