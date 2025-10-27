#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"


//Don't change the macros!
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"


#define MAX_TWEET_LENGTH 20

// //fill the markov chain with given data
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
    int words_counter = 0;
    char buffer[1004] = {0};
    Node *last_node = NULL, *curr_node = NULL;
    while ((words_counter < words_to_read || words_to_read == -1) && fgets(buffer, sizeof(buffer), fp))
    {
        char* word1 = strtok(buffer, DELIMITERS);
        last_node = NULL;

        while ((words_counter < words_to_read || words_to_read == -1) && word1 != NULL)
        {
            words_counter++;
            curr_node = add_to_database(markov_chain, word1);
            if (curr_node == NULL) {
                return 1; // failed allocation
            }

            if (last_node != NULL) {
                // Add to the frequency list
                if (add_node_to_frequency_list(last_node->data, curr_node->data) == 1)
                {
                    // failed allocation
                    return 1;
                }
            }

            // Move word1 to word2 for next iteration
            word1 = strtok(NULL, DELIMITERS);
            last_node = curr_node;
        }
    }

    return 0;
}

MarkovChain* create_markov_chain()
{
  MarkovChain* chain = malloc(sizeof(MarkovChain));
  if (chain == NULL)
    return NULL;  // Memory allocation failure

  // Create the linked list to hold nodes
  chain->database = calloc(1, sizeof(LinkedList));
  if (chain->database == NULL)
  {
    free(chain);
    chain = NULL;
    return NULL;  // Memory allocation failure
  }

  return chain;
}

int main (int argc, char *argv[])
{
  //check number of arguments
  if (argc != 4 && argc !=5)
    {
        fprintf(stdout, "%s",  NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    int num_tweets = (int)strtol(argv[2], NULL, 10);
    int seed = strtol(argv[1], NULL, 10);
    srand(seed);
  //check if file is good
  FILE *file = fopen(argv[3], "r");
  if (file == NULL)
    {
      fprintf(stdout, "%s",  FILE_PATH_ERROR);
      return EXIT_FAILURE;
    }
  int words_to_read = -1;

  if (argc == 5)
  {
    words_to_read = (int)strtol(argv[4], NULL, 10);
  }

  MarkovChain* markov_chain = create_markov_chain();
  int result = fill_database(file, words_to_read, markov_chain);
    fclose(file);

  if (result == 1)
  {
    free_database(&markov_chain);
    return EXIT_FAILURE;
  }

  for (int i=0; i < num_tweets; i++) {
      MarkovNode* first_node = get_first_random_node(markov_chain);
      printf("Tweet %d: ", i + 1);
      generate_tweet(first_node, MAX_TWEET_LENGTH);
      printf("\n");
  }

  free_database(&markov_chain);
  return 0;
}
