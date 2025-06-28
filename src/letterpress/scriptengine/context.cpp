#include <letterpress/scriptengine/context.hpp>

#include <letterpress/scriptengine/method.hpp>
#include <letterpress/scriptengine/module.hpp>
#include <letterpress/scriptengine/scriptengine.hpp>
#include <letterpress/utils/overloaded.hpp>

#include <angelscript.h>
#include <scriptdictionary/scriptdictionary.h>

using namespace lp::script;
using lp::doc::IDocClass;
using lp::doc::ILayout;

/**
 * @brief Populates the given AngelScript dictionary, \p dict from the data in \p data.
 * @brief The source data, \p data , must not be deleted befored the corresponding entries are removed from dict. E.g.,
 * values of type std::string are pointed at in the script object.
 * 
 * @param ctxt 
 * @param dict 
 * @param data 
 */
void populateFromSTL(asIScriptContext* ctxt, CScriptDictionary* dict, lp::LPDict& data) {
	for (auto& [key, value] : data)
		std::visit(
				lp::utils::overloaded{
						[&dict, key](float val) { dict->Set(key, val); },
						[&dict, key, ctxt](std::string& val) {
							auto typeId = ctxt->GetEngine()->GetTypeIdByDecl("string");
							dict->Set(key, static_cast<void*>(&val), typeId);
						},
						[](auto& val) {
							/** \todo implement **/
						}
				},
				value
		);
}

class ScriptDocClass final : public IDocClass {
private:
	asIScriptObject* obj;

public:
	explicit ScriptDocClass(asIScriptObject* obj) : obj(obj) { obj->AddRef(); }
	~ScriptDocClass() {
		if (obj != nullptr)
			obj->Release();
		obj = nullptr;
	}

	virtual ILayout& getLayout() override {
		throw std::runtime_error("not implemented"); //TODO: implement
	}
};

Context::Context(asIScriptContext* context) : context(context) {
	if (context)
		context->AddRef();
}

Context::Context(Context&& other) : context(std::move(other.context)) { other.context = nullptr; }

Context::~Context() { destroy(); }

Context& Context::operator=(Context&& other) {
	destroy();
	context = std::move(other.context);
	other.context = nullptr;
	return *this;
}

void Context::destroy() {
	if (context)
		context->Release();
	context = nullptr;
}

std::shared_ptr<IDocClass> Context::instantiateDocumentClass(
		const std::string& name, const lp::LPDict& args, Module& module, asIScriptObject*& obj
) {
	auto type = module.module->GetTypeInfoByName(name.c_str());
	if (type == nullptr)
		return nullptr;
	auto decl = std::format("{0} @{0}(dictionary)", name);
	auto factory = type->GetFactoryByDecl(decl.c_str());
	if (factory == nullptr)
		return nullptr;
	context->Prepare(factory);
	auto dict = CScriptDictionary::Create(context->GetEngine());
	/** We need to copy here since the API should pass a const reference to the arguments but the std::string value must
	 *  be non-const referenced. */
	lp::LPDict cpy = args;
	populateFromSTL(context, dict, cpy);
	context->SetArgObject(0, dict);
	switch (context->Execute()) {
	case asEXECUTION_FINISHED:
		break;
	case asEXECUTION_EXCEPTION:
		printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
		/** \todo Maybe add the option to continue on errors **/
		dict->Release();
		return nullptr;
	default:
		dict->Release();
		return nullptr;
	}
	dict->Release();
	obj = *static_cast<asIScriptObject**>(context->GetAddressOfReturnValue());
	return std::make_shared<ScriptDocClass>(obj);
}

std::expected<Method, Error> Context::getMethod(const std::string& name, asIScriptObject* obj) {
	auto func = obj->GetObjectType()->GetMethodByName(name.c_str());
	if (func != nullptr)
		return Method(*this, func, obj);
	return std::unexpected(Error::NotFound);
}
std::expected<Method, Error> Context::getMethod(const std::string& name, Module& module) {
	auto func = module.module->GetFunctionByName(name.c_str());
	if (func != nullptr)
		return Method(*this, func);
	return std::unexpected(Error::NotFound);
}
std::expected<Method, Error> Context::getMethod(const std::string& name, ScriptEngine& engine) {
	for (size_t i = 0; i < engine.angelscript->GetGlobalFunctionCount(); ++i) {
		auto func = engine.angelscript->GetGlobalFunctionByIndex(i);
		if (func->GetName() == name)
			return Method(*this, func);
	}
	return std::unexpected(Error::NotFound);
}

std::expected<lp::LPValue, Error> Context::construct(int typeId, const std::string& decl, LPArray& params) {
	assert((typeId & asTYPEID_MASK_OBJECT) != 0);
	auto typeinfo = context->GetEngine()->GetTypeInfoById(typeId);
	if (!typeinfo)
		return std::unexpected(Error::TypeNotFound);
	asIScriptFunction* func = nullptr;
	for (size_t i = 0; i < typeinfo->GetBehaviourCount(); ++i) {
		func = typeinfo->GetBehaviourByIndex(i, nullptr);
		if (decl == func->GetDeclaration(false))
			break;
		func = nullptr;
	}
	if (!func)
		return std::unexpected(Error::FunctionNotFound);

	/** \todo create the object to be constructed **/

	asIScriptObject* obj = static_cast<asIScriptObject*>(context->GetEngine()->CreateScriptObject(typeinfo));

	Method(*this, func, obj).invoke(params);
	auto value = lp::LPValue(lp::script::Object{typeinfo, obj});
	if (std::get<lp::script::Object>(value).isScriptObject())
		obj->Release();
	return value;
}