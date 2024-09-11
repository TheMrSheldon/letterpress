#pragma once

#include <filesystem>
#include <istream>

#include "document/document.hpp"
#include "logging.hpp"
#include "scriptengine/scriptengine.hpp"

namespace lp {
	class Driver;

	class Parser final {
	private:
		lp::log::LoggerPtr logger;
		script::ScriptEngine scriptEngine;
		Driver& driver;

	public:
		explicit Parser(Driver& driver);

		lp::doc::Document
		parse(const std::filesystem::path& path, std::vector<std::filesystem::path> includeDirs) noexcept;
		lp::doc::Document parse(const std::string& string, std::vector<std::filesystem::path> includeDirs) noexcept;
		lp::doc::Document parse(std::istream& input, std::vector<std::filesystem::path> includeDirs) noexcept;
	};
} // namespace lp