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


#define object_item_index(kihson, current) kihson->parser.values[current].data.object_item.next_index
#define get_object_item(kihson, index)    &kihson->parser.values[index].data.object_item

#define array_item_index(kihson, current) kihson->parser.values[current].data.array_item.next_index
#define get_array_item(kihson, index)    &kihson->parser.values[index].data.array_item

#define item_string(kihson, item)  &kihson->lexer.all_json_strings.data[item->string_index]
#define item_value(kihson, item)   &kihson->parser.values[item->value_index]


#define kihson_object_foreach(kihson, current_value, _string, _value)\
    for (long _index = current_value->data.object.list_index; _index != -1; _index = object_item_index(kihson, _index))\
        for (ObjectItem *_item = get_object_item(kihson, _index); _item != NULL; _item = NULL)\
            for (char *_string = item_string(kihson, _item); _string != NULL; _string = NULL)\
                for (Value *_value = item_value(kihson, _item); _value != NULL; _value = NULL)

#define kihson_object_foreach_item(kihson, current_value, _item)\
    for (long _index = current_value->data.object.list_index; _index != -1; _index = object_item_index(kihson, _index))\
        for (ObjectItem *_item = get_object_item(kihson, _index); _item != NULL; _item = NULL)

#define kihson_object_foreach_index(_kihson, _value, _index)\
    for (long _index = get_object(_value).list_index; _index != -1; _index = object_item_index(_kihson, _index))

#define kihson_array_foreach(kihson, current_value, _value)\
    for (long _index = current_value->data.array.list_index; _index != -1; _index = array_item_index(kihson, _index))\
        for (ArrayItem *_item = get_array_item(kihson, _index); _item != NULL; _item = NULL)\
            for (Value *_value = item_value(kihson, _item); _value != NULL; _value = NULL)

typedef struct {
    KihsonLexer  lexer;
    KihsonParser parser;
    Value *json_head;
} Kihson;


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
Value *kihson_parse(Kihson *kihson, char *null_string);

/// Tokenize and parse json string.
void kihson_load_with(Kihson *kihson, char *string, long length);


Value *object_get_value(Kihson *kihson, Value *value, char *string);
Value *try_get_value(Kihson *kihson, ObjectItem *item, char *string);


bool is_object(Value *value);
bool is_array(Value *value);
bool is_null(Value *value);
bool is_number(Value *value);
bool is_string(Value *value);
bool is_boolean(Value *value);

bool get_boolean(Value *value);
Object get_object(Value *value);
Array get_array(Value *value);
double get_double(Value *value);
long get_long(Value *value);
char *get_string(Kihson *kihson, Value *value);

//
// Json Writing
//

/// Generate new json string based on the structure.
char *kihson_generate(Kihson *kihson);

/// Generate new json string based on the structure.
char *kihson_generate_pretty(Kihson *kihson, int indent_size);

#endif // KIHSON
