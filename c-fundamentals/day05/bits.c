#include <stdio.h>
#include <stdint.h>

void print_bin(const char *name, uint8_t v) {   // 8비트를 2진수로 출력
    printf("%-10s = ", name);
    for (int i = 7; i >= 0; i--) printf("%d", (v >> i) & 1);
    printf(" (0x%02X, %u)\n", v, v);
}

int main(void) {
    // [실험 1] 4가지 패턴
    uint8_t reg = 0;
    reg |= (1 << 3);            print_bin("3번 켜기", reg);
    reg |= (1 << 0) | (1 << 7); print_bin("0,7번 켜기", reg);
    reg &= ~(1 << 3);           print_bin("3번 끄기", reg);
    reg ^= 0xFF;                print_bin("전체 뒤집기", reg);
    printf("5번 비트 = %d\n", (reg >> 5) & 1);

    // [실험 2] 여러 비트를 한 번에 다루기 (필드)
    uint8_t ctrl = 0;
    uint8_t mode = 5;                          // 3비트 필드에 넣을 값
    ctrl = (ctrl & ~(0x7 << 2)) | (mode << 2); // 2~4번 비트에 mode 쓰기
    print_bin("ctrl", ctrl);
    printf("mode 다시 꺼내기 = %u\n", (ctrl >> 2) & 0x7);

    // [실험 3] 링 버퍼의 & (N-1)
    int N = 8;
    for (int i = 6; i <= 10; i++)
        printf("i=%d: i %% N = %d, i & (N-1) = %d\n", i, i % N, i & (N - 1));

    // [실험 4] 위험한 경우들
    int8_t  s = -8;
    uint8_t u = 0xF8;
    printf("s >> 1 = %d\n", s >> 1);
    printf("u >> 1 = %u\n", u >> 1);
    printf("1 << 31 = %d\n", 1 << 31);
    printf("1u << 31 = %u\n", 1u << 31);
    return 0;
}