#include <stdio.h>
#include <stddef.h>   // offsetof

struct A { char c; int i; char d; };
struct B { int i; char c; char d; };
struct C { char c; double x; int i; };
struct D { double x; int i; char c; };
struct __attribute__((packed)) P { char c; int i; char d; };

int main(void) {
    printf("A: size=%zu, c=%zu, i=%zu, d=%zu\n",
           sizeof(struct A), offsetof(struct A, c), offsetof(struct A, i), offsetof(struct A, d));
    printf("B: size=%zu, i=%zu, c=%zu, d=%zu\n",
           sizeof(struct B), offsetof(struct B, i), offsetof(struct B, c), offsetof(struct B, d));
    printf("C: size=%zu, c=%zu, x=%zu, i=%zu\n",
           sizeof(struct C), offsetof(struct C, c), offsetof(struct C, x), offsetof(struct C, i));
    printf("D: size=%zu, x=%zu, i=%zu, c=%zu\n",
           sizeof(struct D), offsetof(struct D, x), offsetof(struct D, i), offsetof(struct D, c));
    printf("P: size=%zu, c=%zu, i=%zu, d=%zu\n",
           sizeof(struct P), offsetof(struct P, c), offsetof(struct P, i), offsetof(struct P, d));

    struct A arr[2];
    printf("A 배열: &arr[0]=%p, &arr[1]=%p\n", (void*)&arr[0], (void*)&arr[1]);
    return 0;
}