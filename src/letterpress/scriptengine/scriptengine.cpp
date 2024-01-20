#include <letterpress/scriptengine/scriptengine.hpp>

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>

#include <letterpress/document/idocclass.hpp>

using namespace lp::script;


void MessageCallback(const asSMessageInfo* msg, void* param) {
	const char* type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}


ScriptEngine::ScriptEngine() : angelscript(asCreateScriptEngine()) {
	
}

ScriptEngine::ScriptEngine(ScriptEngine&& other) : angelscript(other.angelscript) {
	other.angelscript = nullptr;
}

ScriptEngine::~ScriptEngine() {
	if (angelscript != nullptr)
		angelscript->ShutDownAndRelease();
}


static void println(std::string string) {
	printf("%s\n", string.c_str());
}


bool ScriptEngine::init() {
	int r = angelscript->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	if (r != 0)
		return false;
	RegisterStdString(angelscript);

	r = angelscript->RegisterInterface("IDocumentType");
	angelscript->RegisterGlobalFunction("void println(string)", asFUNCTION(println), asCALL_CDECL);
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

Module ScriptEngine::loadModule(std::filesystem::path path) {
	return Module::LoadFromFile(angelscript, path);
}