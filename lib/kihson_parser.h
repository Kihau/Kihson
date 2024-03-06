#ifndef KIHSON_PARSER
#define KIHSON_PARSER

#include "kihson_lexer.h"

// TODO: !!!!!!!!!!!!!!!
// HUGE MEMORY BUG, ALL POINTER GET INVALIDATED AFTER REALLOC!!!
// STORE LINKS AS INDEXES INSTEAD!!!

typedef struct Object {
    int item_count;

    // struct ObjectItem *item_list;
    long item_list_index;
} Object;

typedef struct ObjectItem {
    // struct ObjectItem *next_item;
    long next_item_index;

    // KihsonStringView string;
    long string_index;

    // struct Value *value;
    long value_index;
} ObjectItem;

typedef struct {
    int item_count;

    // struct ArrayItem *item_list;
    long item_list_index;
} Array;

typedef struct ArrayItem {
    // struct ArrayItem *next_item;
    long next_item_index;

    // struct Value *value;
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
    Object           object;
    Array            array;
    Number           number;
    bool             boolean;

    // KihsonStringView string;
    long string_index;
} ValueData;

typedef struct Value {
    ValueType type;
    ValueData data;
} Value;

// Return this after parsing and make the Value struct internal to the impl?
// typedef struct Value {
//     ValueType type;
//     ValueData data;
//     Item *items;
// } KihsonValue;

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
    Item *items;
    long items_capacity;
    long items_length;

    // Json files consist from one and onyl value.
    // Value *value_base;
} KihsonParser;

KihsonParser kihparser_new(void);
void kihparser_clear(KihsonParser *parser);
void kihparser_free(KihsonParser *parser);
Value *kihparser_parse(KihsonParser *parser, KihsonLexer *lexer);

#endif // KIHSON_PARSER
