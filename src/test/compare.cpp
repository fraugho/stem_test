#include <stdio.h>
#include <string.h>
#include <libstemmer.h>
#include <fstream>
#include <iostream>
#include <codecvt>
#include "../../lib/ole/english_stem.h"

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
    int ole_wc = 0, snow_wc = 0, line_count;
    std::string word;

    // CSV
    std::ofstream output_file("stem_results.csv");
    if (!output_file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    // Writes the header
    output_file << "Original Word,Oleander,Snowball\n";

    std::ofstream dif_file("stem_dif.csv");
    if (!dif_file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    // Writes the header
    dif_file << "Original Word,Oleander,Snowball\n";

    //TXT
    std::ifstream words("../../data/words.txt");
    if (!words.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // Oleander Stemmer
    stemming::english_stem<> ole_stem_eng;

    // Snowball
    struct sb_stemmer* snow_stemmer = sb_stemmer_new("english", 0);
    if (snow_stemmer == NULL) {
        std::cerr << "Failed to create snowball stemmer" << std::endl;
        return 1;
    }
    bool dif_found = false;

    while(std::getline(words, word)){
        // Convert std::string to std::wstring for Oleander stemmer
        std::wstring w_word = str_to_wstr(word);

        // stem in place
        ole_stem_eng(w_word);
        std::string ole_stem = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(w_word);

        // Get the stemmed result from Snowball
        const sb_symbol* snow_stem = sb_stemmer_stem(snow_stemmer, (const sb_symbol*)word.data(), word.length());

        // Writes to the output file
        output_file << word << "," << ole_stem << "," << snow_stem << "\n";

        int i = 0;
        for(; snow_stem[i] != '\0'; ++i){
            if (ole_stem[i] != snow_stem[i] && !dif_found){
                dif_file << word << "," << ole_stem << "," << snow_stem << "\n";
                dif_found = true;
            }
        }
        snow_wc += i;
        ole_wc += ole_stem.length();
        ++line_count;
        dif_found = false;
    }

    std::cout << "Snowball Average Word Len: " << snow_wc / line_count << "\n";
    std::cout << "Ole Average Word Len: " << ole_wc / line_count << "\n";

    // Clean up
    sb_stemmer_delete(snow_stemmer);
    output_file.close();
    return 0;
}
