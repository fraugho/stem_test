#include <stdio.h>
#include <string.h>
#include "libstemmer.h"
#include <fstream>
#include <iostream>
#include <codecvt>
#include "../../lib/ole/french_stem.h"
#include <cwchar>

int test();

int main() {
    test();
    return 0;
}

std::wstring str_to_wstr(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

int test() {
    std::string s_word = "continuellement";
    std::wstring o_word(s_word.size(), L'\0');
    mbstowcs(&o_word[0], s_word.c_str(), s_word.size());;
    //std::wstring o_word = L"continuellement";
    std::wstring og_word = L"continuellement";

    // Oleander Stemmer
    stemming::french_stem<> ole_fre_stem;

    // Snowball
    struct sb_stemmer* snow_stemmer = sb_stemmer_new("french", 0);
    if (snow_stemmer == NULL) {
        std::cerr << "Failed to create snowball stemmer" << std::endl;
        return 1;
    }

    // stem in place
    ole_fre_stem(o_word);
    std::string ole_stem = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(o_word);

    // Get the stemmed result from Snowball
    const sb_symbol* snow_stem = sb_stemmer_stem(snow_stemmer, (const sb_symbol*)s_word.data(), s_word.length());

    // Print results
    std::wcout << "Original Word: " << og_word << "\n";
    std::wcout << "Oleander Stem: " << o_word << "\n";
    std::cout << "Snowball Stem: " << snow_stem << "\n";

    // Clean up
    sb_stemmer_delete(snow_stemmer);
    return 0;
}
