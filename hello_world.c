#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "scanner.h"

typedef enum {true, false} bool;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        perror("File is not specified");
        error_exit(INTERNAL_ERR);
    }

    char *file_name = argv[1];
    int read_result;
    token *tokens;
    token first_token;
    if ((*first_token.word = malloc(MEM_ALLOC_CONST * sizeof(char))) == NULL) {
        error_exit(INTERNAL_ERR);
    }
    strcpy(first_token.word, "someVeryLongLineThatShouldeBiggerThan10");
    printf("%s\n", first_token.word);
    exit(1);
    first_token.id = 5;
    first_token.line = 0;
    printf("FT_p: %p, FT_S: %ld FT.id: %d FT.line: %d FT.word %s\n\n", first_token, sizeof(first_token), first_token.id, first_token.line, *first_token.word);
    if ((tokens = calloc(MEM_ALLOC_CONST, sizeof(token))) == NULL) {
        error_exit(INTERNAL_ERR);
    }
    tokens[0] = first_token;
    if ((read_result = start_scanner(file_name, tokens)) != SUCCESS) {
        error_exit(read_result);
    }

    return SUCCESS;
}

int start_scanner(char *file_name, token *tokens)
{
    token first_token;
    first_token = tokens[0];
    printf("FT_p: %p, FT_S: %ld FT.id: %d FT.line: %d FT.word %s\n\n", first_token, sizeof(first_token), first_token.id, first_token.line, first_token.word);
    exit(1);
    // FILE *input_file;
    // int line_count, token_count, char_count;
    // char c;
    // char *word;
    // bool is_space = false;

    // line_count = token_count = char_count = 0;

    // if ((input_file = fopen(file_name, "r")) == NULL) {
    //     return INTERNAL_ERR;
    // }
    // char *word;
    // while ((c = fgetc(input_file)) != EOF) {
    //     switch(c) {
    //         case '\n':
    //             printf("%s\n", line);
    //             printf("Line: %d  Tokens: %d  Chars: %d\n", line_count, token_count, char_count);
    //             memset(line, 0, strlen(line));
    //             token_count = char_count = 0;
    //             line_count++;
    //             break;
    //             // TODO: add token to struct
    //         case ' ': 
    //             if (is_space == false) {
    //                 is_space = true;
    //                 token_count++;
    //                 char_count = 0
    //                  // = (char *)malloc(strlen(word)* sizeof(char))
    //             }
    //             // Intentionally gave no break for default case execution
    //         default:
    //             line[char_count] = tolower(c);
    //             char_count++;
    //             if (is_space == true) { is_space = false; }
    //             break;
    //     }
    // }
    // free(line);
    // fclose(input_file);
    return SUCCESS;
}

void error_exit(int err_num)
{
    exit(err_num);
}