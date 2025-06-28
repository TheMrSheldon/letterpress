#ifndef LETTERPRESS_SCRIPTENGINE_SCRIPTINTERFACE_DOCTYPE_HPP
#define LETTERPRESS_SCRIPTENGINE_SCRIPTINTERFACE_DOCTYPE_HPP

#include <angelscript.h>

#include <cassert>

static void registerDocumentTypeInterface(asIScriptEngine* engine) {
	auto r = engine->RegisterInterface("IDocumentType");
	assert(r >= 0);
}

#endif