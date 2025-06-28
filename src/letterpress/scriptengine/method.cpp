#include <letterpress/scriptengine/method.hpp>

#include <letterpress/scriptengine/context.hpp>
#include <letterpress/utils/overloaded.hpp>

#include <angelscript.h>

#include <assert.h>

using namespace lp::script;

Method::Method(Context& context, asIScriptFunction* func, asIScriptObject* thisarg)
		: context(context), func(func), thisarg(thisarg) {}

std::expected<lp::LPValue, Error> Method::invoke(lp::LPArray& arguments) {
	context.context->Prepare(func);
	if (thisarg)
		context.context->SetObject(thisarg);
	assert(func->GetParamCount() == arguments.size());
	for (auto i = 0u; i < arguments.size(); ++i) {
		std::visit(
				lp::utils::overloaded{
						[&](const float& f) {
							auto r = context.context->SetArgFloat(i, f);
							assert(r == asEXECUTION_FINISHED);
						},
						[&](const int& n) {
							auto r = context.context->SetArgDWord(i, n);
							assert(r == asEXECUTION_FINISHED);
						},
						[&](std::string& s) {
							auto r = context.context->SetArgObject(i, &s);
							assert(r == asEXECUTION_FINISHED);
						},
						[&](void* ptr) {
							auto r = context.context->SetArgObject(i, ptr);
							assert(r == asEXECUTION_FINISHED);
						}, // Or use SetArgAddress?
						[&](const lp::script::Object& obj) { auto r = context.context->SetArgObject(i, obj.data()); },
						[](const auto&) {
							abort(); /** \todo implement **/
						}
				},
				arguments[i]
		);
	}
	//context.context->SetArgObject(i, &std::get<std::string>(arguments[i]));
	auto r = context.context->Execute();
	if (r == asEXECUTION_FINISHED) {
		auto typeinfo = context.context->GetEngine()->GetTypeInfoById(func->GetReturnTypeId());
		if (typeinfo == nullptr) // Returns void
			return lp::LPValue(nullptr);
		auto obj = static_cast<asIScriptObject*>(context.context->GetReturnObject());
		return lp::LPValue(Object(obj->GetObjectType(), obj));
	} else
		return std::unexpected(Error::GeneralError);
}

const char* Method::name() const noexcept { return func->GetName(); }

std::vector<Param> Method::params() const noexcept {
	std::vector<Param> params(func->GetParamCount());
	for (size_t i = 0; i < func->GetParamCount(); ++i)
		func->GetParam(i, &params[i].typeId);
	return params;
}
