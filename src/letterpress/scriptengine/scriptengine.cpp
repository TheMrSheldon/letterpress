#include <letterpress/scriptengine/scriptengine.hpp>

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>

#include <letterpress/document/document.hpp>
#include <letterpress/document/idocclass.hpp>

using namespace lp::script;

ScriptEngine::ScriptEngine() : angelscript(asCreateScriptEngine()) {}

ScriptEngine::ScriptEngine(ScriptEngine&& other) : angelscript(other.angelscript) { other.angelscript = nullptr; }

ScriptEngine::~ScriptEngine() {
	if (angelscript != nullptr)
		angelscript->ShutDownAndRelease();
}

#include <iostream>

bool ScriptEngine::init(lp::doc::Document* doc) {
	int r = angelscript->SetMessageCallback(asMETHOD(lp::script::ScriptEngine, logCallback), this, asCALL_THISCALL);
	if (r < 0)
		return false;
	RegisterStdString(angelscript);

	r = angelscript->RegisterInterface("IDocumentType");
	assert(r >= 0);
	if (doc != nullptr) {
		r = angelscript->RegisterGlobalFunction(
				"void par()", asMETHOD(lp::doc::Document, writeParagraph), asCALL_THISCALL_ASGLOBAL, doc
		);
		assert(r >= 0);
		r = angelscript->RegisterGlobalFunction(
				"void pushFont(string)", asMETHOD(lp::doc::Document, pushFont), asCALL_THISCALL_ASGLOBAL, doc
		);
		assert(r >= 0);
		r = angelscript->RegisterGlobalFunction(
				"void popFont()", asMETHOD(lp::doc::Document, popFont), asCALL_THISCALL_ASGLOBAL, doc
		);
		assert(r >= 0);
	} else {
		r = angelscript->RegisterGlobalFunction("void pushFont(string)", asSFuncPtr(), asCALL_CDECL);
		r = angelscript->RegisterGlobalFunction("void popFont()", asSFuncPtr(), asCALL_CDECL);
	}
	return true;
}

void ScriptEngine::deinit() {
	angelscript->ShutDownAndRelease();
	angelscript = nullptr;
}

Context ScriptEngine::createContext() {
	auto context = angelscript->CreateContext();
	auto ret = Context(context);
	context->Release();
	return ret;
}

Module ScriptEngine::createModule(std::string name, std::vector<std::filesystem::path> files) {
	CScriptBuilder builder;
	auto r = builder.StartNewModule(angelscript, name.c_str());
	if (r < 0)
		throw std::runtime_error("Unrecoverable error while starting a new module.");
	for (auto&& file : files) {
		r = builder.AddSectionFromFile(file.c_str());
		if (r < 0)
			throw std::runtime_error("Please correct the errors in the script and try again.\n");
	}
	if (r < 0)
		throw std::runtime_error("Please correct the errors in the script and try again.\n");
	r = builder.BuildModule();
	if (r < 0)
		throw std::runtime_error("Please correct the errors in the script and try again.\n");
	return Module(builder.GetModule());
}

Module ScriptEngine::loadModule(std::filesystem::path path) { return Module::LoadFromFile(angelscript, path); }

void ScriptEngine::logCallback(const asSMessageInfo* msg, void* param) noexcept {
	switch (msg->type) {
	case asMSGTYPE_ERROR:
		logger->error("{} ({}, {}) : {}", msg->section, msg->row, msg->col, msg->message);
		break;
	case asMSGTYPE_WARNING:
		logger->warn("{} ({}, {}) : {}", msg->section, msg->row, msg->col, msg->message);
		break;
	case asMSGTYPE_INFORMATION:
		logger->info("{} ({}, {}) : {}", msg->section, msg->row, msg->col, msg->message);
		break;
	default:
		abort(); /** \todo more graceful pls **/
	}
}