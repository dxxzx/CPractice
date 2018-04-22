#include <stdio.h>

#define loge(tag, fmt, ...) fprintf(stdout, strcat(tag, fmt), __VA_ARGS__)

void log(const char *tag, const char *fmt, ...)

int main(int argc, char** argv)
{
    loge("error", "\e[01;31m%s", "error");
    return 0;
}
