/****** scanner.h ******/



#define N_KEYWORDS 35
#define N_OPERATOR_CHARS 8
#define N_OPERATORS 12
#define N_DELIMITERS 7
#define IFJ17_STANDARD_LINE_LEN 60
#define WORD_MAX_LEN 30
#define MEM_ALLOC_CONST 10

typedef enum {
    integer,
    string
} tokenType;

typedef struct {
    int id;
    int line;
    char word[WORD_MAX_LEN];
} token;

//buffer for identifiers
char *buffer;

//function declarations
// Read file until EOF and return 0 on success
int start_scanner(char *file_name, token *lines);
int stop_scanner();
int get_token();
void error_exit(int value);
