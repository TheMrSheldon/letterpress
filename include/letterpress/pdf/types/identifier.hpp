#pragma once

#include <string>

namespace lp::pdf {
	class Identifier {
	private:
		std::string value;

	public:
		explicit Identifier(const std::string& value) : value(value) {}

		std::string& get() noexcept { return value; }
		const std::string& get() const noexcept { return value; }
	};
} // namespace lp::pdf