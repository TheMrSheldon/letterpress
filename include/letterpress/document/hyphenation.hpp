#pragma once

#include <filesystem>
#include <map>
#include <vector>

namespace lp::doc {
	class Hyphenation final {
	private:
		std::map<std::string, std::vector<uint8_t>> patterns;
		size_t maxPatternLen;

	public:
		Hyphenation() noexcept;
		/**
		 * @brief Adds a new hyphenation pattern.
		 * @details A pattern consists of DLDL...DLD where D is a digit and L a letter.
		 */
		void addPattern(const std::string& pattern) noexcept;
		std::vector<std::string> hyphenate(std::string word) const noexcept;

		static Hyphenation fromFile(std::filesystem::path file) noexcept;
	};
} // namespace lp::doc