#include <stdio.h>

int main(int argc, char const *argv[])
{
    int a = 4 & 3;
    printf("%d\n", a);

    // if ((1 << 0) & (1 << 1))
    // {
    //     printf("(1 << 0) & (1 << 1) = true.");
    // }

    // for (size_t i = 0; i < 255; i++)
    // {
    //     if (i)
    //     {
    //         printf("%d = true.\n", i);
    //     }
    // }

    return 0;
}
