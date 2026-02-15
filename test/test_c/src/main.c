#include <stdio.h>
#include <string.h>

int main()
{
    int m, n, a[100], b[100], first, last;
    scanf("%d%d", &m, &n);
    memset(a, 0, sizeof(a));
    memset(b, 0, sizeof(a));

    printf("0.");
    for (int i = 1;; ++i) {
        if (m == 0) {
            // printf("整除");
            break;
        }
        if (a[m] != 0) {
            // printf("循环, m=%d, i=%d", m, i);
            // for (int j = 0; j < n; j++) {
            //     printf("a[%d]=%d\n", j, a[j]);
            //     printf("b[%d]=%d\n", j, b[j]);
            // }
            first = a[m];
            last  = i - 1;
            printf("\nfirst=%d, last=%d", first, last);
            break;
        }
        a[m] = i;
        b[i] = m;
        printf("%d", m * 10 / n);
        m = m * 10 % n;
    }
    return 0;
}