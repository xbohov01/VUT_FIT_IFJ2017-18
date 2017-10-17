/****** scanner.h ******/



#define N_KEYWORDS 35
#define N_OPERATOR_CHARS 8
#define N_OPERATORS 12
#define N_DELIMITERS 7

//enumerator for token types
//TODO Martin fill out token types
typedef enum {

	integer
} T_token_type;

//structure for token
struct {
	T_token_type type;
	int value_int;
	double value_double;
	char *value_string;
} token;

//buffer for identifiers
string *buffer;

//function declarations
int start_scanner(char *filename);
int stop_scanner();
int get_token();
