#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

FILE* alloc;
FILE* dealloc;

typedef struct WordNode_struct {
    char* myWord;
    struct WordNode_struct* next; 
} WordNode;

typedef struct LadderNode_struct {
    WordNode* topWord;
    struct LadderNode_struct* next; 
} LadderNode;


int countWordsOfLength(char* filename, int wordSize) { 
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1; // Unable to open the file
    }

    int count = 0;
    char word[100]; // Assuming a maximum word length of 100
    while (fscanf(file, "%s", word) != EOF) {
        if (strlen(word) == wordSize) {
            count++;
        }
    }

    fclose(file);
    return count;
}

bool buildWordArray(char* filename, char** words, int numWords, int wordSize) { 
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false; // Unable to open the file
    }

    int count = 0;
    char word[100]; // Assuming a maximum word length of 100
    int wordIndex = 0;
    while (fscanf(file, "%s", word) != EOF) {
        if (strlen(word) == wordSize) {
            // Allocate space for the word in the words array
            fprintf(alloc, "buildWord_words: %x\n", words[wordIndex]);
            //words[wordIndex] = (char*)malloc((wordSize + 1) * sizeof(char));
            fprintf(alloc, "buildWord_words2: %x\n", words[wordIndex]);
            strcpy(words[wordIndex], word);
            wordIndex++;
        }
    }

    fclose(file);

    return (wordIndex == numWords);
}

int findWord(char** words, char* aWord, int loInd, int hiInd) { 
    while (loInd <= hiInd) {
        int mid = loInd + (hiInd - loInd) / 2;
        int cmp = strcmp(aWord, words[mid]);
        if (cmp == 0) {
            return mid; // Word found at index mid
        }
        if (cmp < 0) {
            hiInd = mid - 1;
        } else {
            loInd = mid + 1;
        }
    }
    return -1; // Word not found
}

void freeWords(char** words, int numWords) {
    for (int i = 0; i < numWords; i++) {
        fprintf(dealloc, "freeWords: %d:_%x\n", i, words[i]);
        free(words[i]);
    }
    fprintf(dealloc, "freeWords: %x\n", words);
    free(words);

}

void insertWordAtFront(WordNode** ladder, char* newWord) {
     // Create a new WordNode
    WordNode* newNode = (WordNode*)malloc(sizeof(WordNode));
    fprintf(alloc, "insertWordFront_newNode: %x\n", newNode);
    if (newNode == NULL) {
       exit(1); // Memory allocation failed
    }

    // Copy both memory address and content
    newNode->myWord = newWord;


    newNode->next = *ladder;

    // Update the ladder pointer to point to the new node
    *ladder = newNode;
}

int getLadderHeight(WordNode* ladder) {
    int height = 0;
    WordNode* currentNode = ladder;
    while (currentNode != NULL) {
        height++;
        currentNode = currentNode->next;
    }
    return height;
}

WordNode* copyLadder(WordNode* ladder) {
    WordNode* newLadder = NULL;
    WordNode* lastNode = NULL;
    WordNode* currentNode = ladder;

    while (currentNode != NULL) {
        WordNode* newNode = (WordNode*)malloc(sizeof(WordNode));
        fprintf(alloc, "copyLadder_copyNew: %x\n", newNode);

        newNode->myWord = currentNode->myWord;
        newNode->next = NULL;

        if (lastNode == NULL) {
            newLadder = newNode;
        } else {
            lastNode->next = newNode;
        }

        lastNode = newNode;
        fprintf(alloc, "copyLadder_currentNode: %x\n", currentNode);
        currentNode = currentNode->next;
    }


    return newLadder;

}

void freeLadder(WordNode* ladder) {
    while (ladder != NULL) {
        WordNode* temp = ladder;
        ladder = ladder->next;
        fprintf(dealloc, "freeLadder: %x\n", ladder);
        free(temp);

    }
}

void insertLadderAtBack(LadderNode** list, WordNode* newLadder) {
    LadderNode* newNode = (LadderNode*)malloc(sizeof(LadderNode));
    fprintf(alloc, "insertLadderBack_ladderNew: %x\n", newNode);


    LadderNode* current = *list;


    newNode->topWord = newLadder;

    newNode->next = NULL;

    if (*list == NULL) {
        *list = newNode;
    } else {
        //LadderNode* current = (LadderNode*)malloc(sizeof(LadderNode));
        LadderNode* current = *list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }


}

WordNode* popLadderFromFront(LadderNode** list) {
    if (*list == NULL) {
        return NULL;
    }

    LadderNode* poppedNode = *list;
    *list = poppedNode->next;
    WordNode* ladder = poppedNode->topWord;
    free(poppedNode);

    return ladder;
}

void freeLadderList(LadderNode* myList) {
    while (myList != NULL) {
        LadderNode* temp = myList;
        myList = myList->next;

        freeLadder(temp->topWord);

        free(temp);

    }
}

WordNode* findShortestWordLadder(   char** words, 
                                    bool* usedWord, 
                                    int numWords, 
                                    int wordSize, 
                                    char* startWord, 
                                    char* finalWord ) {
    // Create an empty list of partially complete ladders
    LadderNode* myList = NULL;

    // Create a ladder with just the start word and append it to myList
    WordNode* myLadder = NULL;
    insertWordAtFront(&myLadder, startWord);
    insertLadderAtBack(&myList, myLadder);

    for (int i = 0; i < numWords; ++i) {
        usedWord[i] = false;
    }

    // Continue until myList is not empty
    while (myList != NULL) {
        // Pop the head ladder off the front of myList
        myLadder = popLadderFromFront(&myList);

        // Get the last word in the current ladder
        char* currentWord = myLadder->myWord;

        // Check if the current ladder ends with the final word
        if (strcmp(currentWord, finalWord) == 0) {
            // Found the shortest ladder, so free unused ladders
            while (myList != NULL) {
                WordNode* remainingLadder = popLadderFromFront(&myList);
                freeLadder(remainingLadder);
            }
            // We found the shortest word ladder, return it
            return myLadder;
        }

        // Find neighbors of the current word (words that differ by one letter)
        for (int i = 0; i < numWords; i++) {
            if (!usedWord[i]) {
                char* neighborWord = words[i];

                int diffCount = 0;  // Count of differing letters
                int len = strlen(neighborWord);

                // Compare the letters of the current word and the neighbor word
                for (int j = 0; j < len; j++) {
                    if (currentWord[j] != neighborWord[j]) {
                        diffCount++;
                        if (diffCount > 1) {
                            break;  // More than one differing letter, not a neighbor
                        }
                    }
                }

                if (diffCount == 1) {
                    // Create a copy of the current ladder and add the neighbor word
                    WordNode* newLadder = copyLadder(myLadder);
                    insertWordAtFront(&newLadder, neighborWord);

                    // Append the new ladder to myList
                    insertLadderAtBack(&myList, newLadder);

                    // Mark the neighbor word as used
                    usedWord[i] = true;
                }
            }
        }
        freeLadder(myLadder);
    }

   while (myList != NULL) {
        WordNode* remainingLadder = popLadderFromFront(&myList);
        freeLadder(remainingLadder);
    }
    // No ladder was found, return NULL
    return NULL;
}

// interactive user-input to set a word; 
//  ensures sure the word is in the dictionary word array
void setWord(char** words, int numWords, int wordSize, char* aWord) {
    bool valid = false;
    printf("  Enter a %d-letter word: ", wordSize);
    int count = 0;
    while (!valid) {
        scanf("%s",aWord);
        count++;
        valid = (strlen(aWord) == wordSize);
        if (valid) {
            int wordInd = findWord(words, aWord, 0, numWords-1);
            if (wordInd < 0) {
                valid = false;
                printf("    Entered word %s is not in the dictionary.\n",aWord);
                printf("  Enter a %d-letter word: ", wordSize); 
            }
        } else {
            printf("    Entered word %s is not a valid %d-letter word.\n",aWord,wordSize);
            printf("  Enter a %d-letter word: ", wordSize);
        }
        if (!valid && count >= 5) { //too many tries, picking random word
            printf("\n");
            printf("  Picking a random word for you...\n");
            strcpy(aWord,words[rand()%numWords]);
            printf("  Your word is: %s\n",aWord);
            valid = true;
        }
    }
}

// helpful debugging function to print a single Ladder
void printLadder(WordNode* ladder) {
    WordNode* currNode = ladder;
    while (currNode != NULL) {
        printf("\t\t\t%s\n",currNode->myWord);
        currNode = currNode->next;
    }
}

// helpful debugging function to print the entire list of Ladders
void printList(LadderNode* list) {
    printf("\n");
    printf("Printing the full list of ladders:\n");
    LadderNode* currList = list;
    while (currList != NULL) {
        printf("  Printing a ladder:\n");
        printLadder(currList->topWord);
        currList = currList->next;
    }
    printf("\n");
}



int main() {

    alloc = fopen("alloc.txt", "w");
    dealloc = fopen("dealloc.txt", "w");
    srand((int)time(0));

    printf("\nWelcome to the Word Ladder Generator!\n\n");

    // set word length using interactive user-input
    int wordSize=4;
    printf("Enter the word size for the ladder: ");
    //scanf("%d",&wordSize);
    printf("\n");

    printf("This program will make the shortest possible\n"); 
    printf("word ladder between two %d-letter words.\n\n",wordSize);

    // interactive user-input sets the dictionary file;
    //  check that file exists; if not, user enters another file
    //  if file exists, count #words of desired length [wordSize]
    char dict[100]="simple4.txt";
    printf("Enter filename for dictionary: ");
   // scanf("%s", dict);
    printf("\n");
    int numWords = countWordsOfLength(dict,wordSize);
    while (numWords < 0) {
        printf("  Dictionary %s not found...\n",dict);
        printf("Enter filename for dictionary: ");
        scanf("%s", dict);
        printf("\n");
        numWords = countWordsOfLength(dict,wordSize);
    }

    // allocate heap memory for the word array; only words with desired length
    char** words = (char**)malloc(numWords*sizeof(char*));
    fprintf(alloc, "main_alloc: %x\n", words);

    for (int i = 0; i < numWords; ++i) {

        words[i] = (char*)malloc((wordSize+1)*sizeof(char));
        fprintf(alloc, "main_alloc: %x\n", words[i]);

    }

    // end program if file does not have at least two words of desired length
    if (numWords < 2) {
        printf("  Dictionary %s contains insufficient %d-letter words...\n",dict,wordSize);
        printf("Terminating program...\n");
        return -1;
    }

    // [usedWord] bool array has same size as word array [words];
    //  all elements initialized to [false];
    //  later, usedWord[i] will be set to [true] whenever 
    //      words[i] is added to ANY partial word ladder;
    //      before adding words[i] to another word ladder,
    //      check for previous usage with usedWord[i] 
    bool* usedWord = (bool*)malloc(numWords*sizeof(bool));

    for (int i = 0; i < numWords; ++i) {
        usedWord[i] = false;
    }

    // build word array (only words with desired length) from dictionary file
    printf("Building array of %d-letter words... ", wordSize); 
    bool status = buildWordArray(dict,words,numWords,wordSize);

    if (!status) {
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of %d-letter words.\n",wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    printf("Done!\n"); 

    // set the two ends of the word ladder using interactive user-input
    //  make sure start and final words are in the word array, 
    //  have the correct length (implicit by checking word array), AND
    //  that the two words are not the same
    char startWord[30];
    char finalWord[30];
    printf("Setting the start %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, startWord);
    printf("\n");
    printf("Setting the final %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, finalWord);
    while (strcmp(finalWord,startWord) == 0) {
        printf("  The final word cannot be the same as the start word (%s).\n",startWord);
        printf("Setting the final %d-letter word... \n", wordSize);
        setWord(words, numWords, wordSize, finalWord);
    }
    printf("\n");

    // run the algorithm to find the shortest word ladder
    WordNode* myLadder = findShortestWordLadder(words, usedWord, numWords, wordSize, startWord, finalWord);

    // display word ladder and its height if one was found
    if (myLadder == NULL) {
        printf("There is no possible word ladder from %s to %s\n",startWord,finalWord);    
    } else {
        printf("Shortest Word Ladder found!\n");
        printLadder(myLadder);
    }
    printf("Word Ladder height = %d\n",getLadderHeight(myLadder));




    // free the heap-allocated memory for the shortest ladder
    freeLadder(myLadder);

    // free the heap-allocated memory for the words array
    //printf("num: %x", words);

    freeWords(words,numWords);

    free(usedWord);

    fclose(alloc);
    fclose(dealloc);

    return 0;
}
