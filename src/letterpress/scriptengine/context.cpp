#include <letterpress/scriptengine/context.hpp>

#include <letterpress/scriptengine/module.hpp>
#include <letterpress/scriptengine/scriptengine.hpp>

#include <angelscript.h>

using namespace lp::script;

class ScriptDocClass final : public lp::doc::IDocClass {
private:
	asIScriptObject* obj;

public:
	ScriptDocClass(asIScriptObject* obj) : obj(obj) { obj->AddRef(); }
	~ScriptDocClass() {
		if (obj != nullptr)
			obj->Release();
		obj = nullptr;
	}

	virtual lp::doc::ILayout& getLayout() override {
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

std::shared_ptr<lp::doc::IDocClass> Context::instantiateDocumentClass(std::string name, Module& module) {
	auto type = module.module->GetTypeInfoByName(name.c_str());
	if (type == nullptr)
		return nullptr;
	auto factory = type->GetFactoryByDecl((name + " @" + name + "()").c_str());
	if (factory == nullptr)
		return nullptr;
	context->Prepare(factory);
	context->Execute();
	auto obj = *(asIScriptObject**)context->GetAddressOfReturnValue();
	return std::make_shared<ScriptDocClass>(obj);
}

bool Context::invokeMethod(std::string name, std::vector<std::string> arguments, Module& module) {
	/** \todo fix hardcoded decl **/
	auto func = module.module->GetFunctionByDecl((std::string("void ") + name + "(string)").c_str());
	if (func == nullptr)
		return false;
	context->Prepare(func);
	for (auto i = 0u; i < arguments.size(); ++i)
		context->SetArgObject(i, &arguments[i]);
	context->Execute();
	return true;
}

bool Context::invokeMethod(
		std::string name, std::vector<std::string> arguments, lp::doc::Document& document, ScriptEngine& engine
) {
	/** \todo support non-global functions **/
	/** \todo fix hardcoded decl **/
	std::string params = "";
	for (auto& arg : arguments)
		params += std::string(params.size() > 0 ? "," : "") + "string";
	auto func = engine.angelscript->GetGlobalFunctionByDecl((std::string("void ") + name + "(" + params + ")").c_str());
	if (func == nullptr)
		return false;
	context->Prepare(func);
	for (auto i = 0u; i < arguments.size(); ++i)
		context->SetArgObject(i, &arguments[i]);
	context->Execute();
	return true;
}