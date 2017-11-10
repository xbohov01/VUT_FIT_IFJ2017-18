#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ALLOC_CONST 10

typedef struct token {
    char *token_contet;
    int attribute;

    int type;
    // Standard values
    int length;
    int line;
    int position;
} Token;

typedef 
enum {as_b, asc_b, declare_b, dim_b, do_b, double_b, else_b, end_b, chr_b, function_b, if_b, input_b, integer_b, length_b, loop_b, print_b, return_b, scope_b, string_b, substr_b, then_b, while_b} \
    all_keys;

typedef enum {
    number,
    func,
    string,
    brace,
    identifier,
} key_type;

const char *key_type_name = {"number", "func", "string", "brace", "identifier"}

int keys_len = 22;

const char *keys[22] = {"as", "asc", "declare", "dim", "do", "double", "else", "end", "chr", "function", "if", "input", "integer", "length", "loop", "print", "return", "scope", "string", "substr", "then", "while"};


void token_from_file(char *filename);

int main() {
    // Item *i;
    // Data *d;
    // Data *nd;
    // i = malloc(sizeof(Item));
    // printf("Malloc item = %d, adr: %p\n", i != NULL, i);

    // d = malloc(sizeof(Data));
    // printf("Malloc data = %d, adr: %p\n", d != NULL, d);
    // d->content = malloc(30 * sizeof(char));
    // printf("Malloc content = %d, adrL %p\n", d->content != NULL, d->content);
    // d->next = NULL;
    // i->first = d;
    // for (int i = 0; i < 30; i++) {
    //     nd = malloc(sizeof(Data));
    //     printf("%d Malloc data = %d, adr: %p\n", i, nd != NULL, nd);
    //     nd->content = malloc(30 * sizeof(char));
    //     nd->next = NULL;
    //     printf("%d Malloc content = %d, adrL %p\n", i, nd->content != NULL, nd->content);
    //     char str[30] = "Number:";
    //     sprintf(str, "%s %d", str, i);
    //     strcpy(nd->content, str);
    //     d->next = nd;
    //     d = nd;
    // }
    // d = i->first;
    // Data *d_for_free = d;
    // while (d != NULL) {
    //     d_for_free = d;
    //     d = d->next;
    //     if (strlen(d_for_free->content) != 0) {
    //         printf("%s\n", d_for_free->content);
    //     }
    //     free(d_for_free->content);
    //     free(d_for_free);
    // }
    // free(i);
    // char *str;
    if (argc != 2) {
        perror("File is not specified");
        return INTERNAL_ERR;
    }
    char *filename = argv[2];
    token_processing(filename);
    return 0;
}

/* returns NULL if token is not jet ready, or EOF when error */
token get_token(char* word) {
    Token tk;
    int word_len = strlen(word);
    char c;
    for(int i=0; i < keys_len; i++) {
        if (strcmp(word, keys[i]) == 0) {
            switch(i) {
                case as_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    tk->type = identifier;
                    tk->content = word;
                    break;
                case asc_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case declare_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case dim_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case do_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case double_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case else_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case end_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case chr_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case function_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case if_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case input_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case integer_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case length_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case loop_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case print_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case return_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case scope_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case string_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case substr_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case then_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                case while_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
                    break;
                default:
                    printf("%s This string is different, that is an error\n", word);
            }
        }
        else {
            printf("Some other token %s\n", word);
        }
    }

    if (isdigit(word[0]) == true) {
        for (int i = 1, i < strlen(word), i++)
            int expected = number;
            if (!isdigit(word[i]) || word[i] != '.') {
                // TODO: Not expect point twice
                fprintf(stderr, "Expected %s but got char '%c'\n", key_type_name[number], word[c]);
                exit(1);
            }
        tk->type = number;
        printf("\n");
    }

}

void token_processing(char *file_name) {
    FILE input_f;
    token token_for_element;
    char *word;
    char c;
    int word_len = 1;
    int capacity = ALLOC_CONST;
    bool char_is_space = false;
    bool last_white_char = true;
    bool is_comm_or_string = false;
    bool is_special = false;
    bool save_char = true;
    bool eval_token = false;
    bool ignore_space_appearence = false;

    if ((input_f = fopen(file_name, "r")) == NULL) {
        fprintf(stderr, "Can not open file %s\n", filename);
        exit(99);
    }

    // Let's malloc some space for word
    void *mem;
    if ((mem = malloc(sizeof(char) * ALLOC_CONST)) == NULL) {
        fprintf(stderr, "Malloc error\n");
        exit(99);
    }
    word = (char *)mem;

    while ((c = fgetc(input_f)) != EOF) {   
        // White space case
        if ((c = ' ' || c = '\t') && last_white_char = false && ignore_space_appearence == false) {
            last_white_char = true;
            save_char = false;
            eval_token = true;
        }
        // New line case
        else if (c = '\n') {
            // Anyway it'll be the end of token (or line in this case)
            is_comm_or_string = false;
            // Special case, becaurse the next line should have first char included in word
            last_white_char = false;
            save_char = false;
            eval_token = true;
        }
        // Possible comment or string starter case
        else if ((c = '/' || c = '!') && is_comm_or_string == false) {
            is_special = true;
            save_char = true; // TODO: not sure
            eval_token = false;
        }
        // String case
        else if (c = '"' && is_special == true) {
            // String literal is still going
            if (is_comm_or_string == true) {
                // So we will end it
                is_comm_or_string = false;
                ignore_space_appearence = false;
                save_char = true;
                eval_token = false;
                token_for_element = get_token(word);
            }
            // Means end of string literal 
            else {
                save_char = true;
                ignore_space_appearence = true;
                is_comm_or_string = true;
                eval_token = true;
            }
        }
        else if (c = '\'') {
            if (is_special == true) {

            }
            else
            {
                is_comm_or_string = true;
                ignore_space_appearence = true;
                save_char = true;
                eval_token = false;
                
            }
        }
        

        // End operations
        sprintf(word, "%s%c", word, c);
        if (eval_token == true) {
            token_for_element = get_token(word);
            word = "";
            word_len = 0;
            // TODO: line and/or position set to zero
        }
        if (word_len >= capacity) {
            mem = realloc(word, sizeof(char) * (capacity + ALLOC_CONST));
            if (mem == NULL) {
                fprintf(stderr, "Reallcation error\n");
                exit(99);
            }

        }
        word_len++;
    }

    fclose(input_f);

    return;
}