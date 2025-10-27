#include "markov_chain.h"
#include <string.h>


/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
  if (markov_chain->database == NULL || data_ptr == NULL)
    return NULL;

  Node *current_node = markov_chain->database->first;
  while (current_node != NULL)
  {
    if (strcmp(current_node->data->data, data_ptr) == 0) {
      return current_node;
    }

    current_node = current_node->next;
  }

  return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr)
{
  Node* existing_node = get_node_from_database(markov_chain, data_ptr);
  if (existing_node != NULL)
    return existing_node;

  MarkovNode *node = calloc(1, sizeof(MarkovNode));
  if (node == NULL)
  {
    return NULL;
  }

  node->data = calloc(strlen(data_ptr) + 1, sizeof(char));
  if (node->data == NULL)
  {
    free(node);        // Free new_node
    return NULL;
  }

  strcpy(node->data, data_ptr);

  if (add(markov_chain->database, node) == 1)
  {
    free(node->data);  // Free data_ptr clone
    free(node);         // Free new_node
    return NULL;
  }

  return markov_chain->database->last;
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node)
{
  if (first_node->data[strlen(first_node->data) - 1] == '.')
    return 0;

  for (int i = 0; i < first_node->size; i++) {
    if (first_node->frequency_list[i].markov_node == second_node) {
      first_node->frequency_list[i].frequency++;
      return 0;
    }
  }

  int size = first_node->size;
  MarkovNodeFrequency* temp = realloc(first_node->frequency_list,
                                  (size + 1) * sizeof(MarkovNodeFrequency));
  if (temp == NULL)
    return 1;  // realloc failed, don't overwrite the original pointer

  first_node->frequency_list = temp;
  first_node->frequency_list[size].markov_node = second_node;
  first_node->frequency_list[size].frequency = 1;
  first_node->size++;
  return 0;
}

void free_database(MarkovChain ** ptr_chain)
{
  LinkedList *list = (*ptr_chain)->database;
  // Loop through all nodes and free their data properly
  while (list->first != NULL)
  {
    Node* node_to_free = list->first;
    list->first = list->first->next;  // Move to next node before freeing

    // Free frequency list and its data
    if (node_to_free->data != NULL)
    {
      if (node_to_free->data->frequency_list != NULL)
      {
        free(node_to_free->data->frequency_list);  // Free frequency list
        node_to_free->data->frequency_list = NULL;  // Nullify pointer to avoid use-after-free
      }

      free(node_to_free->data->data);  // Free data
      node_to_free->data->data = NULL;  // Nullify pointer to avoid use-after-free

      free(node_to_free->data);  // Free the MarkovNode associated with the node
      node_to_free->data = NULL;  // Nullify the data pointer
    }

    // Free the node itself
    free(node_to_free);
    node_to_free = NULL;  // Nullify the node pointer to prevent use-after-free
  }

  // Finally, free the database and the MarkovChain structure itself
  free((*ptr_chain)->database);
  (*ptr_chain)->database = NULL;  // Nullify pointer
  free(*ptr_chain);
  *ptr_chain = NULL;  // Nullify pointer to avoid dangling pointer
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
  Node *current_node = NULL;
  do {
    current_node = markov_chain->database->first;
    int random_number = get_random_number(markov_chain->database->size);

    for (int i = 0; i < random_number; i++)
      current_node = current_node->next;
  } while (current_node->data->data[strlen(current_node->data->data) - 1] == '.');

  return current_node->data;
}
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
  if (cur_markov_node->frequency_list == NULL)
    return NULL;

  int sum_of_all_freq = 0;
  for (int i = 0; i < cur_markov_node->size; i++) {
    sum_of_all_freq += cur_markov_node->frequency_list[i].frequency;
  }

  int random_number = get_random_number(sum_of_all_freq);
  for (int i = 0; i < cur_markov_node->size; i++) {
    random_number -= cur_markov_node->frequency_list[i].frequency;
    if (random_number < 0)
      return cur_markov_node->frequency_list[i].markov_node;
  }

  return NULL;
}
void generate_tweet(MarkovNode *first_node, int max_length)
{
  if (first_node == NULL || max_length <= 0)
    return;

  MarkovNode *curr = first_node;
  while (max_length > 0)
  {
    if (curr->frequency_list == NULL)
    {
      printf("%s", curr->data);
      break;
    }

    printf("%s ", curr->data);
    max_length--;
    curr = get_next_random_node(curr);
  }
}