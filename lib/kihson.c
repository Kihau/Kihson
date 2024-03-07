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

char *get_string(Kihson *kihson, Value *value) {
    return kihson->lexer.all_json_strings.data + value->data.string_index;
}

bool get_boolean(Value *value) {
    return value->data.boolean;
}

Value *object_get_value(Kihson *kihson, Value *current_value, char *string) {
    kihson_object_foreach(kihson, current_value, key, value) {
        if (strcmp(key, string) == 0) {
            return value;
        }
    }

    return NULL;
}

Value *try_get_value(Kihson *kihson, ObjectItem *item, char *string) {
    char *key = item_string(kihson, item);
    if (strcmp(key, string) == 0) {
        return item_value(kihson, item);
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
