#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>
#include "libstemmer.h"
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
    std::locale::global(std::locale("fr_FR.UTF-8"));
    std::wcout.imbue(std::locale());
    lang_test(Language::French);
    return 0;
}

// Convert wstring to UTF-8 for Snowball stemmer
std::string wstr_to_utf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// Convert UTF-8 string to wstring
std::wstring utf8_to_wstr(const std::string& utf8str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8str);
}

int lang_test(Language lang) {
    int ole_wc = 0, snow_wc = 0, line_count = 0;

    std::wstring word;
    std::string word_utf8;

    std::string output_file_name;
    std::string dif_file_name;
    std::string data_file_name;

    struct sb_stemmer* snow_stemmer;
    switch(lang){
        case English:
            output_file_name = "eng_stem.csv";
            dif_file_name = "eng_stem_dif.csv";
            data_file_name = "../../data/english.txt";
            snow_stemmer = sb_stemmer_new("english", 0);
            if (snow_stemmer == NULL) {
                std::wcerr << L"Failed to create snowball stemmer" << std::endl;
                return 1;
            }
            break;
        case French:
            output_file_name = "fre_stem.csv";
            dif_file_name = "fre_stem_dif.csv";
            data_file_name = "../../data/french.txt";
            snow_stemmer = sb_stemmer_new("french", 0);
            if (snow_stemmer == NULL) {
                std::wcerr << L"Failed to create snowball stemmer" << std::endl;
                return 1;
            }
            break;
        case Russian:
            output_file_name = "rus_stem.csv";
            dif_file_name = "rus_stem_dif.csv";
            data_file_name = "../../data/russian.txt";
            snow_stemmer = sb_stemmer_new("russian", 0);
            if (snow_stemmer == NULL) {
                std::wcerr << L"Failed to create snowball stemmer" << std::endl;
                return 1;
            }
            break;
    }

    // Read input as UTF-8, process as wstring, write as UTF-16
    std::ifstream input_file(data_file_name.c_str());
    if (!input_file.is_open()) {
        std::wcerr << L"Error opening input file!" << std::endl;
        return 1;
    }

    // CSV output files using wide streams with UTF-16 encoding
    std::wofstream output_file(output_file_name.c_str(), std::ios::binary);
    if (!output_file.is_open()) {
        std::wcerr << L"Error opening output file!" << std::endl;
        return 1;
    }
    // Set UTF-16 locale with BOM
    std::locale utf16_locale(std::locale(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::generate_header>);
    output_file.imbue(utf16_locale);
    // Write the header with quotes
    output_file << L"\"Original Word\",\"Oleander\",\"Snowball\"" << std::endl;

    std::wofstream dif_file(dif_file_name.c_str(), std::ios::binary);
    if (!dif_file.is_open()) {
        std::wcerr << L"Error opening difference file!" << std::endl;
        return 1;
    }
    dif_file.imbue(utf16_locale);
    // Write the header with quotes
    dif_file << L"\"Original Word\",\"Oleander\",\"Snowball\"" << std::endl;

    // Oleander Stemmers
    stemming::english_stem<> ole_eng_stem;
    stemming::russian_stem<> ole_rus_stem;
    stemming::french_stem<> ole_fre_stem;

    while(std::getline(input_file, word_utf8)){
        // Skip empty lines
        if (word_utf8.empty()) continue;
        
        // Convert UTF-8 input to wstring and trim whitespace
        std::wstring word = utf8_to_wstr(word_utf8);
        
        // Remove any trailing whitespace/newlines
        while (!word.empty() && (word.back() == L'\r' || word.back() == L'\n' || word.back() == L' ' || word.back() == L'\t')) {
            word.pop_back();
        }
        // Make a copy for Oleander stemming (stems in place)
        std::wstring ole_word = word;

        // Apply Oleander stemming
        switch(lang){
            case English:
                ole_eng_stem(ole_word);
                break;
            case French:
                ole_fre_stem(ole_word);
                break;
            case Russian:
                ole_rus_stem(ole_word);
                break;
        }

        // Convert word to UTF-8 for Snowball stemmer  
        std::string utf8_word = wstr_to_utf8(word);
        
        // Get the stemmed result from Snowball
        const sb_symbol* snow_stem = sb_stemmer_stem(snow_stemmer, 
            (const sb_symbol*)utf8_word.data(), utf8_word.length());
        
        // Convert Snowball result back to wstring
        std::string snow_utf8_str(reinterpret_cast<const char*>(snow_stem));
        std::wstring snow_wstr = utf8_to_wstr(snow_utf8_str);

        // Write to the output file with proper CSV formatting and explicit flush
        output_file << L"\"" << word << L"\",\"" << ole_word << L"\",\"" << snow_wstr << L"\"" << std::endl;
        output_file.flush();

        // Check for differences and write to difference file
        if (ole_word != snow_wstr) {
            dif_file << L"\"" << word << L"\",\"" << ole_word << L"\",\"" << snow_wstr << L"\"" << std::endl;
            dif_file.flush();
        }

        // Update word count statistics
        snow_wc += snow_wstr.length();
        ole_wc += ole_word.length();
        ++line_count;
    }

    std::wcout << L"Snowball Average Word Len: " << (float)snow_wc / line_count << L"\n";
    std::wcout << L"Ole Average Word Len: " << (float)ole_wc / line_count << L"\n";

    // Clean up
    sb_stemmer_delete(snow_stemmer);
    input_file.close();
    output_file.close();
    dif_file.close();
    return 0;
}
