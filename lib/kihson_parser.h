#ifndef KIHSON_PARSER
#define KIHSON_PARSER

#include "kihson_lexer.h"

typedef struct Object {
    int item_count;
    struct ObjectItem *item_list;
} Object;

typedef struct ObjectItem {
    struct ObjectItem *next_item;
    KihsonStringView string;
    struct Value *value;
} ObjectItem;

typedef struct {
    int item_count;
    struct ArrayItem *item_list;
} Array;

typedef struct ArrayItem {
    struct ArrayItem *next_item;
    struct Value *value;
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
    KihsonStringView string;
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
