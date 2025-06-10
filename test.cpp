
#include <stdio.h>
#include <string.h>
#include <libstemmer.h>
#include <fstream>
#include <iostream>
#include <codecvt>
#include "../../../lib/cpp/ole/src/english_stem.h"

void snow_stem();
void ole_stem();
void french_stem(const std::string& language);
int test();

int main() {
    /*
    ole_stem();
    snow_stem();
    french_stem("french");
     * */
    test();
    return 0;
}
std::wstring stringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

int test() {
    std::string word;

    // Open a CSV file for writing
    std::ofstream outputFile("stemming_results.csv");
    
    // Check if the file is open
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // Write the header
    outputFile << "Original Word,Oleander,Snowball\n";

    // Ole Stemmer
    stemming::english_stem<> ole_stem_eng;

    // Snowball
    struct sb_stemmer* snow_stemmer = sb_stemmer_new("english", 0);
    if (snow_stemmer == NULL) {
        std::cerr << "Failed to create snowball stemmer" << std::endl;
        return 1;
    }

    std::ifstream dict("words.txt");
    while(std::getline(dict, word)){
        // Convert std::string to std::wstring for Oleander stemmer
        std::wstring wideWord = stringToWString(word);

        // Get the stemmed results
        ole_stem_eng(wideWord); // This modifies wideWord in place
        std::wstring oleStemmed = wideWord; // Store the result

        // Get the stemmed result from Snowball
        const sb_symbol* snowStemmed = sb_stemmer_stem(snow_stemmer, (const sb_symbol*)word.data(), word.length());

        // Write to the output file
        outputFile << word << "," << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(oleStemmed) << "," << snowStemmed << "\n";
    }
    /*
    for (int i = 0; i < 50 && std::getline(dict, word); ++i) {
        // Convert std::string to std::wstring for Oleander stemmer
        std::wstring wideWord = stringToWString(word);

        // Get the stemmed results
        ole_stem_eng(wideWord); // This modifies wideWord in place
        std::wstring oleStemmed = wideWord; // Store the result

        // Get the stemmed result from Snowball
        const sb_symbol* snowStemmed = sb_stemmer_stem(snow_stemmer, (const sb_symbol*)word.data(), word.length());

        // Write to the output file
        outputFile << word << "," << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(oleStemmed) << "," << snowStemmed << "\n";
    }
     * */

    // Clean up
    sb_stemmer_delete(snow_stemmer);
    outputFile.close();

    return 0;
}

void french_stem(const std::string& language) {
    // Example words in French or German
    std::string word = "mangé"; // French for "eaten"
    // std::string word = "gegangen"; // German for "gone"
    
    struct sb_stemmer *stemmer = sb_stemmer_new(language.c_str(), 0);
    
    if (stemmer == NULL) {
        std::cerr << "Failed to create stemmer for language: " << language << "\n";
        return;
    }

    // Stem the word
    const sb_symbol *stemmed = sb_stemmer_stem(stemmer, (const sb_symbol *)word.data(), word.length());
    int stemmed_length = sb_stemmer_length(stemmer);

    // Print stemmed word
    std::cout << std::string(reinterpret_cast<const char*>(stemmed), stemmed_length) << "\n";

    sb_stemmer_delete(stemmer);
}

void snow_stem(){
    std::string word = "running";
    struct sb_stemmer *stemmer = sb_stemmer_new("english", 0);
    
    if (stemmer == NULL) {
        fprintf(stderr, "Failed to create stemmer\n");
        return;
    }

    // Stem the word
    const sb_symbol *stemmed = sb_stemmer_stem(stemmer, (const sb_symbol *)word.data(), word.length());

    //printing stemmed word
    std::cout << stemmed << "\n";

    word = "kicked";
    stemmed = sb_stemmer_stem(stemmer, (const sb_symbol *)word.data(), word.length());
    std::cout << stemmed << "\n";


    std::ifstream dict("words.txt");
    /*
    for(int i = 0; i < 50; ++i){
        std::getline(dict, word);
        std::cout << word << "\n";
    }
     * */

    sb_stemmer_delete(stemmer);
}

void ole_stem(){
    // the word to be stemmed
    std::wstring word(L"documentation");

    stemming::english_stem<> StemEnglish;
    std::wcout << L"(English) Original text: " << word.c_str() << std::endl;

    StemEnglish(word);

    std::wcout << L"(English) Stemmed text: " << word.c_str() << std::endl;
}
