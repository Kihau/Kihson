#ifndef KIHSON
#define KIHSON

#include "kihson_string.h"
#include "kihson_lexer.h"
#include "kihson_parser.h"

#define KIHAPI ""

#define KIHSON_MALLOC  malloc
#define KIHSON_CALLOC  calloc
#define KIHSON_REALLOC realloc
#define KIHSON_FREE    free

typedef struct {
    KihsonString string;
    KihsonLexer  lexer;
    KihsonParser parser;

    // char *json_string;
    // StringView *string;
    // KihsonTokens *tokens;
    // KihsonTree *tokens;
} Kihson;


// TODO: All should return result after the operation.


/// Allocate new kihson structure.
Kihson kihson_new(void);

/// Clear fields of the kihson structure without deallocating anything.
void kihson_clear(Kihson *kihson);

/// Dealocate fields of the kihson structure.
void kihson_free(Kihson *kihson);


//
// Json Reading
//

/// Tokenize and parse json string.
void kihson_parse(Kihson *kihson, char *null_string);

/// Tokenize and parse json string.
void kihson_load_with(Kihson *kihson, char *string, long length);


//
// Json Writing
//

/// Generate new json string based on the structure.
char* kihson_generate(Kihson *kihson);

/// Generate new json string based on the structure.
char* kihson_generate_pretty(Kihson *kihson, int indent_size);

#endif // KIHSON
