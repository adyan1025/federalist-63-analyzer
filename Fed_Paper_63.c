#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_PAPER_LENGTH 2000
#define MAX_WORD_LENGTH 30
#define PAPERS_PER_AUTHOR 3

typedef struct Paper {
    char writer[15];
    char file_name[PAPERS_PER_AUTHOR][20];
    char dictionary[MAX_PAPER_LENGTH * PAPERS_PER_AUTHOR][MAX_WORD_LENGTH];
    double word_frequency[MAX_PAPER_LENGTH * PAPERS_PER_AUTHOR];
    int similar_words, text_total, points;
    int individual_text_count[PAPERS_PER_AUTHOR];
} paper_t;

int isValidWord(char word[]);
void capitalLetter(char word[]);
void wordFreq(paper_t *author);
void compareOriginalWords(char current_word[], paper_t *author, int *new_word);
int readOriginalPaper(FILE *pF, paper_t *author, int *new_word);
void compareWords(char current_word[], paper_t *author, int *new_word, paper_t *original);
int readPaper(FILE *pF, paper_t *author, int *new_word, paper_t *original);
void alphaSort(paper_t *author, int total_words);

int main() {
    paper_t ham, jay, mad, anon;
    int in_fed63_words = 0;
    int word_count = 0;

    strcpy(anon.file_name[0], "Fed_63.txt");
    strcpy(ham.file_name[0], "Ham_Fed_1.txt");
    strcpy(ham.file_name[1], "Ham_Fed_6.txt");
    strcpy(ham.file_name[2], "Ham_Fed_7.txt");
    strcpy(jay.file_name[0], "Jay_Fed_2.txt");
    strcpy(jay.file_name[1], "Jay_Fed_3.txt");
    strcpy(jay.file_name[2], "Jay_Fed_64.txt");
    strcpy(mad.file_name[0], "Mad_Fed_10.txt");
    strcpy(mad.file_name[1], "Mad_Fed_37.txt");
    strcpy(mad.file_name[2], "Mad_Fed_40.txt");

    strcpy(anon.writer, "Anonymous");
    strcpy(ham.writer, "Hamilton");
    strcpy(jay.writer, "Jay");
    strcpy(mad.writer, "Madison");


    paper_t auth_list[] = {ham, jay, mad};

    //Anon
    FILE *pF = fopen(anon.file_name[0], "r");
    if (pF == NULL) {
        printf("Error opening file %s.\n", anon.file_name[0]);
        return 1;
    }
    int og_total = readOriginalPaper(pF, &anon, &in_fed63_words);
    anon.similar_words = in_fed63_words;
    anon.text_total = og_total;
    alphaSort(&anon, anon.similar_words);
    wordFreq(&anon);
    fclose(pF);

    //All authors
    for (int i = 0; i < PAPERS_PER_AUTHOR; i++) {
        in_fed63_words = 0;
        auth_list[i].text_total = 0;

        for (int j = 0; j < PAPERS_PER_AUTHOR; j++) {
            FILE *pF = fopen(auth_list[i].file_name[j], "r");
            if (pF == NULL) {
                printf("Error opening file %s.\n", anon.file_name[0]);
                return 1;
            }
            word_count = readPaper(pF, &auth_list[i], &in_fed63_words, &anon);
            fclose(pF);
            auth_list[i].text_total += word_count;
            auth_list[i].individual_text_count[j] = word_count;
            auth_list[i].similar_words = in_fed63_words;
        }
        
        alphaSort(&auth_list[i], auth_list[i].similar_words);
        wordFreq(&auth_list[i]);
    }


    
    for (int i = 0; i < PAPERS_PER_AUTHOR; i++) {
        auth_list[i].points = 0;
    }
    for (int j = 0; j < anon.text_total; j++) {
        double ham_sim = 0.0, jay_sim = 0.0, mad_sim = 0.0;
        for (int k = 0; k < auth_list[0].similar_words; k++) {
            if (strcmp(anon.dictionary[j], auth_list[0].dictionary[k]) == 0) {
                ham_sim += fabs(auth_list[0].word_frequency[k] - anon.word_frequency[j]);
                break;
            }
        }
        for (int k = 0; k < auth_list[1].similar_words; k++) {
            if (strcmp(anon.dictionary[j], auth_list[1].dictionary[k]) == 0) {
                jay_sim += fabs(auth_list[1].word_frequency[k] - anon.word_frequency[j]);
                break;
            }
        }
        for (int k = 0; k < auth_list[2].similar_words; k++) {
            if (strcmp(anon.dictionary[j], auth_list[2].dictionary[k]) == 0) {
                mad_sim += fabs(auth_list[2].word_frequency[k] - anon.word_frequency[j]);
                break;
            }
        }

        if ((ham_sim > jay_sim) && (ham_sim > mad_sim)) {
            auth_list[0].points++;
        }
        else if ((jay_sim > ham_sim) && (jay_sim > mad_sim)) {
            auth_list[1].points++;
        }
        else if ((mad_sim > ham_sim) && (mad_sim > jay_sim)) {
            auth_list[2].points++;
        }
    }

    if ((auth_list[0].points > auth_list[1].points) && (auth_list[0].points > auth_list[2].points)) {
        printf("\n%s most likely wrote Federalist 63.\n\n", auth_list[0].writer);
    }
    else if ((auth_list[1].points > auth_list[0].points) && (auth_list[1].points > auth_list[2].points)) {
        printf("\n%s most likely wrote Federalist 63.\n\n", auth_list[1].writer);
    }
    else if ((auth_list[2].points > auth_list[0].points) && (auth_list[2].points > auth_list[1].points)) {
        printf("\n%s most likely wrote Federalist 63.\n\n", auth_list[2].writer);
    }
    
    return 0;
}

void alphaSort(paper_t *author, int total_words) {
    int placeholder;
    char temp[total_words];

    for (int i = 0; i < total_words; i++) {
        for (int j = i + 1; j < total_words; j++) {
            if (strcmp(author->dictionary[i], author->dictionary[j]) > 0) {
                placeholder = author->word_frequency[i];
                strcpy(temp, author->dictionary[i]);

                author->word_frequency[i] = author->word_frequency[j];
                strcpy(author->dictionary[i], author->dictionary[j]);

                author->word_frequency[j] = placeholder;
                strcpy(author->dictionary[j], temp);
            }
        }
    }
}

int isValidWord(char word[]) {
    int counter = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        if (!isalpha(word[i])) {
            return 0;
        }
        counter++;
    }
    if (counter >= 5) {
        return 1;
    }
    else {
        return 0;
    }
}

void capitalLetter(char word[]) {
    for (int i = 0; word[i] != '\0'; i++) {
        if (isalpha(word[i]) != 0) {
            if (isupper(word[i]) != 0) {
                word[i] = tolower(word[i]);
            } 
        }
    }
}

void wordFreq(paper_t *author) {

    for (int i = 0; i < author->similar_words; i++) {
        author->word_frequency[i] = (author->word_frequency[i]) / (author->text_total);

    }

}

void compareOriginalWords(char current_word[], paper_t *author, int *new_word) {
    for (int i = 0; i < MAX_PAPER_LENGTH * PAPERS_PER_AUTHOR; i++) {
            if (strcmp(current_word, author->dictionary[i]) == 0) {
                author->word_frequency[i]++;
                return;
            }
        }
    strcpy(author->dictionary[*new_word], current_word);
    author->word_frequency[*new_word] = 1;
    (*new_word)++;
}

int readOriginalPaper(FILE *pF, paper_t *author, int *new_word) {
    char current_word[MAX_WORD_LENGTH];
    int counter = 0;

    while (fscanf(pF, "%s", current_word) == 1) {
        counter++;
        if (isValidWord(current_word) == 1) {
            capitalLetter(current_word);
            compareOriginalWords(current_word, author, new_word);
        }      
    }
    return counter;
}

void compareWords(char current_word[], paper_t *author, int *new_word, paper_t *original) {
    for (int i = 0; i < original->similar_words; i++) {
            if (strcmp(original->dictionary[i],current_word) == 0) {
                for (int j = 0; j < *new_word; j++) {
                    if (strcmp(current_word, author->dictionary[j]) == 0) {
                        author->word_frequency[j]++;
                        return;
                    }
                }
            }
        }
    strcpy(author->dictionary[*new_word], current_word);
    author->word_frequency[*new_word] = 1;
    (*new_word)++;
    return;
}


int readPaper(FILE *pF, paper_t *author, int *new_word, paper_t *original) {
    char current_word[MAX_WORD_LENGTH];
    int counter = 0;

    while (fscanf(pF, "%s", current_word) == 1) {
        counter++;
        if (isValidWord(current_word) == 1) {
            capitalLetter(current_word);
            compareWords(current_word, author, new_word, original);
        }
        
    }
    return counter;
}



