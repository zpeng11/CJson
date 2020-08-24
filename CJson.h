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
    CJSON_INVALID_STRING_MISS_QUOTATION
};

typedef enum{
    CJSON_NULL, CJSON_TRUE, CJSON_FALSE, CJSON_NUMBER,CJSONS_STRING, CJSON_ARRAY, CJSON_OBJECT
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

int cjson_get_boolean(cjson_value * v);
void cjson_set_boolean(cjson_value * v, int bool);

double cjson_get_number(cjson_value * v);
void cjson_set_number(cjson_value * v, double n);

const char * cjson_get_string(cjson_value * v);
void cjson_set_string(cjson_value *v, const char *str, size_t str_len);
#endif /*CJSON_H*/