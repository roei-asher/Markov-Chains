#include "markov_chain.h"
#include <string.h>
/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number){return(max_number)?rand()%max_number:0;}

/**
 * Function to create a new MarkovNode.
 * @param word - the word data for the node.
 * @return - pointer to the new MarkovNode.
 */
MarkovNode* create_markov_node(MarkovChain *markov_chain, void *data_ptr)
{
    if (!markov_chain || !data_ptr) {return NULL;}
    MarkovNode *new_node = malloc(sizeof(MarkovNode));
    if (!new_node){printf(ALLOCATION_ERROR_MASSAGE); return NULL;}
    new_node->data = markov_chain->copy_func(data_ptr);
    if (!new_node->data)
        {
        free(new_node);
        printf(ALLOCATION_ERROR_MASSAGE);
        return NULL;
        }
    // Initialize Node.
    new_node->frequency_list = NULL;
    new_node->frequency_count = 0;

    return new_node;
}

/**
 * Function to create a new MarkovNodeFrequency.
 * @param markov_node - pointer to the markov node.
 * @return - pointer to the new MarkovNodeFrequency.
 */
MarkovNodeFrequency* create_markov_node_frequency(MarkovNode *markov_node)
{
    if (!markov_node) {return NULL;}
    MarkovNodeFrequency *new_frequency = malloc(sizeof(MarkovNodeFrequency));
    if (!new_frequency){printf(ALLOCATION_ERROR_MASSAGE); return NULL;}
    new_frequency->markov_node = markov_node;
    new_frequency->frequency = 0;
    new_frequency->next = NULL;
    return new_frequency;
}

/**
 * Function to get a node from the database.
 * @param markov_chain - the MarkovChain.
 * @param data_ptr - the data pointer to find.
 * @return - pointer to the node, NULL if not found.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
    if (!markov_chain || !data_ptr || !markov_chain->comp_func) {return NULL;}
    Node *current = markov_chain->database->first;
    while (current)
    {
        MarkovNode *markov_node = current->data;
        if (!markov_chain->comp_func(markov_node->data, data_ptr))
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Function to add a word to the MarkovChain.
 * @param markov_chain - pointer to the MarkovChain.
 * @param data_ptr - pointer for the word to add.
 * @return - pointer to the MarkovNode added or found.
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
    if (!markov_chain || !data_ptr) {return NULL;}
    Node *found_node = get_node_from_database(markov_chain, data_ptr);
    if (found_node){return found_node;}
    // data_ptr (word) is not in our MarkovChain => we can add it.
    MarkovNode *new_markov_node = create_markov_node(markov_chain, data_ptr);
    if (!new_markov_node){return NULL;}
    // Created a valid MarkovNode and will now attempt to add to the database.
    if (add(markov_chain->database, new_markov_node) == EXIT_FAILURE)
        {
        markov_chain->free_data(new_markov_node->data);
        free(new_markov_node);
        printf(ALLOCATION_ERROR_MASSAGE);
        return NULL;
        }
    return markov_chain->database->last;
}

/**
 * Add the second MarkovNode to the frequency list of the first MarkovNode.
 * If already in list, update it's occurrence frequency value.
 * @param first_node
 * @param second_node
 * @return EXIT_SUCCESS / EXIT_FAILURE (in the case of allocation error)
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node,
    MarkovChain *markov_chain)
{
    if (!first_node || !second_node){return EXIT_FAILURE;}
    MarkovNodeFrequency *current = first_node->frequency_list;
    MarkovNodeFrequency *prev = NULL;

    // Check if second_node is already in the list
    while (current)
        {
        if (!markov_chain->comp_func(current->markov_node, second_node))
            {
            current->frequency++;
            return EXIT_SUCCESS;
            }
        prev = current;
        current = current->next;
        }

    // If not found, add a new node to the list
    MarkovNodeFrequency *new_freq = malloc(sizeof(MarkovNodeFrequency));
    if (!new_freq){printf(ALLOCATION_ERROR_MASSAGE); return EXIT_FAILURE;}

    new_freq->markov_node = second_node;
    new_freq->frequency = 1;
    new_freq->next = NULL;

    if (prev){prev->next = new_freq;}
    else{first_node->frequency_list = new_freq;}

    first_node->frequency_count++;
    return EXIT_SUCCESS;
}

/**
 * Free MarkovChain and all of its content from memory
 * @param chain_ptr - markov_chain to free
 */

// USED TO BE FUNCTION CALLED "free_database"
void free_markov_chain(MarkovChain **chain_ptr)
{
    if (chain_ptr == NULL || *chain_ptr == NULL){return;}
    MarkovChain *chain = *chain_ptr;
    if (!chain->free_data){return;}
    Node *current = chain->database->first;
    // Loop over all MarkovNode's and free all memory they occupy.
    while (current)
        {
        Node *next = current->next;
        MarkovNode *node = current->data;
        chain->free_data(node->data);
        MarkovNodeFrequency *freq = node->frequency_list;
        while (freq)
            {
            MarkovNodeFrequency *next_freq = freq->next;
            free(freq);
            freq = next_freq;
            }

        free(node);
        free(current);
        current = next;
        }
    free(chain->database);
    free(chain);
    *chain_ptr = NULL;
}

/**
 * Get one random MarkovNode from the given markov_chain's database.
 * @param markov_chain
 * @return the random MarkovNode
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    if (!markov_chain || !markov_chain->database ||
        markov_chain->database->size == 0) {return NULL;}
    int index = get_random_number(markov_chain->database->size);
    Node *current = markov_chain->database->first;
    for (int i = 0; i < index; i++) {
        if (!current) {return NULL;}
        current = current->next;
    }
    MarkovNode *node = current->data;
    while (markov_chain->is_last(node->data))
        {
        index = get_random_number(markov_chain->database->size);
        current = markov_chain->database->first;
        for (int i = 0; i < index; i++) {
            if (!current) {return NULL;}
            current = current->next;
        }
        node = current->data;
        }
    return node;
}

/**
 * Choose randomly the next MarkovNode, depend on its occurrence frequency.
 * @param cur_markov_node - current MarkovNode
 * @return the next random MarkovNode
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    if (!cur_markov_node || !cur_markov_node->frequency_count){return NULL;}
    // Get total frequency
    int total_frequency = 0;
    MarkovNodeFrequency *current = cur_markov_node->frequency_list;
    while (current)
        {
        total_frequency += current->frequency;
        current = current->next;
        }

    int rand_value = get_random_number(total_frequency);
    int cumulative_frequency = 0;
    current = cur_markov_node->frequency_list;
    // Return MarkovNode at random frequency
    while (current)
        {
        cumulative_frequency += current->frequency;
        if (rand_value < cumulative_frequency){return current->markov_node;}
        current = current->next;
        }

    return NULL;
}

/**
 * Starts generating tweet from a random MarkovNode
 * @param first_node markov_node to start with
 * @param max_length maximum length of tweet to generate
 */
void generate_random_sequence(MarkovChain *markov_chain,
    MarkovNode *first_node, int max_length)
{
    if (!markov_chain || !first_node || max_length < 2){return;}
    MarkovNode *current_node = first_node;
    markov_chain->print_func(current_node->data);
    int words_printed = 1;

    while (words_printed < max_length)
        {
        current_node = get_next_random_node(current_node);
        if (!current_node){break;}
        markov_chain->print_func(current_node->data);
        words_printed++;
        if (markov_chain->is_last(current_node->data) ||
            words_printed == max_length)
            {
            break;
            }
        }
    printf("\n");
}

