#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <codecvt>
#include <locale>
#include <iomanip>
#include "libstemmer.h"
#include "../lib/ole/english_stem.h"
#include "../lib/ole/french_stem.h"
#include "../lib/ole/russian_stem.h"

enum Language {
    English,
    French,
    Russian
};

struct LanguageConfig {
    Language lang;
    std::string name;
    std::string snowball_name;
    std::string data_file;
};

// Function prototypes
std::wstring utf8_to_wstr(const std::string& str);
std::string wstr_to_utf8(const std::wstring& wstr);
void test_snowball(const std::vector<std::wstring>& words, const std::string& lang_name);
void test_oleander(std::vector<std::wstring> words, Language lang);
std::vector<std::wstring> load_words(const std::string& file_name);
void profile_language(const LanguageConfig& config);
void run_all_tests();

int main() {
    // Set up locale for wide character output
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    
    run_all_tests();
    return 0;
}

// Convert UTF-8 string to wide string
std::wstring utf8_to_wstr(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Convert wide string to UTF-8 string
std::string wstr_to_utf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// Test Snowball Stemmer
void test_snowball(const std::vector<std::wstring>& words, const std::string& lang_name) {
    struct sb_stemmer* stemmer = sb_stemmer_new(lang_name.c_str(), 0);
    if (stemmer == nullptr) {
        std::cout << "Failed to create Snowball stemmer for " << lang_name << std::endl;
        return;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& word : words) {
        // Convert wstring to UTF-8 for Snowball
        std::string utf8_word = wstr_to_utf8(word);
        sb_stemmer_stem(stemmer, (const sb_symbol*)utf8_word.c_str(), utf8_word.length());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time = static_cast<double>(duration.count()) / words.size();
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Snowball (" << lang_name << "): " << avg_time << " μs/word, "
              << duration.count() << " μs total" << std::endl;
    
    sb_stemmer_delete(stemmer);
}

// Test Oleander Stemmer
void test_oleander(std::vector<std::wstring> words, Language lang) {
    std::string lang_name;
    auto start = std::chrono::high_resolution_clock::now();
    
    switch(lang) {
        case English: {
            lang_name = "English";
            stemming::english_stem<> eng_stemmer;
            for (auto& word : words) {
                eng_stemmer(word);
            }
            break;
        }
        case French: {
            lang_name = "French";
            stemming::french_stem<> fre_stemmer;
            for (auto& word : words) {
                fre_stemmer(word);
            }
            break;
        }
        case Russian: {
            lang_name = "Russian";
            stemming::russian_stem<> rus_stemmer;
            for (auto& word : words) {
                rus_stemmer(word);
            }
            break;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time = static_cast<double>(duration.count()) / words.size();
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Oleander (" << lang_name << "): " << avg_time << " μs/word, "
              << duration.count() << " μs total" << std::endl;
}

// Load words from file
std::vector<std::wstring> load_words(const std::string& file_name) {
    std::ifstream input_file(file_name);
    std::vector<std::wstring> words;
    std::string line;
    
    if (!input_file.is_open()) {
        std::cerr << "Error: Cannot open file " << file_name << std::endl;
        return words;
    }
    
    while (std::getline(input_file, line)) {
        // Skip empty lines
        if (line.empty()) continue;
        
        // Remove trailing whitespace/newlines
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || 
                                line.back() == ' ' || line.back() == '\t')) {
            line.pop_back();
        }
        
        if (!line.empty()) {
            words.push_back(utf8_to_wstr(line));
        }
    }
    
    input_file.close();
    return words;
}

// Profile a specific language
void profile_language(const LanguageConfig& config) {
    std::cout << "\n=== Testing " << config.name << " ===" << std::endl;
    
    std::vector<std::wstring> words = load_words(config.data_file);
    if (words.empty()) {
        std::cout << "No words loaded from " << config.data_file << std::endl;
        return;
    }
    
    std::cout << "Loaded " << words.size() << " words from " << config.data_file << std::endl;
    
    // Test Oleander first (it modifies the words in-place)
    std::vector<std::wstring> words_copy = words; // Make a copy for Oleander
    test_oleander(words_copy, config.lang);
    
    // Test Snowball (doesn't modify input)
    test_snowball(words, config.snowball_name);
}

// Run tests for all languages
void run_all_tests() {
    std::vector<LanguageConfig> languages = {
        {English, "English", "english", "../data/english.txt"},
        {French, "French", "french", "../data/french.txt"},
        {Russian, "Russian", "russian", "../data/russian_utf-8.txt"}
    };
    
    std::cout << "Multi-Language Stemmer Performance Comparison" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    for (const auto& lang_config : languages) {
        profile_language(lang_config);
    }
    
    std::cout << "\nTesting completed!" << std::endl;
}
