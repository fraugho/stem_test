#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <codecvt>
#include <libstemmer.h>
#include "../../../lib/cpp/ole/src/english_stem.h"

// Function prototypes
std::wstring stringToWString(const std::string& str);
void test_snow(const std::vector<std::wstring>& words);
void test_ole(const std::vector<std::wstring>& words);
std::vector<std::wstring> get_wstr(const std::string& file_name);
void test();

int word_count = 0;

int main() {
    test();
    return 0;
}

// Convert UTF-8 string to wide string
std::wstring stringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Test Snowball Stemmer
void test_snow(const std::vector<std::wstring>& words) {
    struct sb_stemmer* snow_stemmer = sb_stemmer_new("english", 0);
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& word : words) {
        // Convert wstring to sb_symbol* for Snowball
        sb_stemmer_stem(snow_stemmer, (const sb_symbol*)word.c_str(), word.length());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Snowball: " << duration.count() / word_count << "us\n";
    
    sb_stemmer_delete(snow_stemmer);
}

// Test Oleander Stemmer
void test_ole(std::vector<std::wstring>& words) {
    stemming::english_stem<> eng_stem;
    auto start = std::chrono::high_resolution_clock::now();

    for (auto word : words) {
        eng_stem(word);
    }
    /*
    for(int i = 0; i < word_count; ++i){
        eng_stem(words[i]);
    }
     * */
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Oleander: " << duration.count() / word_count << "us\n";
}

// Read words from a file into a vector of wide strings
std::vector<std::wstring> get_wstr(const std::string& file_name) {
    std::ifstream dict(file_name);
    std::vector<std::wstring> words;
    std::string word;
    
    while (std::getline(dict, word)) {    
        words.push_back(stringToWString(word));
        ++word_count;
    }
    
    return words;
}

// Main test function
void test() {
    std::vector<std::wstring> words = get_wstr("words.txt");
    test_ole(words);
    test_snow(words);
}
