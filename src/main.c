#include <stdio.h>
#include <lib/kihson.h>
#include <string.h>

#define PROJECT_NAME "Kihson"

char *read_to_string(char *path) {
    FILE *json_file = fopen(path, "r");
    if (json_file == NULL) {
        return NULL;
    }

    fseek(json_file, 0, SEEK_END);
    size_t file_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);

    char *json_string = calloc(file_size + 1, 1);
    fread(json_string, 1, file_size, json_file);
    return json_string;
}

int test1(Kihson *kihson) {
    char *json_string = read_to_string("res/example1.json");
    if (json_string == NULL) {
        printf("Failed to open json file\n");
        return 1;
    }

    printf("%s\n", json_string);

    Value *json_head = kihson_parse(kihson, json_string);
    if (json_head == NULL) {
        printf("Json parsing failed\n");
        free(json_string);
        return 1;
    }

    if (!is_object(json_head)) {
        printf("Not and object, exiting...\n");
        free(json_string);
        return 1;
    }

    kihson_object_foreach(json_head, string1, value1) {
        printf("%s: ", string1->data);

        if (is_null(value1)) {
            printf("null");
        } else if (is_array(value1)) {
            printf("[ ");
            kihson_array_foreach(value1, value2) {
                if (is_number(value2)) {
                    printf("%lf ", get_double(value2));
                }
            }
            printf("]");
        }

        printf("\n");
    }

    free(json_string);
}

int test2(Kihson *kihson) {
    char *json_string = read_to_string("res/example2.json");
    if (json_string == NULL) {
        printf("Failed to open json file\n");
        return 1;
    }

    printf("%s\n", json_string);

    Value *json_head = kihson_parse(kihson, json_string);
    if (json_head == NULL) {
        printf("Json parsing failed\n");
        free(json_string);
        return 1;
    }

    if (!is_object(json_head)) {
        printf("Not and object, exiting...\n");
        free(json_string);
        return 1;
    }

    struct Config {
        char *token;
        int ids[16];
        int ids_count;
        float timeout;
    };


    struct Config config = {
        .token = NULL,
        .ids = {},
        .ids_count = 0,
        .timeout = 0.0,
    };

    Value *token_value = object_get_value(json_head, "token");
    if (is_string(token_value)) {
        config.token = get_cstring(token_value);
    }

    Value *ids_value = object_get_value(json_head, "item-ids");
    if (is_array(ids_value)) {
        kihson_array_foreach(ids_value, array_value) {
            if (is_number(array_value)) {
                config.ids[config.ids_count] = get_long(array_value);
                config.ids_count += 1;
            }
        }
    }

    Value *timeout_value = object_get_value(json_head, "timeout");
    if (is_number(ids_value)) {
        config.timeout = get_double(timeout_value);

    }

    free(json_string);
}

int test3(Kihson *kihson) {
    char *json_string = read_to_string("res/example2.json");
    if (json_string == NULL) {
        printf("Failed to open json file\n");
        return 1;
    }

    printf("%s\n", json_string);

    Value *json_head = kihson_parse(kihson, json_string);
    if (json_head == NULL) {
        printf("Json parsing failed\n");
        free(json_string);
        return 1;
    }

    if (!is_object(json_head)) {
        printf("Not and object, exiting...\n");
        free(json_string);
        return 1;
    }

    struct Config {
        char *token;
        int ids[16];
        int ids_count;
        float timeout;
    };


    struct Config config = {
        .token = NULL,
        .ids = {},
        .ids_count = 0,
        .timeout = 0.0,
    };

    kihson_object_foreach(json_head, string1, value1) {
        if (strcmp(string1->data, "token") == 0) {
            config.token = string1->data;
        } else if (strcmp(string1->data, "item-ids") == 0) {
            if (!is_array(value1)) {
                continue;
            }

            kihson_array_foreach(value1, value2) {
                if (is_number(value2)) {
                    config.ids[config.ids_count] = get_long(value2);
                    config.ids_count += 1;
                }
            }
        } else if (strcmp(string1->data, "timeout") == 0) {
            if (is_number(value1)) {
                config.timeout = get_double(value1);
            }  
        }
    }

    free(json_string);
}

int test3(Kihson *kihson) {
    char *json_string = read_to_string("res/example2.json");
    if (json_string == NULL) {
        printf("Failed to open json file\n");
        return 1;
    }

    printf("%s\n", json_string);

    Value *json_head = kihson_parse(kihson, json_string);
    if (json_head == NULL) {
        printf("Json parsing failed\n");
        free(json_string);
        return 1;
    }

    if (!is_object(json_head)) {
        printf("Not and object, exiting...\n");
        free(json_string);
        return 1;
    }

    struct Config {
        char *token;
        int ids[16];
        int ids_count;
        float timeout;
    };


    struct Config config = {
        .token = NULL,
        .ids = {},
        .ids_count = 0,
        .timeout = 0.0,
    };

    kihson_object_foreach_item(json_head, item1) {
        Value *token_value = try_get_value(item1, "token");
        if (is_string(token_value)) {
            config.token = get_cstring(token_value);
        }

        Value *ids_value = try_get_value(item1, "item-ids");
        if (is_array(ids_value)) {
            kihson_array_foreach(ids_value, array_value) {
                if (is_number(array_value)) {
                    config.ids[config.ids_count] = get_long(array_value);
                    config.ids_count += 1;
                }
            }
        }

        Value *timeout_value = try_get_value(item1, "timeout");
        if (is_number(ids_value)) {
            config.timeout = get_double(timeout_value);

        }
    }

    free(json_string);
}

int main(int argc, char **argv) {
    // printf("%i\n", (int)'\t');

    Kihson kihson = kihson_new();

    test1(&kihson);
    kihson_clear(&kihson);
    test2(&kihson);
    kihson_clear(&kihson);
    test3(&kihson);

    kihson_free(&kihson);

    return 0;
}
