#ifndef ERRORS_HEADER
#define ERRORS_HEADER
#include "errors.h"
#endif


#include "scanner.h"
#include "list.h"


int main(int argc, char* argv[])
{
    List my_new_list;
    Item *item1;
    token_list_init(&my_new_list);
    printf("Success 1\n");
    item1 = token_init(3, 4);
    printf("%d\n", item1->data->capacity);
    printf("Success 2\n");
    insert_first(&my_new_list, item1);
    printf("Success 3\n");
    if (item1 == NULL) {
        // TODO: info to stderr
        data_free(&my_new_list);
        return INTERNAL_ERR;
    }
    feed_word(item1, "Some neat text");
    printf("Success 4\n");
    printf("item1->data->content: %s, item1->data->content == my_new_list->first->data->content: %d \n", item1->data->content, strcmp(item1->data->content, my_new_list.first->data->content));
    data_free(&my_new_list);
    printf("Success free\n");


    /* Test part for struct token and menory allocation

    if (argc != 2) {
        perror("File is not specified");
        error_exit(INTERNAL_ERR);
    }

    char *file_name = argv[1];
    int read_result;
    Token *tokens;
    Token first_token;
    if ((first_token.content = malloc(sizeof(char) * MEM_ALLOC_CONST)) == NULL) {
        error_exit(INTERNAL_ERR);
    }
    strcpy(first_token.content, "someVeryLongLine_yeahThatIsBigMan_Iknow");
    printf("%s\n", first_token.content);
    free(first_token.content);
    exit(1);

    */
    // first_token.position = 5;
    // first_token.line = 0;
    // printf("FT_p: %p, FT_S: %ld FT.position: %d FT.line: %d FT.content %s\n\n", first_token, sizeof(first_token), first_token.position, first_token.line, *first_token.content);
    // if ((tokens = calloc(MEM_ALLOC_CONST, sizeof(token))) == NULL) {
    //     error_exit(INTERNAL_ERR);
    // }
    // tokens[0] = first_token;
    // if ((read_result = start_scanner(file_name, tokens)) != SUCCESS) {
    //     error_exit(read_result);
    // }

    return SUCCESS;
}

int start_scanner(char *file_name, Token *tokens)
{
    Token first_token;
    first_token = tokens[0];
    printf("FT_p: %p, FT_S: %ld FT.position: %d FT.line: %d FT.content %s\n\n", first_token, sizeof(first_token), first_token.position, first_token.line, first_token.content);
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