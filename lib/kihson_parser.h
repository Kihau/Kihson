#ifndef KIHSON_PARSER
#define KIHSON_PARSER

#include "kihson_lexer.h"

typedef struct Object {
    int item_count;
    long item_list_index;
} Object;

typedef struct ObjectItem {
    long next_item_index;
    long string_index;
    long value_index;
} ObjectItem;

typedef struct {
    int item_count;
    long item_list_index;
} Array;

typedef struct ArrayItem {
    long next_item_index;
    long value_index;
} ArrayItem;

typedef enum {
    VALUE_OBJECT,
    VALUE_ARRAY,
    VALUE_STRING,
    VALUE_NUMBER,
    VALUE_BOOLEAN,
    VALUE_NULL,
} ValueType;

typedef union {
    Object object;
    Array  array;
    Number number;
    bool   boolean;
    long   string_index;
} ValueData;

typedef struct Value {
    ValueType type;
    ValueData data;
} Value;

typedef enum {
    ITEM_OBJECT,
    ITEM_ARRAY,
    ITEM_VALUE,
} ItemType;

typedef union {
    ObjectItem object;
    ArrayItem  array;
    Value      value;
} ItemData;

typedef struct Item {
    ItemType type;
    ItemData data;
} Item;

typedef struct {
    ValueType type;
    ValueData data;
    Item *items;
    char *strings;
} KihsonValue;

typedef struct {
    Item *items;
    long items_capacity;
    long items_length;

    // Json files consist from one and onyl value.
    KihsonValue value_base;
} KihsonParser;

KihsonParser kihparser_new(void);
void kihparser_clear(KihsonParser *parser);
void kihparser_free(KihsonParser *parser);
KihsonValue *kihparser_parse(KihsonParser *parser, KihsonLexer *lexer);

#endif // KIHSON_PARSER
