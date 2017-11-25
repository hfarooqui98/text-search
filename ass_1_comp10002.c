/*     Project 1 sollution for Foundations of Algorithm; Semester 2, 2017
   Written by: Hummad Farooqui       Last Edited: Sunday 17th September 2017 */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* VOLUBLE = 1: View all output scores | VOLUBLE = 0: View only top 5 */
#define VOLUBLE 1
/* Maximum arguments that can be provided to be searched and ranked */
#define MAX_ARGS 100
/* Maximum characters in a line accounting for \r and \n */
#define MAX_CHAR 1002
/* Structure for individual score */
typedef struct {
        int line;
        double score;
        char *string;
} score_t;
score_t *topscores[5];

/************************* Function Prototypes ********************************/

void itter_input(FILE *text);
void check_top(int line, double score, char *string);
void print_topscores(int line, double score, char *string);
int check_query(char *string[], int input_len);
int read_input(int argc, char *argv[]);
int suffix_match(char *query, char *line, int bytes);
double strscore(char *string, int words, int bytes);
double calc(double sum_frequency, double words);

/******************************************************************************/
/* Array of pointers to strings which do not meet alphanumeric,
   lowercase requirments */
char *incomp[MAX_ARGS];
/* Array of pointers to strings part of the query provided by user */
char *query[MAX_ARGS];
/* Counts for amount of incompatible entries (if any) as well as well as amount
   of queries submited */
int incomp_count = 0, query_count = 0;

int
main(int argc, char *argv[]) {
        /* Stage 1 of assignment */
        if(read_input(argc, argv)) {
                return 0;
        }
        /* Initialising array of top 5 scores */
        int i;
        score_t add;
        for(i = 0; i <= 4; i++) {
                add.line = 0;
                add.score = 0;
                add.string = NULL;
                topscores[i] = &add;
        }
        /* Stage 2 and 3 of Assignment */
        itter_input(stdin);
        printf("---------------------------------------------------------\n");
        /* Stage 4
           int line;
           double score;
           char *string;
           for(i = 0; i<= 4; i++) {
                line = topscores[i]->line;
                score = topscores[i]->score;
                string = topscores[i]->string;
                printf("STRING: %s\n",string);
                print_topscores(line,score,string);
           } */
        return 0;
}

/**************** Funtion to read and check input correctness *****************/

int
read_input(int argc, char *argv[]){
        /* Variable input_len serves the purpouse of defining limit of argv */
        int input_len = argc;
        /* Checking to see at least one argument is provided to program */
        if (argc >= MAX_ARGS || argc == 1) {
                printf("S1: No query specified, must provide at least one word\n");
                return 1;
        }
        /* Returning what the user input to query */
        int i;
        incomp_count = check_query(argv, input_len);
        printf("S1: query = ");
        for (i = 1; (i <= argc) && (argv[i] != '\0'); i++) {
                printf("%s ", argv[i]);
        }
        printf("\n");
        /* Checking to see all arguments consist solely of lowercase
           alphanumeric charecters and if not printing invalid ones */
        if (incomp_count) {
                printf("S1:");
                for (i = 0; i < incomp_count; i++) {
                        printf(" %s", incomp[i]);
                }
                printf(": invalid character(s) in query\n");
                return 1;
        }
        printf("---\n");
        return 0;
}

/*********** Function to check the correct and incorrect query words **********/

int
check_query(char *string[], int input_len) {
        int i, j;
        /* First loop to itterate through the pointers to inputs strings */
        for (i = 1; (i <= input_len) && (string[i] != '\0'); i++) {
                int incomp_found = 0;
                /* Second loop to itterate through the charecters in string */
                for (j = 0; string[i][j] != '\0'; j++) {
                        char check = string[i][j];
                        /* Checking if char is not upper
                           case or non alphanumeric */
                        if (!(isalnum(check)) || (isupper(check))) {
                                incomp[incomp_count] = string[i];
                                incomp_found = 1;
                                incomp_count++;
                                break;
                        }
                }
                /* Checks to see if the string checked was
                   correct and if so adds to query */
                if (!(incomp_found)) {
                        query[query_count] = string[i];
                        query_count++;
                }
        }
        return incomp_count;
}

/******** Function to itterate through input file and print details ***********/

void
itter_input(FILE *text) {
        int line = 0, bytes = 0, words = 0, i = 0, encounter_alnum = 0;
        /* Array to witch fgets puts in the complete line */
        char string[MAX_CHAR];
        /* Using fgets to itterate through each line of the file */
        while (fgets(string, MAX_CHAR, text) != NULL) {
                /* itterating through each charecter in
                   the string breaking at new line */
                for (i = 0; string[i] != '\n'; i++) {
                        /* in case the text also has a new carrige character
                           ignore it otherwise increase byte count*/
                        if (string[i] != '\r') {
                                bytes++;
                        }
                        /* checking if alphanumeric encountered
                           if so that means a word has started */
                        if (isalnum(string[i])) {
                                encounter_alnum = 1;
                        }
                        /* if character we are checking isnt alphanumeric and
                           the word has started that means the word has ended so
                            increment word count and reset encounter_alnum check
                            as word hasnt started */
                        if (!(isalnum(string[i])) && encounter_alnum) {
                                words++;
                                encounter_alnum = 0;
                        }
                }
                line++;
                /* Prints out details for every line if line isnt empty */
                double score = strscore(string, words, bytes);
                if (VOLUBLE && bytes > 0) {
                        printf("%s", string);
                        printf("S2: line = %d, bytes = %d, words = %d\n", line,
                               bytes, words);
                        printf("S3: line = %d, score = %.3f\n", line,
                               score);
                        printf("---\n");
                }
                /* Checks current line against top 5 scoring lines realocating
                   if necessary*/
                check_top(line, score, string);
                words = 0;
                bytes = 0;
        }
}

double
log2(double x) {
        return log(x) / log(2.0);
}

/************ Function that scores each line for user's search ****************/

double
strscore(char *string, int words, int bytes) {
        int i = 0;
        double sum_frequency = 0;
        /* goes through each query, finds its score for suffix_match and then
           adds it to the total frequency to calculate numerator */
        for (i = 0; i <= (query_count - 1); i++) {
                sum_frequency +=
                        log2(1.0 + suffix_match(query[i], string, bytes));
        }
        return calc(sum_frequency, words);
}

/********************** Function to check suffix repeats **********************/

int
suffix_match(char *query, char *line, int bytes) {
        int match_count = 0, i = 0, j = 0, word_len = strlen(query);
        int line_len = bytes, encounter_alnum = 0;
        while (i <= line_len) {
                /* checks to see if character is alnum and that a word hasnt
                   already started */
                if (isalnum(line[i]) && !encounter_alnum) {
                        encounter_alnum = 1;
/* j is used to itterate through the query string */
                        j = 0;
                        /* start moving through the line */
                        for (; isalnum(line[i]); i++) {
                                char check_char = tolower(query[j]);
                                /* if the first character of the word doesnt
                                   match with that of the query exit loop and
                                   restart when next word starts */
                                if((tolower(line[i]) != check_char)) {
                                        break;
                                }
                                /* if a character matches and is the last
                                   character of the query than a complete suffix
                                   match has been found */
                                if ((tolower(line[i]) == check_char) &&
                                    (j == word_len - 1)) {
                                        match_count++;
                                        break;
                                }
                                /* if the character matches but the word query
                                   word hasnt ended increment j */
                                if ((tolower(line[i]) == check_char) &&
                                    (j < word_len - 1)) {
                                        j++;
                                }
                        }
                }
                /* this handles the situation where we have broken out of a loop
                   without the word in the line completing then it will prevent
                   from entering the for loop untill the end of that word has
                   been reached */
                if (!isalnum(line[i])) {
                        encounter_alnum = 0;
                }
                i++;
        }
        return match_count;
}

/************************ Function to calculate score *************************/

double
calc(double numerator, double words) {
        return numerator / ((log(8.5 + words)) / (log(2.0)));
}

/************* Function to check and assign top 5 socred lines ****************/

void
check_top(int line, double score, char *string) {
        double score_check = score;
        int line_check = line, i;
        char *string_assign = string;
        for(i = 0; i <= 4; i++) {
                if(score_check > topscores[i]->score) {
                        if(i != 4) {
                                topscores[i+1]->score = topscores[i]->score;
                                topscores[i+1]->line = topscores[i]->line;
                                topscores[i+1]->string = topscores[i]->string;
                        }
                        topscores[i]->string = string_assign;
                        topscores[i]->score = score_check;
                        topscores[i]->line = line_check;
                        break;
                }
                if(score_check == topscores[i]->score &&
                   (topscores[i]->score != 0)) {
                        if(line_check > topscores[i]->line) {

                        }
                        if(line_check < topscores[i]->line) {

                        }
                }
        }
}

/**************************** Print Top 5 scoring lines ***********************/
void print_topscores(int line, double score, char *string) {
        if(score) {
                printf("S4: line = %d, score = %.3lf\n",line,score);
                printf("%s\n",string);
                printf("---\n");
        }
}
/* Algorithms are fun!! */
