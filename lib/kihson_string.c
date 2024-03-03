#include "kihson_string.h"

KihsonString kihstring_new(void) {
    const long capacity = 4;
    return kihstring_with(capacity);
}

KihsonString kihstring_with(long capacity) {
    char *data = malloc(capacity);
    KihsonString new_string = {
        .data = data,
        .length = 0,
        .capacity = capacity,
    };

    return new_string;
}

void kihstring_push(KihsonString *string, char byte) {
    if (string->length == string->capacity) {
        string->capacity *= 2;
        string->data = realloc(string->data, string->capacity);
    }

    string->data[string->length] = byte;
    string->length += 1;
}

void kihstring_clear(KihsonString *string) {
    string->length = 0;
}

void kihstring_free(KihsonString *string) {
    free(string->data);
    string->data = NULL;
    string->length = 0;
    string->capacity = 0;
}

KihsonStringView kihstring_view_new(char *string) {
    long length = strlen(string);
    KihsonStringView new_string_view = {
        .data = string,
        .length = length,
    };

    return new_string_view;
}

KihsonStringView kihstring_view_with(char *string, long length) {
    KihsonStringView new_string_view = {
        .data = string,
        .length = length,
    };

    return new_string_view;
}
