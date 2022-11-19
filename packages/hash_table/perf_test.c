#include <string.h>
#include "hash_table.h"
#include "linked_list.h"
#include <stdio.h> 
#include <ctype.h>

#define Delimiters "\"_+-#@()[]{}.,:;!? \t\n\r"

bool char_in_delim(char c)
{
    for (size_t i = 0; i < strlen(Delimiters); i++)
    {
        if (c == Delimiters[i]) { return true; }
    }
    return false;
}


void line_to_words(char *line, ioopm_list_t *list)
{
    char word_buf[255] = {0}; // here we store the current word we are scanning in.
    int current_buf_len = 0; // The current extent to which we have copied over characters.
    for (size_t i = 0; i < strlen(line); i++)
    {
        // Character setup
        char current_char = line[i];
        if(!char_in_delim(current_char)) // // letter -> insert into word
        {
            current_char = (char)tolower(current_char); 
            word_buf[current_buf_len] = current_char;
            current_buf_len++;
        }
        else if(current_buf_len) // non-letter -> insert word into list, start over
        {
            if (current_buf_len > 0) // insert word into list if word > 0
            {
                word_buf[current_buf_len + 1] = '\0'; // insert null
                ioopm_linked_list_append(list, word_buf, current_buf_len + 1); // insert word into list
            }
            // reset buf
            memset(word_buf, '\0', 255);    // clear buf
            current_buf_len = 0;            // reset len
        }
    }
    if (current_buf_len > 0) // insert word if there's anything left after the loop
    {
        word_buf[current_buf_len + 1] = '\0'; // insert null
        ioopm_linked_list_append(list, word_buf, current_buf_len + 1); // insert word into list
    }  
}


ioopm_hash_table_t *words_to_hash_table(ioopm_list_t *list)
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(26, ioopm_string_order, ioopm_int_order, ioopm_hash_function_alphabetical, ioopm_alloc_simple_ptr, ioopm_alloc_simple_ptr, ioopm_free_simple_ptr, ioopm_free_simple_ptr);
    int listlen = ioopm_linked_list_size(list);
    for (int i = 0; i < listlen; i++)
    {
        void *data;
        ioopm_linked_list_get(list, i, &data);
        if (ioopm_hash_table_has_key(ht, data)) // Does the word already exist in ht?
        {
            void *prevalence = NULL;
            ioopm_hash_table_remove(ht, data, &prevalence); // retrieve prevalence - remove entry.
            *(int *)prevalence = *(int *)prevalence + 1;
            ioopm_hash_table_insert(ht, data, prevalence, strlen(data) + 1, sizeof(int)); // Insert entry again with an increment to prevalence
            free(prevalence);
        }
        else // new word 
        {
            int prevalence = 1;
            ioopm_hash_table_insert(ht, data, (void *)&prevalence, strlen(data) + 1, sizeof(int));
        }
        //printf("\n  Word(%d): '%s'", i, (char *)data);
        free(data);
    }
    return ht;
}


ioopm_list_t *process_file(FILE *f)
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  while (!feof(f))
  {
    char *buf = NULL;
    size_t len = 0;
    getline(&buf, &len, f);
    line_to_words(buf, list);
    free(buf);
  }
  return list;
}


void print_entry (void *key, void **data, void *extra)
{
    printf(" ['%s', %d] ", (char *)key, **(int **)data);
}


int main(int argc, char *argv[]) 
{
    if (argc >= 2)
    {
        for (size_t i = 1; i < argc; i++)
        {
            FILE *in = fopen(argv[i], "r");
            ioopm_list_t *word_list = process_file(in);
            ioopm_hash_table_t *ht = words_to_hash_table(word_list);
            ioopm_hash_table_apply_to_all(ht, print_entry, NULL);
            ioopm_hash_table_destroy(&ht);
            ioopm_linked_list_destroy(&word_list);
            fclose(in);
        }
    }
    else
    {
        printf("Usage: '%s FILENAME_1 FILENAME_2 ... FILENAME_n'", argv[0]);
    }
    return 0;
}