#include "kihson_lexer.h"

static void kihlexer_advance_byte(KihsonLexer *lexer);
static bool kihlexer_string_match(KihsonLexer *lexer, char *string);
static char kihlexer_peek_byte(KihsonLexer *lexer);

static TokenLexResult kihlexer_chop_string(KihsonLexer *lexer, Token *token);
static TokenLexResult kihlexer_chop_number(KihsonLexer *lexer, Token *token);

static bool is_digit(char byte);
static void debug_lexer_fail(KihsonLexer *lexer, char *where);

KihsonTokenArray kihtokens_new(void) {
    int tokens_capacity = 128;
    Token *tokens = malloc(sizeof(Token) * tokens_capacity);

    KihsonTokenArray token_array = {
        .tokens = tokens,
        .tokens_capacity = tokens_capacity,
        .tokens_length = 0,
    };

    return token_array;
}

void kihtokens_clear(KihsonTokenArray *token_array) {
    token_array->tokens_length = 0;
}

void kihtokens_free(KihsonTokenArray *token_array) {
    token_array->tokens_capacity = 0;
    token_array->tokens_length = 0;
    free(token_array->tokens);
    token_array->tokens = NULL;
}

void kihtokens_push_token(KihsonTokenArray *token_array, Token token) {
    if (token_array->tokens_capacity == token_array->tokens_length) {
        token_array->tokens_capacity *= 2;
        token_array->tokens = realloc(token_array->tokens, token_array->tokens_capacity * sizeof(Token));
    }

    token_array->tokens[token_array->tokens_length] = token;
    token_array->tokens_length += 1;
}


KihsonLexer kihlexer_new(void) {
    int string_capacity = 1024;
    KihsonString all_json_strings = kihstring_with(string_capacity);

    KihsonLexer lexer = {
        .all_json_strings = all_json_strings,

        .byte_offset = 0,
        .line_offset = 0,
        .line_number = 0,
    };

    return lexer;
}

void kihlexer_load(KihsonLexer *lexer, KihsonStringView json_string) {
    lexer->json_string = json_string;
}

void kihlexer_load_cstr(KihsonLexer *lexer, char *json_cstring) {
    KihsonStringView json_string = {
        .data   = json_cstring,
        .length = -1,
    };

    lexer->json_string = json_string;
}

bool kihlexer_tokenize(KihsonLexer *lexer, KihsonTokenArray *token_array) {
    while (true) {
        kihlexer_advance_token(lexer);

        switch (lexer->token.token_type) {
            case TOKEN_END: {
                return true;
            } break;

            case TOKEN_ERROR: {
                return false;
            } break;

            default: {
                kihtokens_push_token(token_array, lexer->token);
            } break;
        }
    }
}

void kihlexer_advance_token(KihsonLexer *lexer) {
    lexer->token.line_offset = lexer->line_offset;
    lexer->token.line_number = lexer->line_number;

    while (true) {
        char next_byte = kihlexer_peek_byte(lexer);

        switch (next_byte) {
            case '\0': {
                lexer->token.token_type = TOKEN_END;
                return;
            }

            case ' ': {
            case '\r': 
            case '\t': 
            case '\n':
                kihlexer_advance_byte(lexer);
                continue;
            }

            case '{': {
                lexer->token.token_type = TOKEN_CURLY_BRACKET_LEFT;
            } break;

            case '}': {
                lexer->token.token_type = TOKEN_CURLY_BRACKET_RIGHT;
            } break;

            case '[': {
                lexer->token.token_type = TOKEN_SQUARE_BRACKET_LEFT;
            } break;

            case ']': {
                lexer->token.token_type = TOKEN_SQUARE_BRACKET_RIGHT;
            } break;

            case ',': {
                lexer->token.token_type = TOKEN_COMMA;
            } break;

            case ':': {
                lexer->token.token_type = TOKEN_COLON;
            } break;

            case 'n': {
                lexer->token.token_type = TOKEN_NULL;
                if (!kihlexer_string_match(lexer, "null")) {
                    debug_lexer_fail(lexer, "TOKEN_NULL");
                    lexer->token.token_type = TOKEN_ERROR;
                }
                return;
            } break;

            case 't': {
                lexer->token.token_type = TOKEN_BOOLEAN;
                lexer->token.token_data.boolean_data = true;
                if (!kihlexer_string_match(lexer, "true")) {
                    debug_lexer_fail(lexer, "TOKEN_BOOLEAN(true)");
                    lexer->token.token_type = TOKEN_ERROR;
                }
                return;
            } break;

            case 'f': {
                lexer->token.token_type = TOKEN_BOOLEAN;
                lexer->token.token_data.boolean_data = false;
                if (!kihlexer_string_match(lexer, "false")) {
                    debug_lexer_fail(lexer, "TOKEN_BOOLEAN(false)");
                    lexer->token.token_type = TOKEN_ERROR;
                }
                return;
            } break;

            case '\"': {
                int result = kihlexer_chop_string(lexer, &lexer->token);
                if (result != LEX_RESULT_SUCCESS) {
                    debug_lexer_fail(lexer, "TOKEN_STRING");
                    lexer->token.token_type = TOKEN_ERROR;
                }
                return;
            } break;

            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                int result = kihlexer_chop_number(lexer, &lexer->token);
                if (result != LEX_RESULT_SUCCESS) {
                    debug_lexer_fail(lexer, "TOKEN_NUMBER");
                    lexer->token.token_type = TOKEN_ERROR;
                }
                return;
            } break;

            default: {
                debug_lexer_fail(lexer, "UNKNOWN SYMBOL");
                lexer->token.token_type = TOKEN_ERROR;
                return;
            } break;
        }

        kihlexer_advance_byte(lexer);
        return;
    }
}

void kihlexer_clear(KihsonLexer *lexer) {
    kihstring_clear(&lexer->all_json_strings);
    lexer->json_string = (KihsonStringView) {
        .data   = NULL,
        .length = 0,
    };

    lexer->byte_offset   = 0;
    lexer->line_offset   = 0;
    lexer->line_number   = 0;
}

void kihlexer_free(KihsonLexer *lexer) {
    kihlexer_clear(lexer);
    kihstring_free(&lexer->all_json_strings);
}

static void debug_lexer_fail(KihsonLexer *lexer, char *where) {
    fprintf(stderr, "ERROR: Tokanization failed after tring to parse \"%s\"\n", where);
    fprintf(stderr, "    byte:   %i\n", lexer->byte_offset + 1);
    fprintf(stderr, "    line:   %i\n", lexer->line_number + 1);
    fprintf(stderr, "    column: %i\n", lexer->line_offset + 1);  
    // fprintf(stderr, "    token:  %i\n",  lexer->tokens_length);  
}

static void kihlexer_advance_byte(KihsonLexer *lexer) {
    if (lexer->byte_offset == lexer->json_string.length) {
        return;
    }

    if (lexer->json_string.data[lexer->byte_offset] == 0) {
        return;
    }

    char byte = lexer->json_string.data[lexer->byte_offset];
    lexer->byte_offset += 1;
    lexer->line_offset += 1;

    // TODO: Handle windows \r char?
    if (byte == '\n') {
        lexer->line_number += 1;
        lexer->line_offset  = 0;
    }
} 

static char kihlexer_peek_byte(KihsonLexer *lexer) {
    if (lexer->byte_offset == lexer->json_string.length) {
        return 0;
    }

    if (lexer->json_string.data[lexer->byte_offset] == 0) {
        return 0;
    }

    return lexer->json_string.data[lexer->byte_offset];
}

static TokenLexResult kihlexer_chop_string(KihsonLexer *lexer, Token *token) {
    kihlexer_advance_byte(lexer);

    bool previous_escape  = false;
    // bool previous_unicode = false;

    // int start_offset = lexer->byte_offset + 1;
    // char *string_start = lexer->all_json_strings.data + lexer->all_json_strings.length;
    int start_offset = lexer->all_json_strings.length;

    char byte = kihlexer_peek_byte(lexer);
    while (byte != 0) {
        kihlexer_advance_byte(lexer);

        switch (byte) {
            case '\\': {
                if (previous_escape) {
                    byte = '\\';
                }

                previous_escape = !previous_escape;
            } break;

            case '\b':  
            case '\f':  
            case '\n':  
            case '\r':  
            case '\t': {
                return LEX_RESULT_FAILURE;
            } break;

            case '/': {
                // Just pass?
                byte = '/';
            } break;

            case 'b': if (previous_escape) {
                byte = '\b';
            } break;

            case 'f': if (previous_escape) {
                byte = '\f';
            } break;

            case 'n': if (previous_escape) {
                byte = '\n';
            } break;

            case 'r': if (previous_escape) {
                byte = '\r';
            } break;

            case 't': if (previous_escape) {
                byte = '\t';
            } break;

            case 'u': {
                if (previous_escape) {
                    // TODO:
                    // long parsed_unicode;
                    // long result = kihlexer_parse_unicode(lexer, &parsed_unicode);
                    // if (result == LEX_RESULT_FAILURE) {
                    //     return result;
                    // }
                } else {
                    byte = 'u';
                }
            } break;

            case '\"': {
                if (!previous_escape) {
                    // Null terminate the string as a form of a separation.
                    kihstring_push(&lexer->all_json_strings, 0);

                    // String tokenization step ends here.
                    // int length = lexer->byte_offset - start_offset;
                    // token->token_data.string_data = kihstring_view_with(string_start, length);
                    token->token_data.string_index = start_offset;
                    token->token_type = TOKEN_STRING;
                    return LEX_RESULT_SUCCESS;
                } else {
                    kihstring_push(&lexer->all_json_strings, byte);
                }
            } break;

            default: {
                if (previous_escape) {
                    // This is not possible. All escape code cases for the JSON format are covered above.
                    return LEX_RESULT_FAILURE;
                }
            } break;
        }

        kihstring_push(&lexer->all_json_strings, byte);
        byte = kihlexer_peek_byte(lexer);
    }

    // This is unreachable, string always end somewhere with the " character so the tokenization was a failure.
    return LEX_RESULT_FAILURE;
}


static bool is_digit(char byte) {
    return byte >= '0' && byte <= '9';
}

// TODO: Improve and do overflow checks.
static TokenLexResult kihlexer_chop_number(KihsonLexer *lexer, Token *token) {
    bool is_negative = false;

    char next_byte = kihlexer_peek_byte(lexer);
    if (next_byte == '-') {
        is_negative = true;
        kihlexer_advance_byte(lexer);
    }

    long natural_value = 0;

    next_byte = kihlexer_peek_byte(lexer);
    if (next_byte == '0') {
        // Pass, go to fraction and exponent parsing.
        kihlexer_advance_byte(lexer);
    } else if (next_byte >= '1' && next_byte <= '9') {
        // Parse one or more digits here.
        next_byte = kihlexer_peek_byte(lexer);
        while (is_digit(next_byte)) {
            kihlexer_advance_byte(lexer);

            int value = next_byte - '0';
            natural_value *= 10;
            natural_value += value;

            next_byte = kihlexer_peek_byte(lexer);
        }
    } else {
        // Tokenizer byte mismatch. Exit out.
        return LEX_RESULT_FAILURE;
    }

    long fraction_value = 0;
    bool is_fraction = false;

    next_byte = kihlexer_peek_byte(lexer);
    if (next_byte == '.') {
        is_fraction = true;
        kihlexer_advance_byte(lexer);

        next_byte = kihlexer_peek_byte(lexer);
        if (!is_digit(next_byte)) {
            return LEX_RESULT_FAILURE;
        }

        while (is_digit(next_byte)) {
            kihlexer_advance_byte(lexer);

            int value = next_byte - '0';
            fraction_value *= 10;
            fraction_value += value;

            next_byte = kihlexer_peek_byte(lexer);
        }
    }

    long exponent_value = 0;
    bool has_exponent = false;
    bool exponent_negative = false;

    if (next_byte == 'e' || next_byte == 'E') {
        has_exponent = true;
        kihlexer_advance_byte(lexer);

        next_byte = kihlexer_peek_byte(lexer);
        if (next_byte == '-') {
            exponent_negative = true;
            kihlexer_advance_byte(lexer);
        } else if (next_byte == '+') {
            kihlexer_advance_byte(lexer);
        }

        next_byte = kihlexer_peek_byte(lexer);
        if (!is_digit(next_byte)) {
            return LEX_RESULT_FAILURE;
        }

        while (is_digit(next_byte)) {
            kihlexer_advance_byte(lexer);

            int value = next_byte - '0';
            exponent_value *= 10;
            exponent_value += value;

            next_byte = kihlexer_peek_byte(lexer);
        }
    }

    double double_data = natural_value;
    if (is_fraction) {
        long digit_count = (long)log10(fraction_value) + 1;
        long divisor = pow(10.0, digit_count);
        double fraction_part = (double)fraction_value / divisor;
        double_data += fraction_part;
    }

    if (has_exponent) {
        if (exponent_negative) {
            exponent_value *= -1.0;
        }

        double exponent_multiplier = pow(10.0, exponent_value);
        double_data *= exponent_multiplier;
    }

    if (is_negative) {
        double_data   = -double_data;
        natural_value = -natural_value;
    }

    Number number = {
        .double_data = double_data,
        .long_data   = natural_value,
    };

    token->token_type = TOKEN_NUMBER;
    token->token_data.number_data = number;

    return LEX_RESULT_SUCCESS;
}

static bool kihlexer_string_match(KihsonLexer *lexer, char *string) {
    int offset = 0;
    char next_byte = kihlexer_peek_byte(lexer);
    while (string[offset] != 0) {
        if (next_byte != string[offset]) {
            return false;
        }

        kihlexer_advance_byte(lexer);
        next_byte = kihlexer_peek_byte(lexer);

        if (next_byte == 0) {
            return false;
        }

        offset += 1;
    }

    return true;
}
