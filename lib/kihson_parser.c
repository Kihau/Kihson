#include "kihson_parser.h"

static void print_error(KihsonLexer *lexer, char *expected);
static bool token_is_value(Token *token);

static long parse_value(KihsonParser *parser, KihsonLexer *lexer);
static long parse_object(KihsonParser *parser, KihsonLexer *lexer);
static long parse_array(KihsonParser *parser, KihsonLexer *lexer);

static long allocate_item(KihsonParser *parser);
static long allocate_value_item(KihsonParser *parser);
static long allocate_object_item(KihsonParser *parser);
static long allocate_array_item(KihsonParser *parser);

static Value      *get_value_item(KihsonParser *parser, long index);
static Object     *get_object(KihsonParser *parser, long index);
static ObjectItem *get_object_item(KihsonParser *parser, long index);
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
    long object_index = allocate_value_item(parser);

    { // TOOD: Improve
        Value *object = get_value_item(parser, object_index);
        object->type = VALUE_OBJECT;
        object->data.object = (Object) {
            .item_count = 0,
            .item_list_index = -1,
        };
    }

    kihlexer_advance_token(lexer);
    if (lexer->token.token_type == TOKEN_CURLY_BRACKET_RIGHT) {
        return object_index;
    }

    long object_item_index = allocate_object_item(parser);
    {
        Value *object = get_value_item(parser, object_index);
        object->data.object.item_list_index = object_item_index;
    }

    while (true) {
        {
            Value *object = get_value_item(parser, object_index);
            object->data.object.item_count += 1;
        }

        if (lexer->token.token_type != TOKEN_STRING) {
            print_error(lexer, "STRING");
            return -1;
        }

        {
            ObjectItem *object_item = get_object_item(parser, object_item_index);
            object_item->string_index = lexer->token.token_data.string_index;
        }

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

        {
            ObjectItem *object_item = get_object_item(parser, object_item_index);
            object_item->value_index = value_index;
        }

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type == TOKEN_CURLY_BRACKET_RIGHT) {
            return object_index;
        } else if (lexer->token.token_type != TOKEN_COMMA) {
            print_error(lexer, "COMMA");
            return -1;
        }

        long next_item_index = allocate_object_item(parser);
        {
            ObjectItem *object_item = get_object_item(parser, object_item_index);
            object_item->next_item_index = next_item_index;
        }
        object_item_index = next_item_index;

        kihlexer_advance_token(lexer);
    }
}

static long parse_array(KihsonParser *parser, KihsonLexer *lexer) {
    long array_index = allocate_value_item(parser);

    {
        Value *array = get_value_item(parser, array_index);
        array->type = VALUE_ARRAY;
        array->data.array = (Array) {
            .item_count = 0,
            .item_list_index = -1,
        };
    }

    kihlexer_advance_token(lexer);
    if (lexer->token.token_type == TOKEN_SQUARE_BRACKET_RIGHT) {
        return array_index;
    }

    long array_item_index = allocate_object_item(parser);
    {
        Value *array = get_value_item(parser, array_index);
        array->data.array.item_list_index = array_item_index;
    }

    while (true) {
        {
            Value *array = get_value_item(parser, array_index);
            array->data.array.item_count += 1;
        }

        long value_index = parse_value(parser, lexer);
        if (value_index == -1) {
            return -1;
        }

        {
            ArrayItem *array_item = get_array_item(parser, array_item_index);
            array_item->value_index = value_index;
        }

        kihlexer_advance_token(lexer);
        if (lexer->token.token_type == TOKEN_SQUARE_BRACKET_RIGHT) {
            return array_index;
        } else if (lexer->token.token_type != TOKEN_COMMA) {
            print_error(lexer, "COMMA");
            return -1;
        }

        long next_item_index = allocate_array_item(parser);
        {
            ArrayItem *array_item = get_array_item(parser, array_item_index);
            array_item->next_item_index = next_item_index;
        }
        array_item_index = next_item_index;

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
            value_index = allocate_value_item(parser);
            Value *value = get_value_item(parser, value_index);
            value->type = VALUE_NUMBER;
            value->data.number = lexer->token.token_data.number_data;
        } break;

        case TOKEN_STRING: {
            value_index = allocate_value_item(parser);
            Value *value = get_value_item(parser, value_index);
            value->type = VALUE_STRING;
            value->data.string_index = lexer->token.token_data.string_index;
        } break;

        case TOKEN_BOOLEAN: {
            value_index = allocate_value_item(parser);
            Value *value = get_value_item(parser, value_index);
            value->type = VALUE_BOOLEAN;
            value->data.boolean = lexer->token.token_data.boolean_data;
        } break;

        case TOKEN_NULL: {
            value_index = allocate_value_item(parser);
            Value *value = get_value_item(parser, value_index);
            value->type = VALUE_NULL;
        } break;

        default: {
            print_error(lexer, "VALUE");
            return NULL;
        } break;
    }

    return value_index;
}

static long allocate_item(KihsonParser *parser) {
    if (parser->items_length == parser->items_capacity) {
        parser->items_capacity *= 2;
        parser->items = realloc(parser->items, parser->items_capacity * sizeof(Item));
    }

    long item_index = parser->items_length;
    parser->items_length += 1;
    return item_index;
}

static long allocate_value_item(KihsonParser *parser) {
    long index = allocate_item(parser);
    parser->items[index].type = ITEM_VALUE;
    return index;
}

static long allocate_object_item(KihsonParser *parser) {
    long index = allocate_item(parser);
    parser->items[index].type = ITEM_OBJECT;
    return index;
}

static long allocate_array_item(KihsonParser *parser) {
    long index = allocate_item(parser);
    parser->items[index].type = ITEM_ARRAY;
    return index;
}

static Value *get_value_item(KihsonParser *parser, long index) {
    if (index >= parser->items_length) {
        return NULL;
    }

    Item *value_item = &parser->items[index];
    return &value_item->data.value;
}

static Object *get_object(KihsonParser *parser, long index) {
    if (index >= parser->items_length) {
        return NULL;
    }

    Item *value_item = &parser->items[index];
    return &value_item->data.value.data.object;
}

static ObjectItem *get_object_item(KihsonParser *parser, long index) {
    if (index >= parser->items_length) {
        return NULL;
    }

    Item *object_item = &parser->items[index];
    return &object_item->data.object;
}

static ArrayItem *get_array_item(KihsonParser *parser, long index) {
    if (index >= parser->items_length) {
        return NULL;
    }

    Item *array_item = &parser->items[index];
    return &array_item->data.array;
}

KihsonParser kihparser_new(void) {
    const long capacity = 1024;
    Item *items = malloc(capacity * sizeof(Item));
    
    KihsonParser parser = {
        .items          = items,
        .items_capacity = capacity,
        .items_length   = 0,
        // .value_base     = NULL,
    };

    return parser;
}

void kihparser_clear(KihsonParser *parser) {
    parser->items_length = 0;
    // parser->value_base    = NULL;
}

void kihparser_free(KihsonParser *parser) {
    free(parser->items);
    parser->items = NULL,
    parser->items_length = 0;
    parser->items_capacity = 0;
    // parser->value_base = NULL;
}

KihsonValue *kihparser_parse(KihsonParser *parser, KihsonLexer *lexer) {
    if (parser->items == NULL) {
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

    Value json_head = parser->items[value_index].data.value;

    KihsonValue value = {
        .data = json_head.data,
        .type = json_head.type,
        .items = parser->items,
        .strings = lexer->all_json_strings.data,
    };

    parser->value_base = value;
    return &parser->value_base;
}
