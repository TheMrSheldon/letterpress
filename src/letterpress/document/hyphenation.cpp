#include <letterpress/document/hyphenation.hpp>

#include <assert.h>
#include <fstream>

using Hyphenation = lp::doc::Hyphenation;

Hyphenation::Hyphenation() noexcept : maxPatternLen(0) {}

void Hyphenation::addPattern(const std::string& pattern) noexcept {
    std::vector<uint8_t> nums;
    std::string str;
    
    auto s = pattern.begin();
    nums.push_back(std::isdigit(*s)? (*(s++)-'0'): 0);
    for (; s < pattern.end();) {
        str += *s;
        ++s;
        nums.push_back(std::isdigit(*s)? (*(s++)-'0'): 0);
    }
    maxPatternLen = std::max(maxPatternLen, str.length());
    patterns[str] = nums;
}

#include <iostream>

std::vector<std::string> Hyphenation::hyphenate(std::string word) const noexcept {
    word = '.'+word+'.';
    uint8_t nums[word.length()+2] = {0};
    for (int length = 1; length <= maxPatternLen; ++length) {
        for (auto start = 0; start+length <= word.length(); start ++) {
            auto substr = word.substr(start, length);
            auto pattern = patterns.find(substr);
            if (pattern != patterns.end()) {
                for (int i = 0; i < length; ++i)
                    nums[start+i] = std::max(nums[start+i], pattern->second[i]);
            }
        }
    }
    std::vector<std::string> result;
    auto start = 1;
    // count i from start+1 since there can't be a hyphen before the first character
    for (int i = start+1; i < word.length(); ++i) {
        if (nums[i] % 2 == 1) {
            result.push_back(word.substr(start, i-start));
            start = i;
        }
    }
    result.push_back(word.substr(start, word.length()-1-start));
    return result;
}

Hyphenation Hyphenation::fromFile(std::filesystem::path file) noexcept {
    assert(std::filesystem::is_regular_file(file));
    Hyphenation result;
    std::ifstream is(file);
    for (std::string line; std::getline(is, line);)
        result.addPattern(line);
    return std::move(result);
}