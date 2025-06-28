#ifndef LETTERPRESS_PARSER_PARSER_HPP
#define LETTERPRESS_PARSER_PARSER_HPP

#include <cassert>
#include <istream>
#include <iterator>
#include <queue>
#include <regex>

namespace lp {
	class GenericParser;

	struct ParserLookAheadIter {
	private:
		unsigned offset;
		GenericParser* parser;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		//using value_type = char32_t;
		using value_type = char; // for now
		using difference_type = value_type;
		using pointer = value_type*;
		using reference = value_type&;

		ParserLookAheadIter(unsigned offset = 0, GenericParser* parser = nullptr) : offset(offset), parser(parser) {}

		value_type operator*() const noexcept;

		ParserLookAheadIter& operator++() noexcept {
			++offset;
			return *this;
		}

		ParserLookAheadIter operator++(int) noexcept {
			ParserLookAheadIter copy{offset, parser};
			operator++();
			return copy;
		}

		ParserLookAheadIter& operator--() noexcept {
			assert(offset != 0);
			--offset;
			return *this;
		}

		ParserLookAheadIter operator--(int) noexcept {
			ParserLookAheadIter copy{offset, parser};
			operator--();
			return copy;
		}

		bool operator==(const ParserLookAheadIter& other) const noexcept {
			return (parser == nullptr && other.parser == nullptr) || (parser == other.parser && offset == other.offset);
		}
	};

	// static_assert(std::bidirectional_iterator<ParserLookAheadIter>);

	class GenericParser {
		friend ParserLookAheadIter;

	public: /** \todo make private **/
		static constexpr char32_t eof = U'\0';
		std::istream& stream;
		std::deque<char32_t> lookAheadBuffer;

		const char32_t& lookAhead(unsigned offset) noexcept {
			while (offset >= lookAheadBuffer.size()) {
				/** \todo add support for utf8 **/
				char c;
				stream.read(&c, 1);
				if (!stream)
					return eof;
				lookAheadBuffer.push_back(c);
			}
			return lookAheadBuffer[offset];
		}

		char32_t advance() noexcept {
			auto c = lookAhead(0);
			if (c != eof)
				lookAheadBuffer.pop_front();
			return c;
		}
		char32_t advance(size_t n) noexcept {
			char32_t c = -1;
			for (size_t i = 0; i < n; ++i)
				c = advance();
			return c;
		}

		size_t lookAheadMatches(const std::regex& regex) noexcept {
			auto begin = ParserLookAheadIter(0, this);
			auto end = ParserLookAheadIter();
			std::match_results<ParserLookAheadIter> m;
			bool match = std::regex_search(begin, end, m, regex, std::regex_constants::match_continuous);
			return match ? m.length() : 0;
		}

		template <class Pred>
		constexpr void skipWhile(Pred predicate) {
			while (predicate(lookAhead(0)))
				advance();
		}

		constexpr std::string readNext(size_t len) noexcept {
			lookAhead(len); // Ensure enough data in lookAhead buffer
			len = std::min(len, lookAheadBuffer.size());
			std::string data(lookAheadBuffer.begin(), lookAheadBuffer.begin() + len);
			advance(len);
			return data;
		}

	public:
		explicit GenericParser(std::istream& stream) : stream(stream) {}
	};
} // namespace lp

#endif