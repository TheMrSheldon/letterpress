#include <letterpress/scriptengine/module.hpp>

#include <angelscript.h>

#include <format>
#include <fstream>
#include <iostream>

using namespace lp::script;

class FileOutputStream final : public asIBinaryStream {
private:
	std::ofstream stream;
public:
	FileOutputStream(std::filesystem::path path) : stream(path, std::ios_base::binary | std::ios_base::trunc) {}

	int Write(const void* ptr, asUINT size) override {
		if (size == 0)
			return 0;
		stream.write((char*) ptr, size);
		return 0;
	}

	int Read(void* ptr, asUINT size) override {
		throw std::runtime_error("Unsupported operation");
	}
};

class FileInputStream final : public asIBinaryStream {
private:
	std::ifstream stream;
public:
	FileInputStream(std::filesystem::path path) : stream(path, std::ios_base::binary) {}
	
	int Write(const void* ptr, asUINT size) override {
		throw std::runtime_error("Unsupported operation");
	}

	int Read(void* ptr, asUINT size) override {
		if (stream.fail()) {
			// TODO: log file not found
			std::cout << "File not found" << std::endl;
			return asERROR;
		}
		if (size == 0)
			return asSUCCESS;
		stream.read((char*) ptr, size);
		return asSUCCESS;
	}
};

Module::Module(asIScriptModule* module) : module(module) {}

void Module::saveToFile(std::filesystem::path path) {
	auto stream = FileOutputStream(path);
	module->SaveByteCode(&stream, true);
}

Module Module::LoadFromFile(asIScriptEngine* engine, std::filesystem::path path) {
	auto module = engine->GetModule(path.filename().c_str(), asGM_ALWAYS_CREATE);
	auto stream = FileInputStream(path);
	auto r = module->LoadByteCode(&stream);
	if (r != asSUCCESS)
		throw std::runtime_error(std::format("Failed to load byte code: {}", r));
	return Module(module);
}