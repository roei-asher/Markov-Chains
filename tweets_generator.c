#include "markov_chain.h"
#include <string.h>

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"

#define DECIMAL_BASE 10
#define MIN_EXPECTED_ARGS 4
#define MAX_EXPECTED_ARGS 5
#define MAX_LINE_LENGTH 1000
#define MAX_TWEET_LENGTH 20
#define DEFAULT_WORDS_TO_READ -1

// --------------------- FUNCTIONS -----------------------

char *strdup(const char *str)
{
    size_t len = strlen(str) + 1;
    char *new_str = malloc(len);
    if (!new_str){return NULL;}
    memcpy(new_str, str, len);
    return new_str;
}

void *copy_string(const void *data){return strdup((const char *)data);}

// void print_string(const void *data) {printf("%s", (const char *)data);}
void print_string(const void *data) {printf(" %s", (const char *)data);}

int compare_strings(const void *a, const void *b)
{return strcmp((const char *)a, (const char *)b);}

bool is_last_string(const void *data) {
    const char *str = (const char *)data;
    size_t len = strlen(str);
    return (str[len - 1] == '.') ? true : false;
}

// -------------------------------------------------------
void read_file(FILE *fp, MarkovChain *markov_chain, int words_to_read);
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain);
// -------------------------------------------------------
bool preprocessed(int argc, char **argv, unsigned int *seed, int *num_tweets,
    FILE **fp, int *words_to_read)
{
    if (argc < MIN_EXPECTED_ARGS || argc > MAX_EXPECTED_ARGS)
        {
        fprintf(stderr, NUM_ARGS_ERROR);
        return false;
        }
    *seed = strtol(argv[1], NULL,DECIMAL_BASE);
    srand(*seed);
    *num_tweets = strtol(argv[2], NULL, DECIMAL_BASE);
    char *file_path = argv[3];
    *words_to_read = DEFAULT_WORDS_TO_READ;
    if (argc == MAX_EXPECTED_ARGS)
        {
        *words_to_read = strtol(argv[4], NULL, DECIMAL_BASE);
        }
    *fp = fopen(file_path, "r");
    if (!*fp)
        {
        fprintf(stderr, FILE_PATH_ERROR);
        return false;
        }
    return true;
}

int main(int argc, char *argv[])
{
    unsigned int seed;
    int num_tweets;
    FILE *fp;
    int words_to_read;
    // Preprocess CLI input
    if (!preprocessed(argc, argv, &seed, &num_tweets, &fp, &words_to_read))
        {
        return EXIT_FAILURE;
        }
    // "Train" Markov chain model on text corpus
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (!markov_chain)
        {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
        }
    markov_chain->database = NULL;
    markov_chain->copy_func = (copy_func_t)copy_string;
    markov_chain->comp_func = (comp_func_t)compare_strings;
    markov_chain->free_data = (free_data_t)free;
    markov_chain->print_func = (print_func_t)print_string;
    markov_chain->is_last = (is_last_t)is_last_string;
    if (fill_database(fp, words_to_read, markov_chain) == EXIT_FAILURE)
        {
        free_markov_chain(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
        }
    // Make "predictions" of tweets (create user specified tweets)
    for (int i = 1; i <= num_tweets; i++)
        {
        MarkovNode *first_node = get_first_random_node(markov_chain);
        printf("Tweet %d:", i);
        generate_random_sequence(markov_chain, first_node, MAX_TWEET_LENGTH);
        }

    free_markov_chain(&markov_chain);
    fclose(fp);

    return EXIT_SUCCESS;
}

int read_and_process_file(FILE *fp, int words_to_read, MarkovChain
    *markov_chain)
{
    char line[MAX_LINE_LENGTH];
    MarkovNode *prev_node = NULL;
    int words_read = 0;
    // Read file line by line
    while (fgets(line, sizeof(line), fp)
           && (words_to_read == DEFAULT_WORDS_TO_READ ||
               words_read < words_to_read))
        {
        char *token = strtok(line, DELIMITERS);
        // Process each word (token) in the line
        while (token && (words_to_read == DEFAULT_WORDS_TO_READ ||
            words_read < words_to_read))
            {
            char *word = strdup(token);
            if (!word) {return EXIT_FAILURE;}
            Node *current_node = add_to_database(markov_chain, (void*)word);
            free(word);
            if (!current_node){return EXIT_FAILURE;}
            MarkovNode *markov_node = current_node->data;
            if (prev_node)
                {
                if (add_node_to_frequency_list(prev_node, markov_node,
                    markov_chain) == EXIT_FAILURE){return EXIT_FAILURE;}
                }
            if (markov_chain->is_last(markov_node->data)) {
                prev_node = NULL;
            }
            else {prev_node = markov_node;}
            token = strtok(NULL, DELIMITERS);
            words_read++;
            }
        }
    return EXIT_SUCCESS;
}

int validate_and_finalize_database(MarkovChain *markov_chain) {
    // Check if we have enough words
    if (markov_chain->database->size < 2){return EXIT_FAILURE;}

    Node *current = markov_chain->database->first;
    int max_sentence_length = 0;
    int current_sentence_length = 0;

    while (current)
        {
        MarkovNode *node = current->data;
        current_sentence_length++;
        if (markov_chain->is_last(node->data))
            {
            if (current_sentence_length > max_sentence_length)
                {
                max_sentence_length = current_sentence_length;
                }
            current_sentence_length = 0;
            }
        current = current->next;
        }
    if (max_sentence_length < 2){return EXIT_FAILURE;}

    return EXIT_SUCCESS;
}

int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
    // Allocate memory for the database
    markov_chain->database = malloc(sizeof(LinkedList));
    if (!markov_chain->database)
        {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
        }
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;
    markov_chain->database->size = 0;

    // Read and process the file
    if (read_and_process_file(fp, words_to_read, markov_chain) == EXIT_FAILURE)
        {
        return EXIT_FAILURE;
        }

    // Validate the database
    if (validate_and_finalize_database(markov_chain) == EXIT_FAILURE)
        {
        return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;
}
