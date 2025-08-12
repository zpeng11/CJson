#ifndef CJSON_H
#define CJSON_H
#include <stddef.h>


typedef struct cjson_value cjson_value;
typedef struct cjson_member cjson_member;

enum 
{
    CJSON_PARSE_OK = 0,
    CJSON_EXPECT_VALUE,
    CJSON_INVALID_VALUE,
    CJSON_ROOT_NOT_SINGULAR,
    CJSON_NUMBER_TOO_BIG,
    CJSON_INVALID_STRING_CHAR,
    CJSON_INVALID_STRING_ESCAPE,
    CJSON_INVALID_STRING_MISS_QUOTATION,
    CJSON_INVALID_UNICODE_HEX,
    CJSON_INVALID_UNICODE_SURROGATE,
    CJSON_MISS_KEY,
    CJSON_MISS_COLON,
    CJSON_MISS_COMMA_OR_SQUARE_BRACKET,
    CJSON_MISS_COMMA_OR_CURLY_BRACKET
};

typedef enum{
    CJSON_NULL, CJSON_TRUE, CJSON_FALSE, CJSON_NUMBER, CJSON_STRING, CJSON_ARRAY, CJSON_OBJECT
}cjson_type;

struct cjson_value
{
    union{
        struct {cjson_member * m; size_t size; size_t capacity; } o;
        struct {cjson_value * a; size_t size; size_t capacity; }a;
        struct { char * s; size_t len;}s;
        double n;
    }u;
    cjson_type type;
};

struct cjson_member
{
    char * key;
    size_t len;
    cjson_value v;
};

#define cjson_init(cjson_value_ptr) do { (cjson_value_ptr)->type = CJSON_NULL; } while(0)
int cjson_parse(cjson_value * v, const char * json_str);
void cjson_free(cjson_value * v);

int cjson_get_boolean(const cjson_value * v);
void cjson_set_boolean(cjson_value * v, int bool);

double cjson_get_number(const cjson_value * v);
void cjson_set_number(cjson_value * v, double n);

const char * cjson_get_string(const cjson_value * v);
size_t cjson_get_string_length(const cjson_value * v);
void cjson_set_string(cjson_value *v, const char *str, size_t str_len);

void cjson_set_array(cjson_value *v, size_t capacity);
size_t cjson_get_array_size(const cjson_value *v);
size_t cjson_get_array_capacity(const cjson_value *v);
cjson_value *cjson_get_array_element(cjson_value *v, size_t index);

void cjson_set_object(cjson_value *v, size_t capacity);

char *cjson_stringify(const cjson_value *v, size_t *length);
#endif /*CJSON_H*/