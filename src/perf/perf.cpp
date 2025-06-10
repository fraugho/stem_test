#include <stdio.h>
#include <string.h>
#include <libstemmer.h>
#include <fstream>
#include <iostream>
#include <codecvt>
#include "../../lib/ole/english_stem.h"
#include <chrono>
#include <vector>

void test();

int main() {
    test();
    return 0;
}

std::wstring stringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Snowball Stemmer
void test_snow(std::vector<std::string>& words){
    struct sb_stemmer* snow_stemmer = sb_stemmer_new("english", 0);
    auto start = std::chrono::high_resolution_clock::now();
    for(auto word : words){
        sb_stemmer_stem(snow_stemmer, (const sb_symbol*)word.data(), word.length());
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout << "Snowball: " << duration.count() << "us\n";
    sb_stemmer_delete(snow_stemmer);
}

// Oleander Stemmer
void test_ole(std::vector<std::wstring>& words){
    stemming::english_stem<> eng_stem;
    auto start = std::chrono::high_resolution_clock::now();
    for(auto word : words){
        eng_stem(word);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout << "Oleander: " << duration.count() << "us\n";
}

std::vector<std::string>* get_str(std::string file_name){
    std::ifstream dict(file_name);
    std::vector<std::string>* words = new std::vector<std::string>();
    std::string word;
    while(std::getline(dict, word)){
        words->push_back(word);
    }
    return words;
}

std::vector<std::wstring>* get_wstr(std::string file_name){
    std::ifstream dict(file_name);
    std::vector<std::wstring>* words = new std::vector<std::wstring>();
    std::string word;
    std::wstring w_word;
    while(std::getline(dict, word)){    
        w_word = stringToWString(word);
        words->push_back(w_word);
    }
    return words;
}

void test() {
    test_ole(*get_wstr("../../data/words.txt"));
    test_snow(*get_str("../../words.txt"));
}
