#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "wc.h"
#include <string.h>
#include <ctype.h>

//type define table_element 
typedef struct element{
    
    char *content;
    int chain_num; 
    struct element *next;
    
} table_element;



struct wc {
	/*defining wc to have size and table*/
    
    long size_t; 
       //int does not work here
    table_element **hash_table;
    
};

int insert_element(struct wc *wc, char *file_words, int table_index);
int hash_func(char* file_words, struct wc *hash_tab);
void table_fill (char *input, struct wc *wc);



struct wc *
wc_init(char *word_array, long size)
{
        //initializing structures and their members
    
	struct wc *wc;

        //allocating memory for wc 
	wc = (struct wc *)malloc(sizeof(struct wc));
	assert(wc);

        //allocating memory for wc table
	wc-> hash_table = (table_element **)malloc(sizeof(table_element *) * size);
        assert(wc -> hash_table);
        
        //allocating memory for size member of wc
        wc-> size_t = size;
        
        table_fill(word_array, wc);

	return wc;
}



void table_fill(char *entry, struct wc *wc)
{
    
  int position = 0; 
  int char_count = 0; 
  int word_count = 0;
  int parse_index = 0; 
  char *words = NULL;
  int hash_index;
  
  for(position = 0; entry[position]!= '\0'; position++) 
    {
      if(isspace(entry[position]))
	{ //white space detected
	  if(char_count != 0)
	  { //there are word(s) before space
              
	    words = (char *)malloc((char_count+1)*sizeof(char));  
            //allocate memory for the size of chars in word + terminating null
	    assert(words);
	    
	    for(parse_index = 0; parse_index < char_count; parse_index++)
	      {
		words[parse_index] = entry[word_count+parse_index];
                //parse new word into table "words" at a location after the last read word
	      }
            
	    words[parse_index] = '\0'; //add the null
            hash_index = hash_func(words, wc);
	    insert_element(wc,words,hash_index);
	  }
            //reset values
	    parse_index = 0;
	    char_count = 0; 
            word_count = position+1; 
            //update word_count to the pos of letter after the last word
	}
      else
	{
	  char_count++;
	}
    }
}





int 
insert_element(struct wc *wc, char *file_words, int table_index){
    
    if(wc->hash_table[table_index]!=NULL)
    { //something already exist(s) at specified index
        table_element *head = wc->hash_table[table_index];
        table_element *last = NULL;
        
        //Collison resolution by chaining (basically linkedlist)
        while(head != NULL){
            if(strcmp(file_words, head->content)==0) 
            { //words already exist. Add to chain
                head->chain_num = head->chain_num + 1;
                free(file_words);
                return 1;
            }
            last = head;
            head = head->next;
        }
        last->next = (table_element *)malloc(sizeof(table_element));
        last->next->next = NULL;
        last->next->chain_num = 1;
        last->next->content = file_words;
        return 1;
        
    } 
   
        wc->hash_table[table_index]= (table_element *)malloc(sizeof(table_element));
      
        wc->hash_table[table_index]->content = file_words;
        wc->hash_table[table_index]->next = NULL;
        wc->hash_table[table_index]->chain_num = 1; 
        return 1;
    
       
}


//polynomial rolling hash function
int hash_func(char* file_words, struct wc *hash_tab)
{ 
    int string_len = strlen(file_words);
    const int p = 31, m = 1e9 + 7;
    int hash = 0;
    long p_pow = 1;
    for(int i = 0; i < string_len; i++) {
        hash = (hash + (file_words[i] - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return abs(hash % (hash_tab->size_t));
}
 

void
wc_output(struct wc *wc)
{
    for(int i = 0; i < wc -> size_t; ++i)
    {
        table_element *output;
        if(wc->hash_table[i] != NULL){
            output = wc->hash_table[i];
            while(output !=NULL){
                printf("%s:%d\n", output->content, output->chain_num);
                output = output->next;
            }
        }
    }
}


void
wc_destroy(struct wc *wc)
{  //traverse through the table and free all memory allocations
    int traverse = 0;
    while(traverse < wc->size_t){
        table_element *head = wc->hash_table[traverse];
        table_element *proceeding;
        while(head != NULL){
            proceeding = head -> next;
            free(head);
            head = proceeding;
        }
      traverse = traverse + 1;  
    }
	free(wc);
}
