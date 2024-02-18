/*
 * Spell Checker Program
 * Lauren Hall
 * 11/12/2023
 * This program checks the spelling of words input by the user. It uses separate word lists for English and Spanish. It has 3 file inputs of English words for mispelled words. 2 that were found and 1 file input for spanish. It has to load the files then asks user to pick between 1. English and 2. Spanish
 * For misspelled words, it suggests the closest matches based on Hamming distance.
 
 
 all English and Spanish words found for my program can be attributed to: https://github.com/dwyl/english-words
 
 the list of profanities was found and can be attributed to: https://github.com/dsojevic/profanity-list/blob/main/en.txt
 
 
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>


#define MAX_WORD_LEN 40
#define MAX_WORDS 110000
#define MAX_HAMMING_DISTANCE 2 

char englishWordlist1[MAX_WORDS][MAX_WORD_LEN];
char englishWordlist2[MAX_WORDS][MAX_WORD_LEN];
char englishWordlist3[MAX_WORDS][MAX_WORD_LEN];
char otherWordlist[MAX_WORDS][MAX_WORD_LEN];
char inappropriateWords[MAX_WORDS][MAX_WORD_LEN];
int inappropriateWordCount;




void loadDictionary(const char *fileName, char wordList[MAX_WORDS][MAX_WORD_LEN], int *wordCount);

           
//	CALCULATE DISTANCE BETWEEN WORDS::  

int calculateHammingDistance(const char *word1, const char *word2) {
    int distance = 0;
    
    int len = strlen(word1);

    if (len != strlen(word2)) {
        return -1; 
    }

    for (int i = 0; i < len; i++) {
        if (word1[i] != word2[i]) {
            distance++;
        }
    }

    return distance;
}            


void loadInappropriateWords(const char *fileName, char inappropriateWords[MAX_WORDS][MAX_WORD_LEN], int *inappropriateWordCount) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Failed to open inappropriate words file %s.\n", fileName);
        return;
    }

    *inappropriateWordCount = 0;
    char word[MAX_WORD_LEN];
    while (*inappropriateWordCount < MAX_WORDS && fscanf(file, "%39s", word) == 1) {
        strcpy(inappropriateWords[*inappropriateWordCount], word);
        (*inappropriateWordCount)++;
    }

    fclose(file);
    printf("Loaded %d inappropriate words from %s.\n", *inappropriateWordCount, fileName);
}


// 	Filters  bad words

void filterOutInappropriateWords(char wordList[MAX_WORDS][MAX_WORD_LEN], int *wordCount, char inappropriateWords[MAX_WORDS][MAX_WORD_LEN], int inappropriateWordCount) {
    for (int i = 0; i < *wordCount; i++) {
        bool isAppropriate = true;
        for (int j = 0; j < inappropriateWordCount; j++) {
            if (strcmp(wordList[i], inappropriateWords[j]) == 0) {
                isAppropriate = false;
                break;
            }
        }
        if (!isAppropriate) {
            // Shift the remaining words to the left
            for (int k = i; k < *wordCount - 1; k++) {
                strcpy(wordList[k], wordList[k + 1]);
            }
            (*wordCount)--;
            i--; // Adjust index as the list size has changed
        }
    }
}


// 	Calculates Letter Diff (Thank YOU WIKI!)

int calculateModifiedHammingDistance(const char *word1, const char *word2) {
    int len1 = strlen(word1);
    int len2 = strlen(word2);

    // If lengths are equal, use standard Hamming distance
    if (len1 == len2) {
        return calculateHammingDistance(word1, word2);
    }

    // If length difference is more than 1, return -1
    if (abs(len1 - len2) > 1) {
        return -1;
    }

    // Calculate distance for words with length differing by 1
    int i = 0, j = 0, distance = 0;
    while (i < len1 && j < len2) {
        if (word1[i] != word2[j]) {
            distance++;
            // Skip the extra character in the longer word
            if (len1 > len2) {
                i++;
            } else {
                j++;
            }
        } else {
            i++;
            j++;
        }
    }
    // Account for the extra character at the end if any
    return distance + abs(len1 - len2);
}

          
            
// 	APPARRENTLY we don't need this after research.. but I left it in anyway

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

//	Needed this for my multiple word lists

void removeDuplicates(char wordList[MAX_WORDS][MAX_WORD_LEN], int *wordCount) {
    for (int i = 0; i < *wordCount; i++) {
        for (int j = i + 1; j < *wordCount; j++) {
            if (strcmp(wordList[i], wordList[j]) == 0) {
                // Shift to the left to the left
                for (int k = j; k < *wordCount - 1; k++) {
                    strcpy(wordList[k], wordList[k + 1]);
                }
                (*wordCount)--;
                j--; // Since we shifted, check the new j-th element again
            }
        }
    }
}


//	LOAD THE DICTIONARY::

void loadDictionary(const char *fileName, char wordList[MAX_WORDS][MAX_WORD_LEN], int *wordCount) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Failed to open dictionary file %s.\n", fileName);
        return; // Exit if the file cannot be opened
    }

    *wordCount = 0;
    char word[MAX_WORD_LEN];
    while (*wordCount < MAX_WORDS && fscanf(file, "%39s", word) == 1) {
        strcpy(wordList[*wordCount], word);
        (*wordCount)++;
    }

    fclose(file);
    printf("Loaded %d words from %s.\n", *wordCount, fileName);
}



//	APPLY LANGUAGE RULES::

void adjustSpanishWord(char *word) {
    int length = strlen(word);
    for (int i = 0; i < length; i++) {
        if ((unsigned char)word[i] == 0xC3) {
            // Next byte determines the actual character
            if (i + 1 < length) {
                switch ((unsigned char)word[i + 1]) {
                    case 0xA1: // á
                        word[i] = 'a';
                        break;
                    case 0xA9: // é
                        word[i] = 'e';
                        break;
                    case 0xAD: // í
                        word[i] = 'i';
                        break;
                    case 0xB3: // ó
                        word[i] = 'o';
                        break;
                    case 0xBA: // ú
                        word[i] = 'u';
                        break;
                    case 0xB1: // ñ
                        word[i] = 'n';
                        break;
                    // Add more cases if needed
                    default:
                        continue;
                }
                // Shift the rest of the string one position to the left
                memmove(&word[i + 1], &word[i + 2], strlen(word) - i - 1);
                length--; // Update the length of the string
            }
        }
    }
}



 


void applyLanguageSpecificRules(char wordList[MAX_WORDS][MAX_WORD_LEN], int wordCount, const char *language) {
    if (strcmp(language, "Spanish") == 0) {
        for (int i = 0; i < wordCount; i++) {
            adjustSpanishWord(wordList[i]);
        }
    }
  
}



//	Checks if it is on the words lists and suggests a word within 5 words

void checkAndSuggestWord(const char *word, char wordList[MAX_WORDS][MAX_WORD_LEN], int wordCount) {
    bool found = false;
    int minDistance = MAX_WORD_LEN;  
    int distance;

    
    for (int i = 0; i < wordCount; i++) {
        if (strcmp(wordList[i], word) == 0) {
            found = true;
            break;
        }
    }

    // alts
    if (!found) {
        printf("The word '%s' is not found. Did you mean:\n", word);

        // Find the minimum Hamming distance for words in the list
        for (int i = 0; i < wordCount; i++) {
            int distance = calculateModifiedHammingDistance(word, wordList[i]);
            if (distance >= 0 && distance < minDistance) {
                minDistance = distance;
            }
        }

        // Suggest other words
        int suggestionsCount = 0;
        for (int i = 0; i < wordCount && suggestionsCount < 5; i++) {
            int distance = calculateModifiedHammingDistance(word, wordList[i]);
            if (distance == minDistance) {
                printf("%s (Modified Hamming distance: %d)\n", wordList[i], distance);
                suggestionsCount++;
            }
        }
    } else {
        printf("The word '%s' is spelled correctly.\n", word);
    }
}
  

void checkAndSuggestWordSingleList(const char *word, char wordList[MAX_WORDS][MAX_WORD_LEN], int wordCount) {
    bool found = false;
    int minDistance = MAX_WORD_LEN;  
    int distance;

    // Check if the word exists in the word list
    for (int i = 0; i < wordCount; i++) {
        if (strcmp(wordList[i], word) == 0) {
            found = true;
            break;
        }
    }

    // If the word is not found, suggest alternatives
    if (!found) {
        printf("The word '%s' is not found. Did you mean:\n", word);

        // Find the minimum Hamming distance for words in the list
        for (int i = 0; i < wordCount; i++) {
            distance = calculateHammingDistance(word, wordList[i]);
            if (distance >= 0 && distance < minDistance) {
                minDistance = distance;
            }
        }

        // Suggest words with the minimum Hamming distance
        int suggestionsCount = 0;
        for (int i = 0; i < wordCount && suggestionsCount < 5; i++) {
            distance = calculateHammingDistance(word, wordList[i]);
            if (distance == minDistance) {
                printf("%s (distance: %d)\n", wordList[i], distance);
                suggestionsCount++;
            }
        }
    } else {
        printf("The word '%s' is spelled correctly.\n", word);
    }
}


// 	This is the function (same as above function) but for the multiple word lists)

void checkAndSuggestWordCombined(const char *word, 
                                 char wordList1[MAX_WORDS][MAX_WORD_LEN], int wordCount1, 
                                 char wordList2[MAX_WORDS][MAX_WORD_LEN], int wordCount2, 
                                 char wordList3[MAX_WORDS][MAX_WORD_LEN], int wordCount3) {
    bool found = false;
    int minDistance = MAX_WORD_LEN; 

    // Check to see if the words on the list
    bool checkWordList(const char *word, char wordList[MAX_WORDS][MAX_WORD_LEN], int wordCount) {
        for (int i = 0; i < wordCount; i++) {
            if (strcmp(wordList[i], word) == 0) {
                return true; // Word found
            }
        }
        return false; 
        // it's not there
    }

// lets see if it be on the 3 lists, shall we?
    if (checkWordList(word, wordList1, wordCount1) || 
        checkWordList(word, wordList2, wordCount2) || 
        checkWordList(word, wordList3, wordCount3)) {
        found = true;
    }

    // Suggest alternatives if the word wasn't found
    if (!found) {
        printf("The word '%s' is not found. Did you mean:\n", word);

        // check the distance of the word on the list
        void updateMinDistance(const char *word, char wordList[MAX_WORDS][MAX_WORD_LEN], int wordCount, int *minDistance) {
            for (int i = 0; i < wordCount; i++) {
                int distance = calculateHammingDistance(word, wordList[i]);
                if (distance >= 0 && distance < *minDistance) {
                    *minDistance = distance;
                }
            }
        }

        // update the distance of the words of the lists
        updateMinDistance(word, wordList1, wordCount1, &minDistance);
        updateMinDistance(word, wordList2, wordCount2, &minDistance);
        updateMinDistance(word, wordList3, wordCount3, &minDistance);

        // Suggestions, please.
        int suggestWords(const char *word, char wordList[MAX_WORDS][MAX_WORD_LEN], int wordCount, int minDistance) {
            int suggestionsCount = 0;
            for (int i = 0; i < wordCount; i++) {
                int distance = calculateHammingDistance(word, wordList[i]);
                if (distance == minDistance) {
                    printf("%s (Hamming distance: %d)\n", wordList[i], distance);
                    if (++suggestionsCount >= 5) break; // Stop after 5 suggestions
                }
            }
            return suggestionsCount;
        }

       
        int totalSuggestions = 0;
        totalSuggestions += suggestWords(word, wordList1, wordCount1, minDistance);
        if (totalSuggestions < 5) totalSuggestions += suggestWords(word, wordList2, wordCount2, minDistance);
        if (totalSuggestions < 5) suggestWords(word, wordList3, wordCount3, minDistance);
    } else {
        printf("The word '%s' is spelled correctly.\n", word);
    }
}



//	THE MAIN ::

int main() {
    int wordCount1, wordCount2, wordCount3, otherWordCount;

    // Load word lists
    loadDictionary("wordlist.txt", englishWordlist1, &wordCount1);
    loadDictionary("wordsEn.txt", englishWordlist2, &wordCount2);
    loadDictionary("en.txt", englishWordlist3, &wordCount3);
    loadDictionary("es.txt", otherWordlist, &otherWordCount);
    loadInappropriateWords("inappropriate_words.txt", inappropriateWords, &inappropriateWordCount);

    // Ask if the user wants to filter out inappropriate words
    printf("Do you want to filter out inappropriate words? (yes/no): ");
    char filterChoice[4];
    scanf("%3s", filterChoice);
    getchar(); // To consume the newline character

    // Convert to lowercase for comparison
    toLowerCase(filterChoice);

    if (strcmp(filterChoice, "yes") == 0) {
        filterOutInappropriateWords(englishWordlist1, &wordCount1, inappropriateWords, inappropriateWordCount);
        filterOutInappropriateWords(englishWordlist2, &wordCount2, inappropriateWords, inappropriateWordCount);
        filterOutInappropriateWords(englishWordlist3, &wordCount3, inappropriateWords, inappropriateWordCount);
        printf("Inappropriate words have been filtered out.\n");
    }

    // Proceed with the spelling check options
    printf("Select an option:\n");
    printf("1. Check English word (single list)\n");
    printf("2. Check English word (combine 3 word lists)\n");
    printf("3. Check Spanish word\n");
    int choice;
    scanf("%d", &choice);
    getchar();

    char word[MAX_WORD_LEN];
    printf("Enter a word to check (or type 0 to exit):\n");
    while (fgets(word, sizeof(word), stdin)) {
        toLowerCase(word);
        word[strlen(word) - 1] = '\0'; // Remove newline character

        if (strcmp(word, "0") == 0) {
            break;
        }

        if (strlen(word) >= MAX_WORD_LEN - 1) {
            printf("Input word is too long, try a shorter word.\n");
            continue;
        }

        switch (choice) {
            case 1:
                checkAndSuggestWord(word, englishWordlist1, wordCount1);
                break;
            case 2:
                checkAndSuggestWordCombined(word, englishWordlist1, wordCount1, 
                                            englishWordlist2, wordCount2, 
                                            englishWordlist3, wordCount3);
                break;
            case 3:
                checkAndSuggestWord(word, otherWordlist, otherWordCount);
                break;
            default:
                printf("Invalid option.\n");
                break;
        }

        printf("Enter another word (or type 0 to exit):\n");
    }

    printf("Goodbye!\n");
    return 0;
}


