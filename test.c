#include "CJson.h"
#include <stdlib.h>
#include <stdio.h>
int main()
{
    char * json = "\"\\u0000\"";
    cjson_value v;
    cjson_init(&v);
    printf("%i\t" , cjson_parse(&v, json));
    printf("%s", v.u.s.s);
}