#ifndef KIHSON_LEXER
#define KIHSON_LEXER

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "kihson_string.h"

typedef enum {
    LEX_RESULT_SUCCESS  = 0,
    LEX_RESULT_END      = 1,
    LEX_RESULT_FAILURE  = 2,
} TokenLexResult;

typedef enum {
    TOKEN_SQUARE_BRACKET_LEFT,
    TOKEN_SQUARE_BRACKET_RIGHT,

    TOKEN_CURLY_BRACKET_LEFT,
    TOKEN_CURLY_BRACKET_RIGHT,

    TOKEN_COLON,
    TOKEN_COMMA,

    TOKEN_STRING,
    TOKEN_NUMBER,

    TOKEN_BOOLEAN,
    TOKEN_NULL,
    
    // Failed to tokenize.
    TOKEN_ERROR,

    // Tokenization finished.
    TOKEN_END,
} TokenType;

typedef struct {
    double double_data;
    long   long_data;
} Number;

typedef union {
    long string_index;
    Number number_data;
    bool   boolean_data;
} TokenData;

typedef struct {
    int line_offset;
    int line_number;

    TokenType token_type;
    TokenData token_data;
} Token;

typedef struct {
    KihsonStringView json_string;
    KihsonString all_json_strings;

    Token token;

    int byte_offset;
    int line_offset;
    int line_number;
} KihsonLexer;

typedef struct {
    Token *tokens;
    int tokens_capacity;
    int tokens_length;
} KihsonTokenArray;


KihsonLexer kihlexer_new(void);
void kihlexer_load(KihsonLexer *lexer, KihsonStringView json_string);
void kihlexer_load_cstr(KihsonLexer *lexer, char *json_cstring);
void kihlexer_advance_token(KihsonLexer *lexer);
void kihlexer_clear(KihsonLexer *lexer);
void kihlexer_free(KihsonLexer *lexer);
bool kihlexer_tokenize(KihsonLexer *lexer, KihsonTokenArray *token_array);
bool kihlexer_tokenize_cstr(KihsonLexer *lexer, KihsonTokenArray *token_array);

KihsonTokenArray kihtokens_new(void);
void kihtokens_push_token(KihsonTokenArray *token_array, Token token);
void kihtokens_clear(KihsonTokenArray *token_array);
void kihtokens_free(KihsonTokenArray *token_array);

#endif // KIHSON_LEXER
