#include <letterpress/scriptengine/object.hpp>

#include <angelscript.h>

#include <utility>

using namespace lp::script;

Object::Object() noexcept : typeinfo(nullptr), obj(nullptr) {}
Object::Object(asITypeInfo* typeinfo, asIScriptObject* obj) noexcept : typeinfo(typeinfo), obj(obj) {
	if (this->obj != nullptr && isScriptObject())
		this->obj->AddRef();
}
Object::Object(const Object& other) noexcept : typeinfo(other.typeinfo), obj(other.obj) {
	if (this->obj != nullptr && isScriptObject())
		this->obj->AddRef();
}
Object::Object(Object&& other) noexcept : typeinfo(std::move(other.typeinfo)), obj(std::move(other.obj)) {
	other.invalidate();
}
Object::~Object() { destroy(); }

Object& Object::operator=(const Object& other) noexcept {
	destroy();
	this->obj = other.obj;
	return *this;
}

Object& Object::operator=(Object&& other) noexcept {
	destroy();
	this->typeinfo = std::move(other.typeinfo);
	this->obj = std::move(other.obj);
	other.invalidate();
	return *this;
}

void Object::invalidate() noexcept {
	obj = nullptr;
	typeinfo = nullptr;
}

void Object::destroy() noexcept {
	if (obj != nullptr) {
		if (isScriptObject())
			obj->Release();
		invalidate();
	}
}

bool Object::isScriptObject() const noexcept { return obj && (typeinfo->GetFlags() & asOBJ_SCRIPT_OBJECT) != 0; }