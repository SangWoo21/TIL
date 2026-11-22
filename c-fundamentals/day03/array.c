#include <stdio.h>

void print_size(int arr[]) {              // 배열을 함수에 넘기면?
    printf("[함수 안] sizeof(arr)=%zu\n", sizeof(arr));
}

int main(void) {
    // [실험 1] 타입별 p+1
    int    i = 0;  int    *pi = &i;
    char   c = 0;  char   *pc = &c;
    double d = 0;  double *pd = &d;
    printf("int*   : %p -> %p\n", (void*)pi, (void*)(pi + 1));
    printf("char*  : %p -> %p\n", (void*)pc, (void*)(pc + 1));
    printf("double*: %p -> %p\n", (void*)pd, (void*)(pd + 1));

    // [실험 2] 배열과 포인터
    int arr[4] = {10, 20, 30, 40};
    int *p = arr;
    printf("arr=%p, &arr[0]=%p, &arr=%p\n", (void*)arr, (void*)&arr[0], (void*)&arr);
    printf("arr+1=%p, &arr+1=%p\n", (void*)(arr + 1), (void*)(&arr + 1));
    printf("*(p+2)=%d, p[2]=%d, 2[arr]=%d\n", *(p + 2), p[2], 2[arr]);

    // [실험 3] sizeof: main 안 vs 함수 안
    printf("[main 안] sizeof(arr)=%zu, sizeof(p)=%zu\n", sizeof(arr), sizeof(p));
    print_size(arr);

    // [실험 4] 어제의 선언
    int *a[4];
    int (*b)[4];
    int (*cc[3])[4];
    printf("sizeof(a)=%zu, sizeof(b)=%zu, sizeof(cc)=%zu\n", sizeof(a), sizeof(b), sizeof(cc));

    // [실험 5] 바이트 단위 오프셋 계산 (메모리 플래너의 기본)
    char buf[64];
    int *t = (int*)(buf + 16);            // buf의 16바이트 뒤를 int로 사용
    printf("buf=%p, t=%p, 차이=%td바이트\n", (void*)buf, (void*)t, (char*)t - buf);
    return 0;
}