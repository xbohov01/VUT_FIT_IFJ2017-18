/****** scanner.h ******/



#define N_KEYWORDS 35
#define N_OPERATOR_CHARS 8
#define N_OPERATORS 12
#define N_DELIMITERS 7

typedef enum{

	integer
} tokenType;

struct{

} token;

//buffer for identifiers
string *buffer;

//function declarations
int start_scanner(char *filename);
int stop_scanner();
int get_token();
