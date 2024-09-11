#ifndef LETTERPRESS_DOCUMENT_DIMENSION_HPP
#define LETTERPRESS_DOCUMENT_DIMENSION_HPP

#include <cassert>
#include <cinttypes>
#include <concepts>

namespace lp::doc {

	struct Dimension {
	private:
		using inttype = int64_t;
		using fptype = double;
		static constexpr auto pt2sp = 65536;
		static constexpr auto pc2sp = 12 * pt2sp;
		// 1in = 72.27pt = (72.27 * (2^{16}+1))sp \approx 4736359sp
		static constexpr auto in2sp = 4736359;
		// 1bp = 1/72 in = (72.27 / 72)pt = ((72.27*(2^{16}+1)) / 72)sp \approx 65783sp
		static constexpr auto bp2sp = 65783;
		// 1mm = 0.1cm = (0.1 / 2.54)in = ((0.1 * 72.27) / 2.54) pt = ((7.27*(2^{16}+1)) / 2.54)sp \approx 187580sp
		static constexpr auto mm2sp = 187580;
		static constexpr auto cm2sp = 10 * mm2sp;
		// 1dd = (1238 / 1157)pt = ((1238 * 2^{16}+1)/1157)sp \approx 70124sp
		static constexpr auto dd2sp = 70124;
		static constexpr auto cc2sp = 12 * dd2sp;

	private:
		inttype _abs; /**< The absolute size measured in scaled points */
		fptype _em;
		fptype _ex;

		inline constexpr Dimension(inttype abs, fptype em, fptype ex) noexcept : _abs(abs), _em(em), _ex(ex) {}

	public:
		inline constexpr Dimension() noexcept : Dimension(0, 0, 0) {}
		inline constexpr Dimension(const Dimension& other) noexcept : Dimension(other._abs, other._em, other._ex) {}
		inline constexpr Dimension& operator=(const Dimension& other) noexcept {
			_abs = other._abs;
			_em = other._em;
			_ex = other._ex;
			return *this;
		}

		inline constexpr Dimension& resolve(fptype em2pt = 0, fptype ex2pt = 0) noexcept {
			_abs = getScaledPoint(em2pt, ex2pt);
			_em = _ex = 0;
			return *this;
		}

		inline fptype getPoint(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)pt2sp;
		}
		inline fptype getPica(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)pc2sp;
		}
		inline fptype getInch(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)in2sp;
		}
		inline fptype getBigPoint(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)bp2sp;
		}
		inline fptype getCentimeter(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)cm2sp;
		}
		inline fptype getMillimeter(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)mm2sp;
		}
		inline fptype getDidotPoint(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)dd2sp;
		}
		inline fptype getCicero(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			return getScaledPoint(em2pt, ex2pt) / (fptype)cc2sp;
		}
		inline inttype getScaledPoint(fptype em2pt = 0, fptype ex2pt = 0) const noexcept {
			assert((em2pt != 0 || _em == 0) && (ex2pt != 0 || _ex == 0));
			return _abs + (_em * em2pt * pt2sp) + (_ex * ex2pt * pt2sp);
		}

		/**
		 * @brief 
         * @details 1 pt = 65536 sp = 2^{16} sp
         * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension point(inttype value) { return Dimension::scaledPoint(value * pt2sp); }
		/**
         * @brief 
         * @details 1pc = 12pt
         * 
         * @param value 
         * @return 
         */
		inline constexpr static Dimension pica(inttype value) { return Dimension::scaledPoint(value * pc2sp); }
		/**
		 * @brief 
		 * @details 1in = 72.27pt
		 * @attention We use TeX's definition of 1in = 72.27pt, which slightly approximates the officially defined
		 * conversion of 0.013837in = 1pt.
		 * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension inch(inttype value) { return Dimension::scaledPoint(value * in2sp); }
		/**
		 * @brief 
		 * @details We use TeX's definition of 72bp = 1in.
		 * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension bigPoint(inttype value) { return Dimension::scaledPoint(value * bp2sp); }
		/**
		 * @brief 
		 * @details 2.54cm = 1in
		 * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension centimeter(inttype value) { return Dimension::scaledPoint(value * cm2sp); }

		/**
		 * @brief 
		 * @details 10mm = 1cm
		 * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension millimeter(inttype value) { return Dimension::scaledPoint(value * mm2sp); }
		/**
		 * @brief 
		 * @details 1157dd = 1238pt
		 * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension didotPoint(inttype value) { return Dimension::scaledPoint(value * dd2sp); }
		/**
		 * @brief 
		 * @details 1cc = 12dd
		 * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension cicero(inttype value) { return Dimension::scaledPoint(value * cc2sp); }
		/**
		 * @brief 
		 * @details Internally, all Dimensions are represented in terms of scaled points. 1pt corresponds to 65536sp and
		 * 
		 * 
		 * @param value 
		 * @return 
		 */
		inline constexpr static Dimension scaledPoint(inttype value) { return Dimension(value, 0, 0); }

		inline constexpr static Dimension em(fptype value) { return {0, value, 0}; }
		inline constexpr static Dimension ex(fptype value) { return {0, 0, value}; }

		inline constexpr Dimension operator+(const Dimension& right) const noexcept {
			return {_abs + right._abs, _em + right._em, _ex + right._ex};
		}
		inline constexpr Dimension operator-(const Dimension& right) const noexcept {
			return {_abs - right._abs, _em - right._em, _ex - right._ex};
		}
		inline constexpr Dimension operator-() const noexcept { return {-_abs, -_em, -_ex}; }
		inline constexpr Dimension& operator+=(const Dimension& right) noexcept {
			_abs += right._abs;
			_em += right._em;
			_ex += right._ex;
			return *this;
		}

		inline constexpr Dimension& operator-=(const Dimension& right) noexcept {
			_abs -= right._abs;
			_em -= right._em;
			_ex -= right._ex;
			return *this;
		}
		inline constexpr Dimension operator*(const std::integral auto& right) const noexcept {
			return {_abs * right, _em * right, _ex * right};
		}
		inline constexpr Dimension operator*(const std::floating_point auto& right) const noexcept {
			return {_abs * right, _em * right, _ex * right};
		}
		inline constexpr Dimension operator/(const std::integral auto& right) const noexcept {
			return {_abs / right, _em / right, _ex / right};
		}
		inline constexpr Dimension operator/(const std::floating_point auto& right) const noexcept {
			return {_abs / right, _em / right, _ex / right};
		}

		inline constexpr bool operator==(const Dimension& right) const noexcept {
			return _abs == right._abs && _em == right._em && _ex == right._ex;
		}

		inline constexpr bool operator<(const Dimension& right) const noexcept {
			auto compAbs = _abs || right._abs;
			auto compEm = _em || right._em;
			auto compEx = _ex || right._ex;
			auto num = compAbs + compEm + compEx;
			assert(num <= 1); // When comparing dimensions, at moste one value may be set.
			if (num == 0)
				return false;
			else if (compAbs)
				return _abs < right._abs;
			else if (compEm)
				return _em < right._em;
			else if (compEx)
				return _ex < right._ex;
			return false; // Both 0
		}
		inline constexpr bool operator<=(const Dimension& right) const noexcept {
			return *this < right || *this == right;
		}
		inline constexpr bool operator>(const Dimension& right) const noexcept { return !(*this <= right); }
		inline constexpr bool operator>=(const Dimension& right) const noexcept { return !(*this < right); }
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
	inline constexpr Dimension operator""_em(long double value) { return Dimension::em(value); }
	inline constexpr Dimension operator""_ex(unsigned long long value) { return Dimension::ex(value); }
	inline constexpr Dimension operator""_ex(long double value) { return Dimension::ex(value); }

} // namespace lp::doc

#endif