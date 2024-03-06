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


Value *kihson_parse(Kihson *kihson, char *null_string) {
    kihlexer_load_cstr(&kihson->lexer, null_string);

    // KihsonTokenArray token_array = kihtokens_new();
    // bool success = kihlexer_tokenize(&kihson->lexer, &token_array);
    return kihparser_parse(&kihson->parser, &kihson->lexer);
}

bool is_object(Value *value) {
    return value != NULL && value->type == VALUE_OBJECT;
}

bool is_array(Value *value) {
    return value != NULL && value->type == VALUE_ARRAY;
}

bool is_null(Value *value) {
    return value != NULL && value->type == VALUE_NULL;
}

bool is_number(Value *value) {
    return value != NULL && value->type == VALUE_NUMBER;
}

bool is_string(Value *value) {
    return value != NULL && value->type == VALUE_STRING;
}

bool is_boolean(Value *value) {
    return value != NULL && value->type == VALUE_BOOLEAN;
}

Object get_object(Value *value) {
    return value->data.object;
}

Array get_array(Value *value) {
    return value->data.array;
}

double get_double(Value *value) {
    return value->data.number.double_data;
}

long get_long(Value *value) {
    return value->data.number.long_data;
}

KihsonStringView get_string(Value *value) {
    return value->data.string;
}

char *get_cstring(Value *value) {
    return value->data.string.data;
}


bool get_boolean(Value *value) {
    return value->data.boolean;
}


Value *object_get_value(Value *object_value, char *string) {
    kihson_object_foreach(object_value, json_string, json_value) {
        if (strcmp(json_string->data, string) == 0) {
            return json_value;
        }
    }

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

void kihson_object_foreach_test(Value *value) {
    Value *new_val;
    KihsonStringView string;
    for (ObjectItem *item = value->data.object.item_list; item != NULL; string = item->string, new_val = item->value, item = item->next_item) {

    }
}
