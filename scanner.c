#include "scanner.h"


int main(int argc, char* argv[])
{
    // Item *item1;
    // token_list_init(&my_new_list);
    // printf("Success 1\n");
    // item1 = token_init(3, 4);
    // printf("%d\n", item1->data->capacity);
    // printf("Success 2\n");
    // insert_first(&my_new_list, item1);
    // printf("Success 3\n");
    // if (item1 == NULL) {
    //     // TODO: info to stderr
    //     data_free(&my_new_list);
    //     return INTERNAL_ERR;
    // }
    // feed_word(item1, "Some neat text");
    // printf("Success 4\n");
    // printf("item1->data->content: %s, item1->data->content == my_new_list->first->data->content: %d \n", item1->data->content, strcmp(item1->data->content, my_new_list.first->data->content));
    // data_free(&my_new_list);
    // printf("Success free\n");

    if (argc != 2) {
        perror("File is not specified");
        return INTERNAL_ERR;
    }

    char *file_name = argv[1];

    List *tokens;
    int scanner_result;
    if ((tokens = start_scanner(file_name)) == NULL){
        return INTERNAL_ERR;
    }
    printf("scanner_result: %d\n", scanner_result);
    if (scanner_result == 0) {
        data_free(tokens);
    }
    return SUCCESS;
}

List  *start_scanner(char *file_name)
{
    FILE *input_file;
    List *tokens_l;
    Item *token;
    int result;
    int line_count, token_count;
    char buffer[1] = "";
    bool is_space = false;
    bool in_quotes = false;

    line_count = token_count = 0;

    if ((input_file = fopen(file_name, "r")) == NULL) {
        return NULL;
    }

    // List initiation
    tokens_l = token_list_init();
    if (tokens_l == NULL) {
        fprintf(stderr, "List init failure\n");
        data_free(tokens_l);
        return NULL;
    }
    // First element initiation
    token = token_init(token_count, line_count);
    if (token == NULL) {
        fprintf(stderr, "Token init failure\n");
        data_free(tokens_l);
        return NULL;
    }
    // Inserting first in list
    insert_first(tokens_l, token);
    if (result != 0) {
        data_free(tokens_l);
        return NULL;
    }
    while ((buffer[1] = fgetc(input_file)) != EOF) {
        if (buffer[1] == '\n') {
            token = token_init(token_count, line_count);
            insert_last(tokens_l, token);
            token_count = 0;
            line_count++;
        }
        else if (isspace(buffer[1]) && in_quotes == false) {
            if (is_space == false) {
                is_space = true;
                token_count++;
                // New element initiation
                token = token_init(token_count, line_count);
                insert_last(tokens_l, token);
                continue;
            }
        }
        // switch(buffer[0]) {
        //     case '\n':
        //         printf("Line: %d  Tokens: %d  Chars: %d\n", line_count, token_count, char_count);
        //         char_count = 0;
        //         token_count = 0;
        //         line_count++;

        //         break;
        //         // TODO: add token to struct
        //     case ' ': 
        //         if (is_space == false) {
        //             is_space = true;
        //             token_count++;
        //             char_count = 0;
        //              // = (char *)malloc(strlen(word)* sizeof(char));
        //         }
        //         // Intentionally gave no break for default case execution
        //     default:
        //         // line[char_count] = tolower(buffer[0]);
        //         char_count++;
        //         if (is_space == true) { is_space = false; }
        //         break;
        // }
        // Moving for history
        printf("%s\n", buffer);
        buffer[0] = buffer[1];
    }
    if (strcmp(buffer, "") == 0) {
        printf("File has zero length\n");
        data_free(tokens_l);
    }
    fclose(input_file);
    return tokens_l;
}
