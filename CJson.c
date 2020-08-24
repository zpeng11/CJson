#include "CJson.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CONTEXT_STACK_DEFAULT_CAPACITY 500

typedef struct context
{
    const char *json;
    char *stack;
    size_t top;
    size_t capacity;
} context;

static void skip_white_space(context *c)
{
    assert(c != NULL && c->json != NULL);
    const char *c_ = c->json;
    while (*c_ == '\t' || *c_ == '\n' || *c_ == ' ' || *c_ == '\r')
    {
        c_++;
    }
    c->json = c_;
}

static void *context_push(context *c, size_t size)
{
    assert(size > 0);
    while (c->top + size >= c->capacity)
    {
        c->capacity += c->capacity >> 1;
    }
    c->stack = (char *)realloc(c->stack, c->capacity);
    char *ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void *context_pop(context *c, size_t size)
{
    assert(size > 0);
    c->top -= size;
    return c->stack + c->top;
}

static int parse_word(context *c, cjson_value *v, const char *word, int succ_type)
{
    size_t i;
    for (i = 0; word[i]; i++)
    {
        if ((c->json)[i] != word[i])
            return CJSON_INVALID_VALUE;
    }
    c->json += i;
    v->type = succ_type;
    return CJSON_PARSE_OK;
}

#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
static int parse_number(context *c, cjson_value *v)
{
    const char *p = c->json;
    if (*p == '-')
        p++;
    if (*p == '0')
        p++;
    else
    {
        if (!ISDIGIT1TO9(*p))
            return CJSON_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++)
            ;
    }
    if (*p == '.')
    {
        p++;
        if (!ISDIGIT(*p))
            return CJSON_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++)
            ;
    }
    if (*p == 'e' || *p == 'E')
    {
        p++;
        if (*p == '+' || *p == '-')
            p++;
        if (!ISDIGIT(*p))
            return CJSON_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++)
            ;
    }
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return CJSON_NUMBER_TOO_BIG;
    v->type = CJSON_NUMBER;
    c->json = p;
    return CJSON_PARSE_OK;
}

#define PUTC(context_ptr, c) *(char *)context_push(context_ptr, sizeof(char)) = c
#define STRING_ERROR(error) \
    do                      \
    {                       \
        c->top = top;       \
        return error;       \
    } while (0)
static int parse_string_raw(context *c, char **str, size_t *len)
{
    skip_white_space(c);
    assert(*c->json == '\"');
    size_t top = c->top;
    const char *p = c->json;
    p++;
    while (1)
    {
        char ch = *p++;
        switch (ch)
        {
        case '\"':
            *len = c->top - top;
            *str = (*len) ? (char *)context_pop(c, *len) : NULL;
            c->json = p;
            return CJSON_PARSE_OK;
        case '\0':
            STRING_ERROR(CJSON_INVALID_STRING_MISS_QUOTATION);
        case '\\':
            switch (*p++)
            {
            case '\"':
                PUTC(c, '\"');
                break;
            case '\\':
                PUTC(c, '\\');
                break;
            case '/':
                PUTC(c, '/');
                break;
            case 'b':
                PUTC(c, '\b');
                break;
            case 'f':
                PUTC(c, '\f');
                break;
            case 'n':
                PUTC(c, '\n');
                break;
            case 'r':
                PUTC(c, '\r');
                break;
            case 't':
                PUTC(c, '\t');
                break;
            default:
                STRING_ERROR(CJSON_INVALID_STRING_ESCAPE);
                break;
            }
        default:
            if ((unsigned char)ch < 0x20)
                STRING_ERROR(CJSON_INVALID_STRING_CHAR);
            PUTC(c, ch);
            break;
        }
    }
}

static int parse_string(context *c, cjson_value *v)
{
    int ret;
    size_t len;
    char *str;
    if ((ret = parse_string_raw(c, &str, &len)) == CJSON_PARSE_OK)
        cjson_set_string(v, str, len);
    return ret;
}

static int parse_value(context *c, cjson_value *v)
{
    skip_white_space(c);
    switch (*c->json)
    {
    case 't':
        return parse_word(c, v, "true", CJSON_TRUE);
    case 'f':
        return parse_word(c, v, "false", CJSON_FALSE);
    case 'n':
        return parse_word(c, v, "null", CJSON_NULL);
    case '\"':
        return parse_string(c, v);
    default:
        return parse_number(c, v);
    }
}

int cjson_parse(cjson_value *v, const char *json_str)
{
    assert(v != NULL && json_str != NULL);
    context c;
    c.json = json_str;
    c.capacity = CONTEXT_STACK_DEFAULT_CAPACITY;
    c.top = 0;
    c.stack = (char *)malloc(c.capacity);
    int res;
    if ((res = parse_value(&c, v)) == CJSON_PARSE_OK)
    {
        skip_white_space(&c);
        if (*(c.json) != '\0')
        {
            res = CJSON_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);
    free(c.stack);
    return res;
}

int cjson_get_boolean(cjson_value *v)
{
    assert(v != NULL && (v->type == CJSON_TRUE || v->type == CJSON_FALSE));
    return (v->type == CJSON_TRUE) ? 1 : 0;
}

void cjson_set_boolean(cjson_value *v, int bool)
{
    assert(v != NULL);
    v->type = bool ? CJSON_TRUE : CJSON_FALSE;
}

double cjson_get_number(cjson_value *v)
{
    assert(v != NULL && v->type == CJSON_NUMBER);
    return v->u.n;
}

void cjson_set_number(cjson_value *v, double n)
{
    assert(v != NULL);
    v->type = CJSON_NUMBER;
    v->u.n = n;
}

const char *cjson_get_string(cjson_value *v)
{
    assert(v != NULL && v->type == CJSONS_STRING);
    return v->u.s.s;
}

void cjson_set_string(cjson_value *v, const char *str, size_t str_len)
{
    cjson_free(v);
    v->type = CJSONS_STRING;
    v->u.s.len = str_len;
    if (str == NULL)
    {
        v->u.s.s = NULL;
        return;
    }
    memcpy((v->u.s.s = (char *)malloc(v->u.s.len + 1)), str, str_len);
    v->u.s.s[str_len] = '\0';
}

void cjson_free(cjson_value *v)
{
    assert(v != NULL);
    switch (v->type)
    {
    case CJSONS_STRING:
        free(v->u.s.s);
        break;
    default:
        v->type = CJSON_NULL;
        break;
    }
}