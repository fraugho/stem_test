#include <stdio.h>
#include <string.h>
//#include "snow_lib/libstemmer/modules.h"
//#include "snow_lib/include/libstemmer.h"
#include "libstemmer.h"
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>
#include "../../lib/ole/english_stem.h"
#include "../../lib/ole/french_stem.h"
#include "../../lib/ole/russian_stem.h"

enum Language{
    English,
    French,
    Russian,
};

int lang_test(Language lang);

int main() {
    //lang_test(Language::English);
    std::locale::global(std::locale("fr_FR.UTF-8")); // or "fr_FR" depending on your system
    std::wcout.imbue(std::locale());
    lang_test(Language::French);
    //lang_test(Language::Russian);
    return 0;
}

std::wstring str_to_wstr(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

int lang_test(Language lang) {
    int ole_wc = 0, snow_wc = 0, line_count;

    std::string word;

    std::string output_file_name;
    std::string dif_file_name;
    std::string data_file_name;

    struct sb_stemmer* snow_stemmer;
    switch(lang){
        case English:
            output_file_name ="eng_stem.csv";
            dif_file_name = "eng_stem_dif.csv";
            data_file_name = "../../data/english.txt";
            snow_stemmer = sb_stemmer_new("english", 0);
            if (snow_stemmer == NULL) {
                std::cerr << "Failed to create snowball stemmer" << std::endl;
                return 1;
            }
            break;
        case French:
            output_file_name ="fre_stem.csv";
            dif_file_name = "fre_stem_dif.csv";
            data_file_name = "../../data/french.txt";
            snow_stemmer = sb_stemmer_new("french", 0);
            if (snow_stemmer == NULL) {
                std::cerr << "Failed to create snowball stemmer" << std::endl;
                return 1;
            }
            break;
        case Russian:
            output_file_name ="rus_stem.csv";
            dif_file_name = "rus_stem_dif.csv";
            data_file_name = "../../data/russian.txt";
            snow_stemmer = sb_stemmer_new("russian", 0);
            if (snow_stemmer == NULL) {
                std::cerr << "Failed to create snowball stemmer" << std::endl;
                return 1;
            }
            break;
    }

    // CSV
    std::ofstream output_file(output_file_name);
    if (!output_file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    // Writes the header
    output_file << "Original Word,Oleander,Snowball\n";

    std::ofstream dif_file(dif_file_name);
    if (!dif_file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    // Writes the header
    dif_file << "Original Word,Oleander,Snowball\n";

    //TXT
    std::ifstream words(data_file_name);
    if (!words.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // Oleander Stemmer
    stemming::english_stem<> ole_eng_stem;
    stemming::russian_stem<> ole_rus_stem;
    stemming::french_stem<> ole_fre_stem;

    bool dif_found = false;

    while(std::getline(words, word)){
        // Convert std::string to std::wstring for Oleander stemmer
        std::wstring w_word = str_to_wstr(word);

        // stem in place
        switch(lang){
            case English:
                ole_eng_stem(w_word);
                break;
            case French:
                ole_fre_stem(w_word);
                break;
            case Russian:
                ole_rus_stem(w_word);
                break;
        }
        std::string ole_stem = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(w_word);

        // Get the stemmed result from Snowball
        const sb_symbol* snow_stem = sb_stemmer_stem(snow_stemmer, (const sb_symbol*)word.data(), word.length());

        // Writes to the output file
        output_file << word << "," << ole_stem << "," << snow_stem << "\n";

        std::string snow_str(reinterpret_cast<const char*>(snow_stem));
        if (ole_stem != snow_str) {
            dif_file << word << "," << ole_stem << "," << snow_str << "\n";
        }

        snow_wc += snow_str.length();
        ole_wc += ole_stem.length();
        ++line_count;
    }

    std::cout << "Snowball Average Word Len: " << (float)snow_wc / line_count << "\n";
    std::cout << "Ole Average Word Len: " << (float)ole_wc / line_count << "\n";

    // Clean up
    sb_stemmer_delete(snow_stemmer);
    output_file.close();
    dif_file.close();
    return 0;
}
