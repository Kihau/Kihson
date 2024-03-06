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

// #define kihson_object_foreach_item(current_value, _item)\
//     for (ObjectItem *_item = current_value->data.object.item_list; _item != NULL; _item = _item->next_item)
//
// #define kihson_object_foreach(current_value, _string, _value)\
//     for (ObjectItem *_item = &current_value->items[current_value->data.object.item_list_index].data.object; _item != NULL; _item = _item->next_item)\
//         for (char *_string = &current_value->strings[_item->string_index]; _string != NULL; _string = NULL)\
//             for (Value *_value = &current_value->items[_item->value_index].data.value; _value != NULL; _value = NULL)
//
// #define kihson_array_foreach(current_value, _value)\
//     for (ArrayItem *_item = current_value->data.array.item_list; _item != NULL; _item = _item->next_item)\
//         for (Value *_value = _item->value; _value != NULL; _value = NULL)

typedef struct {
    KihsonLexer  lexer;
    KihsonParser parser;
    Value *json_head;
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

/// Tokenize and parse json string. Result true on success and false on failure.
KihsonValue *kihson_parse(Kihson *kihson, char *null_string);

/// Tokenize and parse json string.
void kihson_load_with(Kihson *kihson, char *string, long length);

KihsonValue *object_get_value(KihsonValue *value, char *string);
KihsonValue *try_get_value(ObjectItem *item, char *string);


bool is_object(KihsonValue *value);
bool is_array(KihsonValue *value);
bool is_null(KihsonValue *value);
bool is_number(KihsonValue *value);
bool is_string(KihsonValue *value);
bool is_boolean(KihsonValue *value);

bool get_boolean(KihsonValue *value);
Object get_object(KihsonValue *value);
Array get_array(KihsonValue *value);
double get_double(KihsonValue *value);
long get_long(KihsonValue *value);
char *get_string(KihsonValue *value);

//
// Json Writing
//

/// Generate new json string based on the structure.
char *kihson_generate(Kihson *kihson);

/// Generate new json string based on the structure.
char *kihson_generate_pretty(Kihson *kihson, int indent_size);

#endif // KIHSON
