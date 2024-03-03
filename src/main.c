#include <stdio.h>
#include <lib/kihson.h>

#define PROJECT_NAME "Kihson"

int main(int argc, char **argv) {
    printf("Hello meson!\n");

    if (argc != 1) {
        printf("%s takes no arguments.\n", argv[0]);
        return 1;
    }

    printf("This is project %s.\n", PROJECT_NAME);
    printf("%i\n", (int)'\t');

    char *json = "{ \"string1\": null, \"string1\":[ 0, 2e-123, -3.3, 4  ], \"string2\": { \"string3\": true, \"string4\": 321.345 } }";
    Kihson kihson = kihson_new();
    kihson_parse(&kihson, json);

    return 0;
}
