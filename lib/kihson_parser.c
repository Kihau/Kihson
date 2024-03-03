#include "kihson_parser.h"

static void print_error(KihsonLexer *lexer, char *expected);
static bool token_is_value(Token *token);

static Value *parse_value(KihsonParser *parser, KihsonLexer *lexer);
static Value *parse_object(KihsonParser *parser, KihsonLexer *lexer);
static Value *parse_array(KihsonParser *parser, KihsonLexer *lexer);

static void push_item(KihsonParser *parser, Item item);
static Item *allocate_item(KihsonParser *parser);
static Value *allocate_value_item(KihsonParser *parser);
static ObjectItem *allocate_object_item(KihsonParser *parser);
static ArrayItem *allocate_array_item(KihsonParser *parser);

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

static Value *parse_object(KihsonParser *parser, KihsonLexer *lexer) {
    Value *object_begin = allocate_value_item(parser);
    object_begin->type = VALUE_OBJECT;
    object_begin->data.object = (Object) {
        .item_count = 0,
        .item_list = NULL,
    };

    kihlexer_advance_token(lexer);
    if (lexer->token.token_type == TOKEN_CURLY_BRACKET_RIGHT) {
        return object_begin;
    }

    ObjectItem *object_item = allocate_object_item(parser);
    object_begin->data.object.item_list = object_item;

    while (true) {
        object_begin->data.object.item_count += 1;

        if (lexer->token.token_type != TOKEN_STRING) {
            print_error(lexer, "STRING");
            return NULL;
        }

        object_item->string = lexer->token.token_data.string_data;

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type != TOKEN_COLON) {
            print_error(lexer, "COLON");
            return NULL;
        }

        kihlexer_advance_token(lexer);
        Value *value = parse_value(parser, lexer);
        if (value == NULL) {
            return NULL;
        }

        object_item->value = value;

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type == TOKEN_CURLY_BRACKET_RIGHT) {
            return object_begin;
        } else if (lexer->token.token_type != TOKEN_COMMA) {
            print_error(lexer, "COMMA");
            return NULL;
        }

        ObjectItem *new_object_item = allocate_object_item(parser);
        object_item->next_item = new_object_item;
        object_item = new_object_item;

        kihlexer_advance_token(lexer);
    }
}

static Value *parse_array(KihsonParser *parser, KihsonLexer *lexer) {
    Value *array_begin = allocate_value_item(parser);
    array_begin->type = VALUE_ARRAY;
    array_begin->data.array = (Array) {
        .item_count = 0,
        .item_list = NULL,
    };

    kihlexer_advance_token(lexer);
    if (lexer->token.token_type == TOKEN_SQUARE_BRACKET_RIGHT) {
        return array_begin;
    }

    ArrayItem *array_item = allocate_array_item(parser);
    array_begin->data.array.item_list = array_item;

    while (true) {
        array_begin->data.array.item_count += 1;

        Value *value = parse_value(parser, lexer);
        if (value == NULL) {
            return NULL;
        }

        array_item->value = value;

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type == TOKEN_SQUARE_BRACKET_RIGHT) {
            return array_begin;
        } else if (lexer->token.token_type != TOKEN_COMMA) {
            print_error(lexer, "COMMA");
            return NULL;
        }

        ArrayItem *new_array_item = allocate_array_item(parser);
        array_item->next_item = new_array_item;
        array_item = new_array_item;

        kihlexer_advance_token(lexer);
    }
}

static Value *parse_value(KihsonParser *parser, KihsonLexer *lexer) {
    Value *value = NULL;

    switch (lexer->token.token_type) {
        case TOKEN_CURLY_BRACKET_LEFT: {
            value = parse_object(parser, lexer);
        } break;

        case TOKEN_SQUARE_BRACKET_LEFT: {
            value = parse_array(parser, lexer);
        } break;

        case TOKEN_NUMBER: {
            value = allocate_value_item(parser);
            value->type = VALUE_NUMBER;
            value->data.number = lexer->token.token_data.number_data;
        } break;

        case TOKEN_STRING: {
            value = allocate_value_item(parser);
            value->type = VALUE_STRING;
            value->data.string = lexer->token.token_data.string_data;
        } break;

        case TOKEN_BOOLEAN: {
            value = allocate_value_item(parser);
            value->type = VALUE_BOOLEAN;
            value->data.boolean = lexer->token.token_data.boolean_data;
        } break;

        case TOKEN_NULL: {
            value = allocate_value_item(parser);
            value->type = VALUE_NULL;
        } break;

        default: {
            print_error(lexer, "VALUE");
            return NULL;
        } break;
    }

    return value;
}

static void push_item(KihsonParser *parser, Item item) {
    if (parser->items_length == parser->items_capacity) {
        parser->items_capacity *= 2;
        parser->items = realloc(parser->items, parser->items_capacity);
    }

    parser->items[parser->items_length] = item;
    parser->items_length += 1;
}

static Item *allocate_item(KihsonParser *parser) {
    if (parser->items_length == parser->items_capacity) {
        parser->items_capacity *= 2;
        parser->items = realloc(parser->items, parser->items_capacity);
    }

    Item *item = &parser->items[parser->items_length];
    parser->items_length += 1;

    return item;
}

static Value *allocate_value_item(KihsonParser *parser) {
    Item *value_item = allocate_item(parser);
    *value_item = (Item) {
        .type = ITEM_VALUE,
    };
    return &value_item->data.value;
}

static ObjectItem *allocate_object_item(KihsonParser *parser) {
    Item *object_item = allocate_item(parser);
    *object_item = (Item) {
        .type = ITEM_OBJECT,
    };
    return &object_item->data.object;
}

static ArrayItem *allocate_array_item(KihsonParser *parser) {
    Item *array_item = allocate_item(parser);
    *array_item = (Item) {
        .type = ITEM_ARRAY,
    };
    return &array_item->data.array;
}

KihsonParser kihparser_new(void) {
    const long capacity = 1024;
    Item *items = malloc(capacity * sizeof(Item));
    
    KihsonParser parser = {
        .items          = items,
        .items_capacity = capacity,
        .items_length   = 0,
        .value_base     = NULL,
    };

    return parser;
}

void kihparser_clear(KihsonParser *parser) {
    parser->items_length = 0;
    parser->value_base    = NULL;
}

void kihparser_free(KihsonParser *parser) {
    free(parser->items);
    parser->items = NULL,
    parser->items_length = 0;
    parser->items_capacity = 0;
    parser->value_base = NULL;
}

void kihparser_parse(KihsonParser *parser, KihsonLexer *lexer) {
    kihlexer_advance_token(lexer);
    Value *value = parse_value(parser, lexer);
    parser->value_base = value;
}
