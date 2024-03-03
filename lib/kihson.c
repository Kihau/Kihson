#include "kihson.h"

Kihson kihson_new(void) {
    KihsonString string = kihstring_with(1024);
    KihsonLexer lexer   = kihlexer_new();

    Kihson kihson = {
        .string = string,
        .lexer  = lexer,
    };

    return kihson;
}

void kihson_clear(Kihson *kihson) {
    kihstring_clear(&kihson->string);
    kihlexer_clear(&kihson->lexer);
}

void kihson_free(Kihson *kihson) {
    kihstring_free(&kihson->string);
}


void kihson_parse(Kihson *kihson, char *null_string) {
    kihlexer_load_cstr(&kihson->lexer, null_string);

    // KihsonTokenArray token_array = kihtokens_new();
    // bool success = kihlexer_tokenize(&kihson->lexer, &token_array);
    kihparser_parse(&kihson->parser, &kihson->lexer);
}


/// Generate new json string based on the structure.
char* kihson_generate(Kihson *kihson) {
    return "TODO";
}

/// Generate new json string based on the structure.
char* kihson_generate_pretty(Kihson *kihson, int indent_size) {
    return "TODO";
}
