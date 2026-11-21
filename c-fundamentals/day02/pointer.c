#include <stdio.h>

void swap_value(int x, int y) {      // 값을 복사해서 받음
    int t = x; x = y; y = t;
}

void swap_pointer(int *x, int *y) {  // 주소를 받음
    int t = *x; *x = *y; *y = t;
}

int *bad_func(void) {
    int local = 42;
    return &local;                   // 지역 변수의 주소를 돌려줌
}

int main(void) {
    int a = 10;
    int *p = &a;

    // [실험 1] 포인터의 기본
    printf("a=%d, &a=%p\n", a, (void*)&a);
    printf("p=%p, *p=%d, &p=%p\n", (void*)p, *p, (void*)&p);
    *p = 20;
    printf("*p=20 이후 a=%d\n", a);

    // [실험 2] 크기
    printf("sizeof(int)=%zu, sizeof(int*)=%zu, sizeof(char*)=%zu, sizeof(double*)=%zu\n",
           sizeof(int), sizeof(int*), sizeof(char*), sizeof(double*));

    // [실험 3] swap
    int x = 1, y = 2;
    swap_value(x, y);
    printf("swap_value 후: x=%d, y=%d\n", x, y);
    swap_pointer(&x, &y);
    printf("swap_pointer 후: x=%d, y=%d\n", x, y);
    printf("마지막: &x=%p, &y=%p\n", (void*)&x, (void*)&y);  // 추가

    // [실험 4] 사라진 지역 변수의 주소
    int *q = bad_func();
    printf("q=%p\n", (void*)q);
    printf("*q=%d\n", *q);
    return 0;
}