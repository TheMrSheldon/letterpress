#include <letterpress/scriptengine/scriptengine.hpp>

#include <iostream> /** \todo remove */

#include <angelscript.h>
#include <scriptarray/scriptarray.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptstdstring/scriptstdstring.h>

#include <letterpress/document/document.hpp>
#include <letterpress/document/idocclass.hpp>
#include <letterpress/parser/lpparser.hpp>
#include <letterpress/scriptengine/method.hpp>

#include "./scriptinterface/doctype.hpp"

using namespace lp::script;

ScriptEngine::ScriptEngine() : angelscript(asCreateScriptEngine()) {}

ScriptEngine::ScriptEngine(ScriptEngine&& other) : angelscript(other.angelscript) { other.angelscript = nullptr; }

ScriptEngine::~ScriptEngine() {
	if (angelscript != nullptr)
		angelscript->ShutDownAndRelease();
}

void writeString(lp::doc::Document* d, std::string s) {
	for (char c : s)
		d->addCharacter(c);
}

void writeGlue(lp::doc::Document* d, int idealwidth, int stretchability, int shrinkability) {
	/** \todo allow for dimensions instead of int **/
	d->addGlue(
			{.idealwidth = lp::doc::Dimension::point(idealwidth),
			 .stretchability = lp::doc::Dimension::point(stretchability),
			 .shrinkability = lp::doc::Dimension::point(shrinkability)}
	);
}

void loginfo(spdlog::logger* logger, std::string msg) { logger->info(msg); }

static void registerParserClass(asIScriptEngine* engine, lp::log::LoggerPtr logger) {
	auto r = engine->RegisterInterface("parser");
	assert(r >= 0);
	// r = engine->RegisterInterfaceMethod("parser", "lookAhead(unsigned)");
	// assert(r >= 0);
	/** \todo implement **/
	logger->trace("parser was registered with typeid: {}", engine->GetTypeIdByDecl("parser"));
}

static void registerGlueClass(asIScriptEngine* engine, lp::log::LoggerPtr logger) {
	auto r = engine->RegisterObjectType("glue", sizeof(lp::doc::Glue), asOBJ_VALUE | asGetTypeTraits<lp::doc::Glue>());
	assert(r >= 0);
	auto factory = +[](lp::doc::Glue* glue, lp::LPParser& parser) {
		/** \todo implement **/
		return lp::doc::Glue{
				.idealwidth = lp::doc::Dimension::centimeter(1),
				.stretchability = lp::doc::Dimension::centimeter(2),
				.shrinkability = lp::doc::Dimension::centimeter(3)
		};
	};
	r = engine->RegisterObjectBehaviour(
			"glue", asBEHAVE_CONSTRUCT, "void f(parser&)", asFUNCTION(factory), asCALL_CDECL_OBJFIRST
	);
	assert(r >= 0);
	logger->trace("glue was registered with typeid: {}", engine->GetTypeIdByDecl("glue"));
}

bool ScriptEngine::init(lp::doc::Document* doc) {
	int r = angelscript->SetMessageCallback(asMETHOD(lp::script::ScriptEngine, logCallback), this, asCALL_THISCALL);
	if (r < 0)
		return false;
	RegisterStdString(angelscript);
	logger->trace("string was registered with typeid: {}", angelscript->GetTypeIdByDecl("string"));
	RegisterScriptArray(angelscript, true); // Required for scriptdictionary
	RegisterScriptDictionary(angelscript);

	registerDocumentTypeInterface(angelscript);
	registerParserClass(angelscript, logger);
	registerGlueClass(angelscript, logger);
	// Register string constructor that takes a parser
	angelscript->RegisterObjectBehaviour(
			"string", asBEHAVE_CONSTRUCT, "void f(parser&)", asFUNCTION(+[](std::string* self, lp::LPParser* parser) {
				new (self) std::string{parser->readBetween('{', '}')};
			}),
			asCALL_CDECL_OBJFIRST
	);
	//

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
		r = angelscript->RegisterGlobalFunction(
				"void writeString(string)", asFunctionPtr(writeString), asCALL_CDECL_OBJFIRST, doc
		);
		assert(r >= 0);
		r = angelscript->RegisterGlobalFunction(
				"void writeGlue(int,int,int)", asFunctionPtr(writeGlue), asCALL_CDECL_OBJFIRST, doc
		);
		assert(r >= 0);
		/** Logging Calls **/
		r = angelscript->RegisterGlobalFunction(
				"void loginfo(string)", asFunctionPtr(loginfo), asCALL_CDECL_OBJFIRST, logger.get()
		);
		assert(r >= 0);
	} else {
		r = angelscript->RegisterGlobalFunction("void par()", asSFuncPtr(), asCALL_CDECL);
		assert(r >= 0);
		r = angelscript->RegisterGlobalFunction("void pushFont(string)", asSFuncPtr(), asCALL_CDECL);
		assert(r >= 0);
		r = angelscript->RegisterGlobalFunction("void popFont()", asSFuncPtr(), asCALL_CDECL);
		assert(r >= 0);
		r = angelscript->RegisterGlobalFunction("void writeString(string)", asSFuncPtr(), asCALL_CDECL);
		assert(r >= 0);
		r = angelscript->RegisterGlobalFunction("void writeGlue(int,int,int)", asSFuncPtr(), asCALL_CDECL);
		assert(r >= 0);
		/** Logging Calls **/
		r = angelscript->RegisterGlobalFunction("void loginfo(string)", asSFuncPtr(), asCALL_CDECL);
		assert(r >= 0);
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