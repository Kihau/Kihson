#include "kihson.h"

Kihson kihson_new(void) {
    KihsonLexer lexer   = kihlexer_new();
    KihsonParser parser = kihparser_new();

    Kihson kihson = {
        .lexer     = lexer,
        .parser    = parser,
        .json_head = NULL,
    };

    return kihson;
}

void kihson_clear(Kihson *kihson) {
    kihlexer_clear(&kihson->lexer);
    kihparser_clear(&kihson->parser);
    kihson->json_head = NULL;
}

void kihson_free(Kihson *kihson) {
    kihlexer_free(&kihson->lexer);
    kihparser_free(&kihson->parser);
    kihson->json_head = NULL;
}


KihsonValue *kihson_parse(Kihson *kihson, char *null_string) {
    kihlexer_load_cstr(&kihson->lexer, null_string);

    // KihsonTokenArray token_array = kihtokens_new();
    // bool success = kihlexer_tokenize(&kihson->lexer, &token_array);
    return kihparser_parse(&kihson->parser, &kihson->lexer);
}

bool is_object(KihsonValue *value) {
    return value != NULL && value->type == VALUE_OBJECT;
}

bool is_array(KihsonValue *value) {
    return value != NULL && value->type == VALUE_ARRAY;
}

bool is_null(KihsonValue *value) {
    return value != NULL && value->type == VALUE_NULL;
}

bool is_number(KihsonValue *value) {
    return value != NULL && value->type == VALUE_NUMBER;
}

bool is_string(KihsonValue *value) {
    return value != NULL && value->type == VALUE_STRING;
}

bool is_boolean(KihsonValue *value) {
    return value != NULL && value->type == VALUE_BOOLEAN;
}

Object get_object(KihsonValue *value) {
    return value->data.object;
}

Array get_array(KihsonValue *value) {
    return value->data.array;
}

double get_double(KihsonValue *value) {
    return value->data.number.double_data;
}

long get_long(KihsonValue *value) {
    return value->data.number.long_data;
}

char *get_string(KihsonValue *value) {
    return &value->strings[value->data.string_index];
}

bool get_boolean(KihsonValue *value) {
    return value->data.boolean;
}

#define obj_item(_kv, _kidx) &_kv->items[_kidx].data.object
#define get_str(_kv, _kitem) &_kv->strings[_kitem->string_index]
#define get_val(_kv, _kitem) &_kv->items[_kitem->value_index].data.value

KihsonValue *object_get_value(KihsonValue *current_value, char *string) {
    long item_index = current_value->data.object.item_list_index;
    // ObjectItem *_item = &current_value->items[item_index].data.object;
    // char *_string = &current_value->strings[_item->string_index];
    // Value *_value = &current_value->items[_item->value_index].data.value;
    // for (; item_index != -1; item_index = _item->next_item_index)
    //     for (ObjectItem *_item = obj_item(current_value, item_index); _item != NULL; _item = NULL)
    //         for (char *_string = get_str(current_value, _item); _string != NULL; _string = NULL)
    //             for (Value *_value = get_val(current_value, _item); _value != NULL; _value = NULL) {
    //
    //             }


    // kihson_object_foreach(object_value, json_string, json_value) {
    //     if (strcmp(json_string, string) == 0) {
    //         return json_value;
    //     }
    // }

    return NULL;
}

// KihsonValue *object_get_value(KihsonValue *object_value, char *string) {
//     kihson_object_foreach(object_value, json_string, json_value) {
//         if (strcmp(json_string, string) == 0) {
//             return json_value;
//         }
//     }
//
//     return NULL;
// }

KihsonValue *try_get_value(ObjectItem *item, char *string) {
    return NULL;
}

/// Generate new json string based on the structure.
char* kihson_generate(Kihson *kihson) {
    return "TODO";
}

/// Generate new json string based on the structure.
char* kihson_generate_pretty(Kihson *kihson, int indent_size) {
    return "TODO";
}
