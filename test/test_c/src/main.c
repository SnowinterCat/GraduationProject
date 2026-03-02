#include <stdio.h>

int MyStrcmp(char str1[], char str2[]);

int main()
{
    char aa[] = "1145140";
    char bb[] = "114514";
    printf("%s %s\n", aa, bb);
    printf("%d\n", MyStrcmp(aa, bb));
    return 0;
}
int MyStrcmp(char str1[], char str2[])
{
    for (; *str1 != '\0' && *str2 != '\0' && *str1 == *str2; ++str1, ++str2) {}
    return *str1 - *str2;
}