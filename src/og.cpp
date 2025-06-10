#include <stdio.h>
#include <string.h>  // Include this for strlen
#include <libstemmer.h>
#include "../../../lib/cpp/ole/src/english_stem.h"

int main() {
    const char *word = "running";
    struct sb_stemmer *stemmer = sb_stemmer_new("english", 0);
    
    if (stemmer == NULL) {
        fprintf(stderr, "Failed to create stemmer\n");
        return 1;
    }

    // Stem the word
    const sb_symbol *stemmed = sb_stemmer_stem(stemmer, (const sb_symbol *)word, strlen(word));
    int stemmed_length = sb_stemmer_length(stemmer);

    // Print the stemmed word
    printf("Stemmed word: ");
    for (int i = 0; i < stemmed_length; i++) {
        putchar(stemmed[i]);
    }
    putchar('\n');

    sb_stemmer_delete(stemmer);
    return 0;
}

void ole_stem(){
    // the word to be stemmed
    std::wstring word(L"documentation");

    stemming::english_stem<> StemEnglish;
    std::wcout << L"(English) Original text: " << word.c_str() << std::endl;

    StemEnglish(word);

    std::wcout << L"(English) Stemmed text: " << word.c_str() << std::endl;
}
