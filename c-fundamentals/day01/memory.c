//전역 변수 1개, static 지역 변수 1개
//지역 변수 2개 (main 안에 하나, 다른 함수 안에 하나)
//malloc으로 할당한 메모리 1개
//main 함수 자체의 주소
#include <stdio.h>
#include <stdlib.h>

int g_var = 1;                 // 전역 변수 → 데이터 영역

void other_func(void) {
    int other_local = 3;       // 다른 함수의 지역 변수 → 스택
    printf("other_local : %p\n", (void*)&other_local);
}

int main(void) {
    static int s_var = 2;      // static 지역 변수 → 데이터 영역
    int main_local = 4;        // main의 지역 변수 → 스택
    int *heap = malloc(sizeof(int));  // → 힙

    printf("main_local  : %p\n", (void*)&main_local);
    other_func();
    printf("heap        : %p\n", (void*)heap);
    printf("s_var       : %p\n", (void*)&s_var);
    printf("g_var       : %p\n", (void*)&g_var);
    printf("main 함수   : %p\n", (void*)main);

    free(heap);                // 빌린 창고는 반납
    return 0;
}