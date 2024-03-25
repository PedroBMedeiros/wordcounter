#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

typedef struct { /* Structure for holding words and their count*/
    int count;
    char *word;
} entry, *ptr_entry;

ptr_entry create_word_entry(char *word) { /* to create word entry */
    ptr_entry result = 
		malloc(sizeof(entry)); /* allocates memory for new entry */
    result->count = 1;
    result->word = malloc(sizeof(char) * strlen(word)); 
		/* ^allocates memory for variable length string^ */
    strcpy(result->word, word); 
    return result;
}

/* check for repeated word */
int check_word_repeat(char *word, ptr_entry *list, int list_size) {
    int i;
    for (i=0; i<list_size; i++) {
        if (strcmp(list[i]->word, word) == 0) {
            return i;
        }
    }
    return -1;
}
/* count, then lexicographical ordering */
int isbigger(ptr_entry a, ptr_entry b) { 
    if (a->count < b->count) {
        return 1;
    } else if (a->count == b->count) {
        if (strcmp(a->word, b->word) > 0) {
            return 1;
        }
    }
    return 0;
}

/* swap pointers*/
void swap(ptr_entry *x,  ptr_entry *y) {
    ptr_entry t;
    t = *x;
    *x = *y;
    *y = t;
}

int main(int argc, char *argv[]) {
    FILE *fptr = stdin;
    char c, opt;
    int i, index, j;
    int k = 10; /* default amount of words to report*/
    bool swapped, noFiles;
    char *wordstr = '\0';   /* temporary working string */
    int wordstr_size = 0;   /* initial working string size */
    ptr_entry* lst_ptrs_to_word_entries = malloc(1 * sizeof(ptr_entry)); 
                      /* ^variable length array of pointers to word entries^ */
    int lst_size = 0;
     
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
            /* printf("File being processed: %s\n", argv[optind]); */
        } else {
            printf("Using stdin only:\n");
        }
        
        /* read one at a time; test and convert to lowercase*/
        while((c = tolower(fgetc(fptr))) != EOF) { 
            while(isalpha(c)) {
                wordstr = realloc(wordstr, ((++wordstr_size+1) * sizeof(char)));
                strncat(wordstr, &c, 1);
                c = tolower(fgetc(fptr));
            }
            if (wordstr_size > 0) {
	/* ^add word only when not repeated, increase count otherwise^ */
                if ( (index = 
			check_word_repeat(
				wordstr, 
				lst_ptrs_to_word_entries,
				lst_size)) >= 0) {
                    lst_ptrs_to_word_entries[index]->count++;
                } else {
		/* extend allocated memory for entry pointers as list grows^ */
                    lst_size++;
                    lst_ptrs_to_word_entries = 
			realloc(lst_ptrs_to_word_entries, 
				sizeof(ptr_entry) * (lst_size));
                    lst_ptrs_to_word_entries[lst_size -1] = 
				create_word_entry(wordstr);
               }
		/* reset working string after every word processing*/
                wordstr[0] = '\0';
                wordstr_size = 0; /* same with size */
            }
            
        }
        if (fptr != NULL) { /* closes all files */
            fclose(fptr);
        }
    }    
   
    /*  sorting */
    /* printf("Sorting....\n"); */
    for(i = 0; i < (lst_size - 1); i++) {
        swapped = false;
        for (j =0; j < (lst_size -i -1); j++) {
            if (isbigger(lst_ptrs_to_word_entries[j],
		 lst_ptrs_to_word_entries[j+1]) > 0) {
                swap(&lst_ptrs_to_word_entries[j],
		 &lst_ptrs_to_word_entries[j+1]);
                swapped = true;
            }
        }
        if (swapped == false) {
            break;
        }
    }
    
    /* Required formatted output*/
    printf("The top %d words (out of %d) are:\n", k, lst_size );
    for (i = 0; (i < k) && (i < lst_size); i++) {
        printf("%9d %s\n", 
	lst_ptrs_to_word_entries[i]->count,
	 lst_ptrs_to_word_entries[i]->word);
    } 

    
    /* freeing up memory */
    for (i = 0; i < lst_size; i++) {
        free(lst_ptrs_to_word_entries[i]->word);
        free(lst_ptrs_to_word_entries[i]);
    }
    free(wordstr);


    return 0;
}
