#pragma once

#include <istream>
#include <filesystem>

#include "document/document.hpp"
#include "logging.hpp"
#include "scriptengine/scriptengine.hpp"

namespace lp {
	class Parser final {
	private:
		lp::log::LoggerPtr logger;
		script::ScriptEngine scriptEngine;
	
	public:
		Parser();

		lp::doc::Document parse(const std::filesystem::path& path, std::vector<std::filesystem::path> includeDirs) noexcept;
		lp::doc::Document parse(const std::string& string, std::vector<std::filesystem::path> includeDirs) noexcept;
		lp::doc::Document parse(std::istream& input, std::vector<std::filesystem::path> includeDirs) noexcept;
	};
}