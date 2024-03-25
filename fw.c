#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_K 10 /* default amount of words to report*/
#define SIZE 10000 /* intial size of list of entries*/


struct hashtable{
   int elements;
   int size;
   struct entry *item;
};

struct entry
{
    int count;
    char *word;
};

struct hashtable *resize(struct hashtable *oldTable);
struct hashtable *insert_entry(struct hashtable *entryarray, char *key);
int search(struct hashtable *entryarray, char *key);




int hashCode(char *key) {
  int i;
  int hashNumber = 0;
  int prime[5] = {2, 3, 5, 7, 9};

  for (i = 0; key[i] != '\0'; i++) {
    hashNumber += (int)key[i] * prime[i % 5];
  }
  return hashNumber;
}

struct hashtable *resize(struct hashtable *oldTable) {
  int i;

  struct hashtable *newTable = 
  (struct hashtable *)malloc(sizeof(struct hashtable));
  struct entry *newBuckets =  
  ((struct entry *)malloc(sizeof(struct entry) * oldTable->size * 2));
  newTable->item = newBuckets;
  newTable->elements = oldTable->elements;
  newTable->size = oldTable->size * 2;

  if (newTable == NULL) {
    perror("Malloc FAILED");
  }

  for (i = 0; i < oldTable->size; i++) {
    if (oldTable->item[i].word != NULL) {
      newTable = insert_entry(newTable, oldTable->item[i].word);
      newTable->elements--;
      newTable->item[search(newTable, oldTable->item[i].word)].count =
          oldTable->item[i].count;
    }
  }
  return newTable;
}

struct hashtable *insert_entry(struct hashtable *entryarray, char *key) {
  /* get hash */
  int hashIndex = hashCode(key) % entryarray->size;

  if (entryarray->item[hashIndex].word == NULL) {
  } else {
    while (entryarray->item[hashIndex].word != NULL) {
      if (!strcmp(entryarray->item[hashIndex].word, key)) {
        entryarray->item[hashIndex].count++;
        return entryarray;
      } else {
        hashIndex++;
      }
    }
  }

  entryarray->item[hashIndex].word = malloc(strlen(key) + 1);
  /*entryarray->item[hashIndex].word = strdup(key);*/
  strcpy(entryarray->item[hashIndex].word, key);
  entryarray->item[hashIndex].count = 1;
  entryarray->elements++;

  if (entryarray->elements > entryarray->size / 2) {
    entryarray->item = resize(entryarray)->item;
    entryarray->size = entryarray->size * 2;
  }

  return entryarray;
}

int search(struct hashtable *entryarray, char *key) {
  /* get the hash*/
  int item = hashCode(key) % entryarray->size;
  int hashIndex = item;

  /* move in array until an empty*/
  while (entryarray->item[hashIndex].word != NULL) {

    /* compares the key within the struct to the key passed, if the same*/
    /* return */
    if (!strcmp(entryarray->item[hashIndex].word, key)) {
      return hashIndex;
    }

    /* else go to next cell */
    hashIndex++;

    /* wrap around the table */
    hashIndex %= entryarray->size;
  }
  return 0;
}

int compare(const void *worda, const void *wordb) {
  const struct entry *a = (const struct entry *)worda;
  const struct entry *b = (const struct entry *)wordb;

  if (a->count == b->count) {
    return strcmp(b->word, a->word);
  } else {
    return b->count - a->count;
  }

}

int main(int argc, char *argv[]) {
    FILE *fptr = stdin;
    char c, opt;
    int i;
    int idx = 0;
    int k = DEFAULT_K; 
    bool noFiles;
    char *wordstr = '\0';   /* temporary working string */
    int wordstr_size = 0;   /* initial working string size */
    struct entry *newArray;
    
    /* intializing hash table */
    struct hashtable *entryarray = 
    (struct hashtable*)malloc(sizeof(struct hashtable));
    struct entry *entries = 
    (struct entry *)calloc(SIZE, sizeof(struct entry));
    entryarray->item = entries;
    entryarray->elements = 0;
    entryarray->size = SIZE;


    /* parsing command line */
    while((opt = getopt(argc, argv, "n:")) != -1) { /* looking for '-n' */
        switch(opt) {
            case 'n':
                if (atoi(optarg) > 0) { /* convert optarg to int */
                    k = atoi(optarg);
                }
                else {
                    printf("usage: fw [-n num] [ file1 [ file 2 ...] ]\n");
                    return -1;
                }
                break;
        }
    }
    
    /* processing files on at a time */
    noFiles = true;
    for(; (optind < argc) || (optind==argc && noFiles); optind++) {
        if (optind < argc) { 
            noFiles = false;
        } 
        if (noFiles == false) {
            fptr = fopen(argv[optind], "r");
            if (fptr == NULL) {
                perror(argv[optind]);
                continue;
            }   
           /* printf("File being processed: %s\n", argv[optind]);*/
        } else {
            /*printf("Using stdin only:\n");*/
        }
        
        /* read one at a time; test and convert to lowercase*/
        while((c = tolower(fgetc(fptr))) != EOF) { 
            while(isalpha(c)) {
                wordstr = 
                realloc(wordstr, ((++wordstr_size+1) * sizeof(char)));
                strncat(wordstr, &c, 1);
                c = tolower(fgetc(fptr));
            }
            
            
            if (wordstr_size > 0) {
                entryarray = insert_entry(entryarray, wordstr);
                wordstr[0] = '\0'; 
                /* reset working string after every word processing*/
                wordstr_size = 0; /* same with size */
            }


        }
        if (fptr != NULL) { /* closes all files */
            fclose(fptr);
        }
    }    
       


    /* sorting with hash table*/

    newArray = 
      ((struct entry *)malloc(sizeof(struct entry) * entryarray->elements));
    for (i = 0; i < entryarray->size; i++) {
        if (entryarray->item[i].word == NULL) {
        continue;
        } else {
        newArray[idx] = entryarray->item[i];
        idx++;
        }
    }

    qsort(newArray, entryarray->elements, sizeof(struct entry), compare);

    printf("The top %d words (out of %d) are:\n", k, entryarray->elements );
    for (i = 0; i < k; ++i) {
        if (i == entryarray->elements) {
        break;
        }
        printf("%9d %s\n", newArray[i].count, newArray[i].word);
    } 
    
    
    /* freeing up memory */
    free(wordstr);


    return 0;
}
