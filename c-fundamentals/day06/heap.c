#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void leak(void) {                    // [1] 메모리 누수
    int *p = malloc(100 * sizeof(int));
    p[0] = 1;
    // free(p); 를 잊음
}

void double_free(void) {             // [2] 이중 해제
    int *p = malloc(sizeof(int));
    free(p);
    free(p);
}

void use_after_free(void) {          // [3] 해제 후 사용
    int *p = malloc(sizeof(int));
    *p = 42;
    free(p);
    printf("해제 후 읽기: %d\n", *p);
}

void overflow(void) {                // [4] 범위 밖 쓰기
    char *s = malloc(8);
    strcpy(s, "hello world");        // 12바이트를 8바이트 공간에 씀
    printf("%s\n", s);
    free(s);
}

void correct(void) {                 // [0] 올바른 사용
    int *p = malloc(100 * sizeof(int));
    if (p == NULL) { printf("malloc 실패\n"); return; }
    p[0] = 1;
    free(p);
    p = NULL;                        // 해제 후 NULL로 바꿔 두기
}

int main(int argc, char *argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 0;
    switch (n) {
        case 0: correct(); break;
        case 1: leak(); break;
        case 2: double_free(); break;
        case 3: use_after_free(); break;
        case 4: overflow(); break;
    }
    printf("[%d] 정상 종료\n", n);
    return 0;
}