#pragma once

#include <string>
#include <variant>
#include <vector>

namespace lp::pdf {
	class Array {
		using vec = std::vector<std::variant<std::string, float>>;

	private:
		vec value;

	public:
		Array() : value() {}
		explicit Array(const vec& value) : value(value) {}

		const vec& get() const noexcept { return value; }

		void append(std::string element) { value.emplace_back(element); }
		void append(float element) { value.emplace_back(element); }
	};
} // namespace lp::pdf