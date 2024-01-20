#include <letterpress/scriptengine/context.hpp>

#include <letterpress/scriptengine/module.hpp>

#include <angelscript.h>

using namespace lp::script;

class ScriptDocClass final : public lp::doc::IDocClass {
private:
	asIScriptObject* obj;

public:
	ScriptDocClass(asIScriptObject* obj) : obj(obj) {
		obj->AddRef();
	}
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

Context::Context(Context&& other) : context(std::move(other.context)) {
	other.context = nullptr;
}

Context::~Context() {
	destroy();
}

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
	auto factory = type->GetFactoryByDecl((name+" @"+name+"()").c_str());
	if (factory == nullptr)
		return nullptr;
	context->Prepare(factory);
	context->Execute();
	auto obj = *(asIScriptObject**)context->GetAddressOfReturnValue();
	return std::make_shared<ScriptDocClass>(obj);
}
