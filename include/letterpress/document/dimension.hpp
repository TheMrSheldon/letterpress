#ifndef LETTERPRESS_DOCUMENT_DIMENSION_HPP
#define LETTERPRESS_DOCUMENT_DIMENSION_HPP

#include <cinttypes>

namespace lp::doc {

	struct Dimension {
	public:
		enum class Unit { Absolute, EM, EX };

	private:
		Unit unit;
		uint32_t value;

		inline constexpr Dimension(Unit unit, uint32_t value) : unit(unit), value(value) {}

	public:
		inline float getPoint(float em2pt);
		inline float getPica(float em2pt);
		inline float getInch(float em2pt);
		inline float getBigPoint(float em2pt);
		inline float getCentimeter(float em2pt);
		inline float getMillimeter(float em2pt);
		inline float getDidotPoint(float em2pt);
		inline float getCicero(float em2pt);
		inline int32_t getScaledPoint(float em2pt);

		/**
		 * @brief 
         * @details 1 pt = 2^{16}+1 sp
         * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension point(int32_t value) { return Dimension::scaledPoint(value << 16); }
		/**
         * @brief 
         * @details 1pc = 12pt
         * 
         * @param value 
         * @return 
         */
		inline constexpr static Dimension pica(int32_t value) { return Dimension::point(12 * value); }
		inline constexpr static Dimension inch(int32_t value);
		inline constexpr static Dimension bigPoint(int32_t value);
		inline constexpr static Dimension centimeter(int32_t value);
		inline constexpr static Dimension millimeter(int32_t value);
		inline constexpr static Dimension didotPoint(int32_t value);
		inline constexpr static Dimension cicero(int32_t value);
		inline constexpr static Dimension scaledPoint(int32_t value) { return Dimension(Unit::Absolute, value); }

		inline constexpr static Dimension em(int32_t value) { return Dimension(Unit::EM, value); }
		inline constexpr static Dimension ex(int32_t value) { return Dimension(Unit::EX, value); }
	};

	inline constexpr Dimension operator""_pt(unsigned long long value) { return Dimension::point(value); }
	inline constexpr Dimension operator""_pc(unsigned long long value) { return Dimension::pica(value); }
	inline constexpr Dimension operator""_in(unsigned long long value) { return Dimension::inch(value); }
	inline constexpr Dimension operator""_bp(unsigned long long value) { return Dimension::bigPoint(value); }
	inline constexpr Dimension operator""_cm(unsigned long long value) { return Dimension::centimeter(value); }
	inline constexpr Dimension operator""_mm(unsigned long long value) { return Dimension::millimeter(value); }
	inline constexpr Dimension operator""_dd(unsigned long long value) { return Dimension::didotPoint(value); }
	inline constexpr Dimension operator""_cc(unsigned long long value) { return Dimension::cicero(value); }
	inline constexpr Dimension operator""_sp(unsigned long long value) { return Dimension::scaledPoint(value); }

	inline constexpr Dimension operator""_em(unsigned long long value) { return Dimension::em(value); }
	inline constexpr Dimension operator""_ex(unsigned long long value) { return Dimension::ex(value); }

} // namespace lp::doc

#endif