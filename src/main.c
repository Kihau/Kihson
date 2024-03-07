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
    fclose(json_file);
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

    kihson_object_foreach(kihson, json_head, string1, value1) {
        printf("%s: ", string1);

        if (is_null(value1)) {
            printf("null");
        } else if (is_array(value1)) {
            printf("[ ");
            kihson_array_foreach(kihson, value1, value2) {
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
    
    int *array = NULL;
    int stack_array[32];
    int count;

    array = stack_array;

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

    // Value *output = object_get_value(kihson, json_head, "timeout");
    // printf("%lf\n", output->data.number.double_data);

    // kihson_object_foreach_item(kihson, json_head, item1) {

    kihson_object_foreach_index(kihson, json_head, index) {
        ObjectItem *item1 = get_object_item(kihson, index);

        Value *token_value = try_get_value(kihson, item1, "token");
        if (is_string(token_value)) {
            config.token = get_string(kihson, token_value);
        }

        Value *ids_value = try_get_value(kihson, item1, "item-ids");
        if (is_array(ids_value)) {
            kihson_array_foreach(kihson, ids_value, array_value) {
                if (is_number(array_value)) {
                    config.ids[config.ids_count] = get_long(array_value);
                    config.ids_count += 1;
                }
            }
        }

        Value *timeout_value = try_get_value(kihson, item1, "timeout");
        if (is_number(timeout_value)) {
            config.timeout = get_double(timeout_value);

        }
    }

    printf(
        "struct Config { \n"
        "    token:     %s,\n"
        "    ids:       [ ",
        config.token
    );

    for (int i = 0; i < config.ids_count; i++) {
        printf("%i, ", config.ids[i]);
    }

    printf(
        "],\n"
        "    ids_count: %i,\n"
        "    timeout:   %f,\n"
        "};\n",
        config.ids_count, config.timeout
    );

    free(json_string);
}

int test3(Kihson *kihson) {
    char *json_string = read_to_string("res/example3.json");
    if (json_string == NULL) {
        printf("Failed to open json file\n");
        return 1;
    }

    Value *json_head = kihson_parse(kihson, json_string);
    if (json_head == NULL) {
        printf("Json parsing failed\n");
        free(json_string);
        return 1;
    }

    if (!is_array(json_head)) {
        printf("Not an array");
        free(json_string);
        return 1;
    }

    kihson_array_foreach(kihson, json_head, gta_object) {
        if (!is_object(gta_object)) {
            continue;
        }

        kihson_object_foreach_item(kihson, gta_object, field) {
            Value *value_name = try_get_value(kihson, field, "Name");
            if (is_string(value_name)) {
                char *name = get_string(kihson, value_name);
                printf("Object name = %s,    ", name);
            }

            Value *spawn_in_car = try_get_value(kihson, field, "CanSpawnInCar");
            if (is_boolean(spawn_in_car)) {
                bool boolean = get_boolean(spawn_in_car);
                printf("Can spawn in cat = %s\n", boolean ? "true" : "false");
            }
        }

        // goto exit_horrible_macro;
    }

    // exit_horrible_macro:

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
