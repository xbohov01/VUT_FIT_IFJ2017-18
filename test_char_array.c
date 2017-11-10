#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef 
enum {as_b, asc_b, declare_b, dim_b, do_b, double_b, else_b, end_b, chr_b, function_b, if_b, input_b, integer_b, length_b, loop_b, print_b, return_b, scope_b, string_b, substr_b, then_b, while_b} \
    all_keys;
const char *keys[22] = {"as", "asc", "declare", "dim", "do", "double", "else", "end", "chr", "function", "if", "input", "integer", "length", "loop", "print", "return", "scope", "string", "substr", "then", "while"};

int keys_len = 22;

void get_token() {
    char *word = "double";
    for(int i=0; i < keys_len; i++) {
        if (strcmp(word, keys[i]) == 0) {
            switch(i) {
                case as_b:
                    printf("I'm a %d element of keys, my value is %s\nNow you can do all you want to do with me in switch case\n", i, keys[i]);
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

    // if (isdigit(word[0]) == true) {
    //     for (int i = 1, i < strlen(word), i++)
    //         int expected = number;
    //         if (!isdigit(word[i]) || word[i] != '.') {
    //             // TODO: Not expect point twice
    //             fprintf(stderr, "Expected %s but got char '%c'\n", key_type_name[number], word[c]);
    //             exit(1);
    //         }
    //     tk->type = number;
    //     printf("\n");
    // }

}

int main() {
    get_token();
}