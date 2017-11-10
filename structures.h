typedef struct token {
    int position;
    int line;
    int capacity;
    int length;
    char *content;
} Token;

typedef Token Data;

typedef struct item {
    struct item *next;
    Data *data;
    struct item *prev;
} Item;

typedef struct list {
    Item *active;
    Item *first;
    Item *last;
} List;