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
static int parse_value(context *c, cjson_value *v);

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

static const char *parse_hex4(const char *p, unsigned *u)
{
    int i;
    *u = 0;
    for (i = 0; i < 4; i++)
    {
        char ch = *p++;
        *u <<= 4;
        if (ch >= '0' && ch <= '9')
            *u |= ch - '0';
        else if (ch >= 'A' && ch <= 'F')
            *u |= ch - ('A' - 10);
        else if (ch >= 'a' && ch <= 'f')
            *u |= ch - ('a' - 10);
        else
            return NULL;
    }
    return p;
}
#define PUTC(c, ch)                                    \
    do                                                 \
    {                                                  \
        *(char *)context_push(c, sizeof(char)) = (ch); \
    } while (0)
static void encode_utf8(context *c, unsigned u)
{
    if (u <= 0x7F)
        PUTC(c, u & 0xFF);
    else if (u <= 0x7FF)
    {
        PUTC(c, 0xC0 | ((u >> 6) & 0xFF));
        PUTC(c, 0x80 | (u & 0x3F));
    }
    else if (u <= 0xFFFF)
    {
        PUTC(c, 0xE0 | ((u >> 12) & 0xFF));
        PUTC(c, 0x80 | ((u >> 6) & 0x3F));
        PUTC(c, 0x80 | (u & 0x3F));
    }
    else
    {
        assert(u <= 0x10FFFF);
        PUTC(c, 0xF0 | ((u >> 18) & 0xFF));
        PUTC(c, 0x80 | ((u >> 12) & 0x3F));
        PUTC(c, 0x80 | ((u >> 6) & 0x3F));
        PUTC(c, 0x80 | (u & 0x3F));
    }
}

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
    unsigned u, u2;
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
            case 'u':
                if (!(p = parse_hex4(p, &u)))
                    STRING_ERROR(CJSON_INVALID_UNICODE_HEX);
                if (u >= 0xD800 && u <= 0xDBFF)
                { /* surrogate pair */
                    if (*p++ != '\\')
                        STRING_ERROR(CJSON_INVALID_UNICODE_SURROGATE);
                    if (*p++ != 'u')
                        STRING_ERROR(CJSON_INVALID_UNICODE_SURROGATE);
                    if (!(p = parse_hex4(p, &u2)))
                        STRING_ERROR(CJSON_INVALID_UNICODE_HEX);
                    if (u2 < 0xDC00 || u2 > 0xDFFF)
                        STRING_ERROR(CJSON_INVALID_UNICODE_SURROGATE);
                    u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                }
                encode_utf8(c, u);
                break;
            default:
                STRING_ERROR(CJSON_INVALID_STRING_ESCAPE);
            }
            break;
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

static int parse_array(context *c, cjson_value *v)
{
    assert(c != NULL && v != NULL && *c->json == '[');
    size_t size = 0;
    c->json++;
    skip_white_space(c);
    int ret;
    if (*c->json == ']')
    {
        cjson_set_array(v, 0);
        c->json++;
        return CJSON_PARSE_OK;
    }
    while (1)
    {
        cjson_value a;
        cjson_init(&a);
        if ((ret = parse_value(c, &a)) != CJSON_PARSE_OK)
            break;
        memcpy(context_push(c, sizeof(cjson_value)), &a, sizeof(cjson_value));
        size++;
        skip_white_space(c);
        if (*c->json == ',')
        {
            c->json++;
            skip_white_space(c);
        }
        else if (*c->json == ']')
        {
            c->json++;
            cjson_set_array(v, size);
            memcpy(v->u.a.a, context_pop(c, size * sizeof(cjson_value)), size * sizeof(cjson_value));
            v->u.a.size = size;
            return CJSON_PARSE_OK;
        }
        else
        {
            ret = CJSON_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
    //cases that failed
    for (int i = 0; i < size; i++)
    {
        cjson_free((cjson_value *)context_pop(c, sizeof(cjson_value)));
    }
    v->type = CJSON_NULL;
    return ret;
}

int parse_object(context *c, cjson_value *v)
{
    assert(c != NULL && v != NULL && *c->json == '{');
    size_t size = 0;
    int ret;
    c->json++;
    skip_white_space(c);
    if (*c->json == '}')
    {
        cjson_set_object(v, 0);
        c->json++;
        return CJSON_PARSE_OK;
    }
    while (1)
    {
        cjson_member m;
        cjson_init(&m.v);
        char *key = NULL;
        if (*c->json != '\"')
        {
            ret = CJSON_MISS_KEY;
            break;
        }
        if ((ret = parse_string_raw(c, &key, &m.len)) != CJSON_PARSE_OK)
        {
            break;
        }
        memcpy((m.key = (char *)malloc(sizeof(char) * (m.len + 1))), key, m.len);
        m.key[m.len] = '\0';
        skip_white_space(c);
        if (*c->json != ':')
        {
            free(m.key);
            ret = CJSON_MISS_COLON;
            break;
        }
        c->json++;
        if ((ret = parse_value(c, &(m.v))) != CJSON_PARSE_OK)
        {
            free(m.key);
            break;
        }
        memcpy(context_push(c, sizeof(cjson_member)), &m, sizeof(cjson_member));
        size++;
        skip_white_space(c);
        if (*c->json == ',')
        {
            c->json++;
            skip_white_space(c);
            continue;
        }
        else if (*c->json == '}')
        {
            cjson_set_object(v, size);
            memcpy(v->u.o.m, context_pop(c, sizeof(cjson_member) * size), sizeof(cjson_member) * size);
            v->u.o.size = size;
            c->json++;
            return CJSON_PARSE_OK;
        }
        else
        {
            ret = CJSON_MISS_COMMA_OR_CURLY_BRACKET;
            break;
        }
    }
    //cases that failed
    for (int i = 0; i < size; i++)
    {
        cjson_member *m = (cjson_member *)context_pop(c, sizeof(cjson_member));
        free(m->key);
        cjson_free(&m->v);
    }
    v->type = CJSON_NULL;
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
    case '[':
        return parse_array(c, v);
    case '{':
        return parse_object(c, v);
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

static void stringify_string(context *c, const char *s, size_t len)
{
    static const char hex_digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    size_t i, size;
    char *head, *p;
    assert(s != NULL);
    p = head = context_push(c, size = len * 6 + 2); /* "\u00xx..." */
    *p++ = '"';
    for (i = 0; i < len; i++)
    {
        unsigned char ch = (unsigned char)s[i];
        switch (ch)
        {
        case '\"':
            *p++ = '\\';
            *p++ = '\"';
            break;
        case '\\':
            *p++ = '\\';
            *p++ = '\\';
            break;
        case '\b':
            *p++ = '\\';
            *p++ = 'b';
            break;
        case '\f':
            *p++ = '\\';
            *p++ = 'f';
            break;
        case '\n':
            *p++ = '\\';
            *p++ = 'n';
            break;
        case '\r':
            *p++ = '\\';
            *p++ = 'r';
            break;
        case '\t':
            *p++ = '\\';
            *p++ = 't';
            break;
        default:
            if (ch < 0x20)
            {
                *p++ = '\\';
                *p++ = 'u';
                *p++ = '0';
                *p++ = '0';
                *p++ = hex_digits[ch >> 4];
                *p++ = hex_digits[ch & 15];
            }
            else
                *p++ = s[i];
        }
    }
    *p++ = '"';
    c->top -= size - (p - head);
}

static void stringify_value(context *c, const cjson_value *v)
{
    switch (v->type)
    {
    case CJSON_NULL:
        memcpy(context_push(c, sizeof(char) * 4), "null", sizeof(char) * 4);
        break;
    case CJSON_TRUE:
        memcpy(context_push(c, sizeof(char) * 4), "true", sizeof(char) * 4);
        break;
    case CJSON_FALSE:
        memcpy(context_push(c, sizeof(char) * 5), "false", sizeof(char) * 5);
        break;
    case CJSON_NUMBER:
        c->top -= 32 - sprintf(context_push(c, 32), "%.17g", v->u.n);
        break;
    case CJSON_STRING:
        stringify_string(c, v->u.s.s, v->u.s.len);
        break;
    case CJSON_ARRAY:
        PUTC(c, '[');
        for (int i = 0; i < v->u.a.size; i++)
        {
            if (i > 0)
                PUTC(c, ',');
            stringify_value(c, &(v->u.a.a[i]));
        }
        PUTC(c, ']');
        break;
    case CJSON_OBJECT:
        PUTC(c, '{');
        for (int i = 0; i < v->u.o.size; i++)
        {
            if(i>0)
                PUTC(c, ',');
            stringify_string(c, v->u.o.m[i].key,v->u.o.m[i].len );
            PUTC(c, ':');
            stringify_value(c, &(v->u.o.m[i].v));
        }
        PUTC(c, '}');
        break;
    default:
    assert(0 &&"Invalid type");
    }
}

char *cjson_stringify(const cjson_value *v, size_t *length)
{
    assert(v != NULL);
    context c;
    c.capacity = CONTEXT_STACK_DEFAULT_CAPACITY;
    c.json = NULL;
    c.top = 0;
    c.stack = (char *)malloc(c.capacity);
    stringify_value(&c, v);
    if (length)
        *length = c.top;
    PUTC(&c, '\0');
    return c.stack;
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
    assert(v != NULL && v->type == CJSON_STRING);
    return v->u.s.s;
}

size_t cjson_get_string_length(cjson_value *v)
{
    assert(v != NULL && v->type == CJSON_STRING);
    return v->u.s.len;
}

void cjson_set_string(cjson_value *v, const char *str, size_t str_len)
{
    cjson_free(v);
    v->type = CJSON_STRING;
    v->u.s.len = str_len;
    if (str == NULL)
    {
        v->u.s.s = NULL;
        return;
    }
    memcpy((v->u.s.s = (char *)malloc(v->u.s.len + 1)), str, str_len);
    v->u.s.s[str_len] = '\0';
}

void cjson_set_array(cjson_value *v, size_t capacity)
{
    cjson_free(v);
    v->type = CJSON_ARRAY;
    v->u.a.capacity = capacity;
    v->u.a.size = 0;
    v->u.a.a = (capacity > 0) ? (cjson_value *)malloc(sizeof(cjson_value) * capacity) : NULL;
}

size_t cjson_get_array_size(cjson_value *v)
{
    assert(v != NULL && v->type == CJSON_ARRAY);
    return v->u.a.size;
}
size_t cjson_get_array_capacity(cjson_value *v)
{
    assert(v != NULL && v->type == CJSON_ARRAY);
    return v->u.a.capacity;
}

cjson_value *cjson_get_array_element(cjson_value *v, size_t index)
{
    assert(v != NULL && v->type == CJSON_ARRAY && index < v->u.a.size);
    return &(v->u.a.a[index]);
}

void cjson_set_object(cjson_value *v, size_t capacity)
{
    cjson_free(v);
    v->type = CJSON_OBJECT;
    v->u.o.capacity = capacity;
    v->u.o.size = 0;
    v->u.o.m = (capacity > 0) ? (cjson_member *)malloc(sizeof(cjson_member) * capacity) : NULL;
}

void cjson_free(cjson_value *v)
{
    assert(v != NULL);
    switch (v->type)
    {
    case CJSON_STRING:
        free(v->u.s.s);
        break;
    case CJSON_ARRAY:
        for (int i = 0; i < v->u.a.size; i++)
        {
            cjson_free(&(v->u.a.a[i]));
        }
        free(v->u.a.a);
        break;
    case CJSON_OBJECT:
        for (int i = 0; i < v->u.o.size; i++)
        {
            free(v->u.o.m[i].key);
            cjson_free(&(v->u.o.m[i].v));
        }
        free(v->u.o.m);
        break;
    default:
        break;
    }
    v->type = CJSON_NULL;
}