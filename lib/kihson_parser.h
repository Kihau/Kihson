#ifndef KIHSON_PARSER
#define KIHSON_PARSER

#include "kihson_lexer.h"

typedef struct Object {
    int item_count;
    long list_index;
} Object;

typedef struct ObjectItem {
    long next_index;
    long string_index;
    long value_index;
} ObjectItem;

typedef struct {
    int item_count;
    long list_index;
} Array;

typedef struct ArrayItem {
    long next_index;
    long value_index;
} ArrayItem;

typedef enum {
    VALUE_OBJECT_ITEM,
    VALUE_ARRAY_ITEM,
    VALUE_OBJECT,
    VALUE_ARRAY,
    VALUE_STRING,
    VALUE_NUMBER,
    VALUE_BOOLEAN,
    VALUE_NULL,
} ValueType;

typedef union {
    ObjectItem object_item;
    ArrayItem  array_item;
    Object     object;
    Array      array;
    Number     number;
    bool       boolean;
    long       string_index;
} ValueData;

typedef struct Value {
    ValueType type;
    ValueData data;
} Value;

typedef struct {
    Value *values;
    long capacity;
    long length;

    // Json files consist from one top level value.
    Value main_value;
} KihsonParser;

KihsonParser kihparser_new(void);
void kihparser_clear(KihsonParser *parser);
void kihparser_free(KihsonParser *parser);
Value *kihparser_parse(KihsonParser *parser, KihsonLexer *lexer);

#endif // KIHSON_PARSER
