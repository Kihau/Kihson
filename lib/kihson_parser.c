#include "kihson_parser.h"

static void print_error(KihsonLexer *lexer, char *expected);
static bool token_is_value(Token *token);

static long parse_value(KihsonParser *parser, KihsonLexer *lexer);
static long parse_object(KihsonParser *parser, KihsonLexer *lexer);
static long parse_array(KihsonParser *parser, KihsonLexer *lexer);

static long allocate_value(KihsonParser *parser);
static long add_object(KihsonParser *parser);
static long add_object_item(KihsonParser *parser);
static long add_array(KihsonParser *parser);
static long add_array_item(KihsonParser *parser);

static Value      *get_value(KihsonParser *parser, long index);
static Object     *get_object(KihsonParser *parser, long index);
static ObjectItem *get_object_item(KihsonParser *parser, long index);
static Array      *get_array(KihsonParser *parser, long index);
static ArrayItem  *get_array_item(KihsonParser *parser, long index);

static char *get_token_name(TokenType type) {
    switch (type) {
        case TOKEN_SQUARE_BRACKET_LEFT:   return "SQUARE BRACKET LEFT";
        case TOKEN_SQUARE_BRACKET_RIGHT:  return "SQUARE BRACKET RIGHT";
        case TOKEN_CURLY_BRACKET_LEFT:    return "CURLY BRACKET LEFT";
        case TOKEN_CURLY_BRACKET_RIGHT:   return "CURLY BRACKET RIGHT";
        case TOKEN_COLON:                 return "COLON";
        case TOKEN_COMMA:                 return "COMMA";
        case TOKEN_STRING:                return "STRING";
        case TOKEN_NUMBER:                return "NUMBER";
        case TOKEN_BOOLEAN:               return "BOOLEAN";
        case TOKEN_NULL:                  return "NULL";
        case TOKEN_ERROR:                 return "ERROR";
        case TOKEN_END:                   return "END";
        default:                          return "UNREACHABLE";
    }
}

static void print_error(KihsonLexer *lexer, char *expected) {
    TokenType type = lexer->token.token_type;
    switch (type) {
        case TOKEN_END: {
            printf("ERROR: Expected \"%s\" token but the token stream ended.\n", expected);
        } break;

        case TOKEN_ERROR: {
            printf("ERROR: Parsing failed because of a tokenizetion error.\n");
        } break;

        default: {
            char *token_name = get_token_name(type);
            printf("ERROR: Expected token \"%s\" token but got token \"%s\".\n", expected, token_name);
        } break;
    }
}

static bool token_is_value(Token *token) {
    TokenType type = token->token_type;
    switch (type) {
        case TOKEN_CURLY_BRACKET_LEFT:
        case TOKEN_SQUARE_BRACKET_LEFT:
        case TOKEN_NUMBER:
        case TOKEN_STRING:
        case TOKEN_BOOLEAN:
            return true;

        default:
            return false;
    }
}

static long parse_object(KihsonParser *parser, KihsonLexer *lexer) {
    long object_index = add_object(parser);

    kihlexer_advance_token(lexer);
    if (lexer->token.token_type == TOKEN_CURLY_BRACKET_RIGHT) {
        return object_index;
    }

    long item_index = add_object_item(parser);
    get_object(parser, object_index)->list_index = item_index; 

    while (true) {
        get_object(parser, object_index)->item_count += 1;

        if (lexer->token.token_type != TOKEN_STRING) {
            print_error(lexer, "STRING");
            return -1;
        }

        long string_index = lexer->token.token_data.string_index;
        get_object_item(parser, item_index)->string_index = string_index;

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type != TOKEN_COLON) {
            print_error(lexer, "COLON");
            return -1;
        }

        kihlexer_advance_token(lexer);
        long value_index = parse_value(parser, lexer);
        if (value_index == -1) {
            return -1;
        }

        get_object_item(parser, item_index)->value_index = value_index;

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type == TOKEN_CURLY_BRACKET_RIGHT) {
            return object_index;
        } else if (lexer->token.token_type != TOKEN_COMMA) {
            print_error(lexer, "COMMA");
            return -1;
        }

        long next_index = add_object_item(parser);
        get_object_item(parser, item_index)->next_index = next_index;
        item_index = next_index;

        kihlexer_advance_token(lexer);
    }
}

static long parse_array(KihsonParser *parser, KihsonLexer *lexer) {
    long array_index = add_array(parser);

    kihlexer_advance_token(lexer);
    if (lexer->token.token_type == TOKEN_SQUARE_BRACKET_RIGHT) {
        return array_index;
    }

    long item_index = add_array_item(parser);
    get_array(parser, array_index)->list_index = item_index;

    while (true) {
        get_array(parser, array_index)->item_count += 1;

        long value_index = parse_value(parser, lexer);
        if (value_index == -1) {
            return -1;
        }

        get_array_item(parser, item_index)->value_index = value_index;

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type == TOKEN_SQUARE_BRACKET_RIGHT) {
            return array_index;
        } else if (lexer->token.token_type != TOKEN_COMMA) {
            print_error(lexer, "COMMA");
            return -1;
        }

        long new_item = add_array_item(parser);
        get_array_item(parser, item_index)->next_index = new_item;
        item_index = new_item;

        kihlexer_advance_token(lexer);
    }
}

static long parse_value(KihsonParser *parser, KihsonLexer *lexer) {
    long value_index = -1;

    switch (lexer->token.token_type) {
        case TOKEN_CURLY_BRACKET_LEFT: {
            value_index = parse_object(parser, lexer);
        } break;

        case TOKEN_SQUARE_BRACKET_LEFT: {
            value_index = parse_array(parser, lexer);
        } break;

        case TOKEN_NUMBER: {
            value_index = allocate_value(parser);
            Value *value = get_value(parser, value_index);
            value->type = VALUE_NUMBER;
            value->data.number = lexer->token.token_data.number_data;
        } break;

        case TOKEN_STRING: {
            value_index = allocate_value(parser);
            Value *value = get_value(parser, value_index);
            value->type = VALUE_STRING;
            value->data.string_index = lexer->token.token_data.string_index;
        } break;

        case TOKEN_BOOLEAN: {
            value_index = allocate_value(parser);
            Value *value = get_value(parser, value_index);
            value->type = VALUE_BOOLEAN;
            value->data.boolean = lexer->token.token_data.boolean_data;
        } break;

        case TOKEN_NULL: {
            value_index = allocate_value(parser);
            Value *value = get_value(parser, value_index);
            value->type = VALUE_NULL;
        } break;

        default: {
            print_error(lexer, "VALUE");
            return -1;
        } break;
    }

    return value_index;
}

static long allocate_value(KihsonParser *parser) {
    if (parser->length == parser->capacity) {
        parser->capacity *= 2;
        parser->values = realloc(parser->values, parser->capacity * sizeof(Value));
    }

    long item_index = parser->length;
    parser->length += 1;
    return item_index;
}

static long add_object(KihsonParser *parser) {
    long index = allocate_value(parser);
    parser->values[index] = (Value) {
        .type = VALUE_OBJECT,
        .data.object = (Object) {
            .item_count = 0,
            .list_index = -1,
        },
    };

    return index;
}

static long add_object_item(KihsonParser *parser) {
    long index = allocate_value(parser);
    parser->values[index] = (Value) {
        .type = VALUE_OBJECT_ITEM,
        .data.object_item = (ObjectItem) {
            .string_index = -1,
            .value_index  = -1,
            .next_index   = -1,
        },
    };

    return index;
}

static long add_array(KihsonParser *parser) {
    long index = allocate_value(parser);
    parser->values[index] = (Value) {
        .type = VALUE_ARRAY,
        .data.array = (Array) {
            .item_count = 0,
            .list_index = -1,
        },
    };

    return index;
}

static long add_array_item(KihsonParser *parser) {
    long index = allocate_value(parser);
    parser->values[index] = (Value) {
        .type = VALUE_ARRAY_ITEM,
        .data.array_item = (ArrayItem) {
            .value_index  = -1,
            .next_index   = -1,
        },
    };

    return index;
}

static Value *get_value(KihsonParser *parser, long index) {
    return &parser->values[index];
}

static Object *get_object(KihsonParser *parser, long index) {
    return &parser->values[index].data.object;
}

static ObjectItem *get_object_item(KihsonParser *parser, long index) {
    return &parser->values[index].data.object_item;
}

static Array *get_array(KihsonParser *parser, long index) {
    return &parser->values[index].data.array;
}

static ArrayItem *get_array_item(KihsonParser *parser, long index) {
    return &parser->values[index].data.array_item;
}

KihsonParser kihparser_new(void) {
    const long capacity = 1024;
    Value *values = malloc(capacity * sizeof(Value));
    
    KihsonParser parser = {
        .values   = values,
        .capacity = capacity,
        .length   = 0,
    };

    return parser;
}

void kihparser_clear(KihsonParser *parser) {
    parser->length = 0;
}

void kihparser_free(KihsonParser *parser) {
    free(parser->values);
    parser->values = NULL,
    parser->length = 0;
    parser->capacity = 0;
}

Value *kihparser_parse(KihsonParser *parser, KihsonLexer *lexer) {
    if (parser->values == NULL) {
        return NULL;
    }

    kihlexer_advance_token(lexer);
    long value_index = parse_value(parser, lexer);

    kihlexer_advance_token(lexer);
    if (lexer->token.token_type != TOKEN_END) {
        return NULL;
    } 

    if (value_index == -1) {
        return NULL;
    }

    Value json_head = parser->values[value_index];
    parser->main_value = json_head;
    return &parser->main_value;
}
