#ifndef KIHSON_STRING
#define KIHSON_STRING

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    long length;
    long capacity;
} KihsonString;

typedef struct {
    char *data;
    long length;
} KihsonStringView;


KihsonString kihstring_new(void);
KihsonString kihstring_with(long capacity);
void kihstring_push(KihsonString *string, char byte);
void kihstring_clear(KihsonString *string);
void kihstring_free(KihsonString *string);

KihsonStringView kihstring_view_new(char *string);
KihsonStringView kihstring_view_with(char *string, long length);

#endif // KIHSON_STRING
