# Day 06: malloc/free와 메모리 버그 (valgrind, AddressSanitizer)

## 목표
1. 힙에서 자주 생기는 버그 4가지를 일부러 만들어 보기
2. 그냥 실행했을 때 버그가 드러나는지 확인
3. valgrind와 AddressSanitizer(ASan)로 버그를 잡고, 두 도구를 비교

## 환경
- 보드: Jetson Xavier NX (ARM64, Ubuntu 20.04, gcc 9.4.0, glibc 2.31)
- 도구: Valgrind 3.15.0, gcc 내장 AddressSanitizer
- 컴파일:
  - 일반: `gcc -Wall -O0 -g heap.c -o heap`
  - ASan: `gcc -Wall -O0 -g -fsanitize=address heap.c -o heap_asan`

## 힙 버그 4가지
| 번호 | 버그 | 뜻 | 창고 비유 |
|---|---|---|---|
| 1 | 메모리 누수 | `free`를 안 함 | 빌린 창고를 반납하지 않음 |
| 2 | 이중 해제 | 같은 메모리를 두 번 `free` | 이미 반납한 창고를 또 반납 |
| 3 | 해제 후 사용 | `free` 후 다시 읽거나 씀 | 반납한 창고에 몰래 들어감 |
| 4 | 범위 밖 쓰기 | 빌린 크기보다 더 씀 | 옆 창고 벽을 뚫고 들어감 |

올바른 사용(0번): `malloc` 결과가 `NULL`인지 확인 → 사용 → `free` → 포인터를 `NULL`로 바꿔 두기

---

## 1단계: 그냥 실행
```
== 0
[0] 정상 종료
== 1
[1] 정상 종료
== 2
free(): double free detected in tcache 2
Aborted (core dumped)
== 3
해제 후 읽기: 0
[3] 정상 종료
== 4
hello world
[4] 정상 종료
```

### 분석
- **버그 4개 중 3개(1, 3, 4번)가 `정상 종료`됐다.** 눈으로만 보면 버그를 찾을 수 없다.
- **1번(누수)**: 반납을 안 해도 프로그램은 돌아간다. 오래 실행해야만 메모리가 쌓여 드러난다.
- **2번(이중 해제)**: glibc의 malloc이 방금 반납된 메모리 목록(tcache)을 확인해서 같은 주소가 또 들어오자 프로그램을 멈췄다.
- **3번(해제 후 사용)**: 42가 아니라 **0**이 나왔다. `free` 시 malloc이 반납 목록을 관리하는 정보를 그 메모리 안에 직접 적어서 42를 덮어썼기 때문이다. 결과는 랜덤이 아니라 **malloc 구현에 따라 정해진다.**
- **4번(범위 밖 쓰기)**: 8바이트 공간에 12바이트를 썼는데도 `hello world`가 끝까지 출력됐다. 넘친 4바이트가 **옆 메모리를 조용히 덮어썼다.** 가장 위험한 경우다.

---

## 2단계: valgrind

### 1번 누수 (`valgrind --leak-check=full ./heap 1`)
```
HEAP SUMMARY:
    in use at exit: 400 bytes in 1 blocks
  total heap usage: 2 allocs, 1 frees, 1,424 bytes allocated

400 bytes in 1 blocks are definitely lost in loss record 1 of 1
   at 0x4849D8C: malloc (...)
   by 0x10889B: leak (heap.c:6)
   by 0x108A7B: main (heap.c:43)
```
- 정확히 **400바이트**(int 4바이트 × 100)가 샜다고 알려 준다.
- **어디서 빌렸는지** 호출 순서대로 보여 준다: `heap.c:6`(malloc) ← `heap.c:43`(leak 호출)
- `2 allocs` 중 하나는 `printf`의 출력 버퍼(1,024바이트 = 1,424 - 400)이고, 이것은 정상적으로 반납됐다.

### 3번 해제 후 사용 (`valgrind ./heap 3`)
```
Invalid read of size 4
   at 0x108914: use_after_free (heap.c:21)     ← 읽은 곳
 Address 0x49fd040 is 0 bytes inside a block of size 4 free'd
   by 0x10890F: use_after_free (heap.c:20)     ← 반납한 곳
 Block was alloc'd at
   by 0x1088F7: use_after_free (heap.c:18)     ← 빌린 곳
해제 후 읽기: 42
```
- **그냥 실행하면 0, valgrind에서는 42가 나왔다.** valgrind는 자기만의 malloc을 쓰고, 반납된 메모리를 한동안 아무에게도 주지 않고 그대로 보관하기 때문이다.
- 같은 코드라도 **실행 환경에 따라 결과가 달라진다**는 직접 증거다. 그래도 valgrind는 `Invalid read`로 정확히 잡아냈다.

---

## 3단계: AddressSanitizer (ASan)

### 3번 해제 후 사용 (`./heap_asan 3`)
```
ERROR: AddressSanitizer: heap-use-after-free on address 0xffff8a5007b0
READ of size 4 at 0xffff8a5007b0 thread T0
    #0 in use_after_free heap.c:21
freed by thread T0 here:
    #1 in use_after_free heap.c:20
previously allocated by thread T0 here:
    #1 in use_after_free heap.c:18
```

### 4번 범위 밖 쓰기 (`./heap_asan 4`)
```
ERROR: AddressSanitizer: heap-buffer-overflow on address 0xffff9fc007b8
WRITE of size 12 at 0xffff9fc007b8 thread T0
    #1 in overflow heap.c:26
0xffff9fc007b8 is located 0 bytes to the right of 8-byte region [0xffff9fc007b0,0xffff9fc007b8)
allocated by thread T0 here:
    #1 in overflow heap.c:25

=>0x200ff3f800f0: fa fa fa fa fa fa 00[fa]fa fa ...
```
- 그냥 실행하면 `hello world`가 멀쩡히 나왔던 4번을 바로 잡았다.
- "12바이트를 썼고, 8바이트 공간의 **바로 오른쪽**을 침범했다"고 정확히 알려 준다.
- 첫 오류에서 `ABORTING`으로 멈춘다.

### ASan의 원리: Shadow memory
- 프로그램 메모리 **8바이트마다 1바이트짜리 "출입 허가표"**를 따로 만든다.
  - `00`: 8바이트 모두 사용 가능
  - `fa`: 빌린 공간 바깥의 경계 구역(redzone)
  - `fd`: 반납된 구역
- 4번의 `00[fa]`: 8바이트(`00`)를 빌렸는데 그 바로 뒤 경계 구역(`fa`)에 썼다는 뜻.
- 3번의 `[fd]`: 반납된 구역을 읽었다는 뜻.
- 컴파일할 때 메모리에 접근하는 모든 줄 앞에 허가표 확인 코드를 심어 두므로, valgrind처럼 명령어를 하나하나 가상 실행할 필요가 없어 빠르다.

---

## valgrind vs ASan
| | valgrind | ASan |
|---|---|---|
| 재컴파일 | **필요 없음** (기존 실행 파일에 바로 사용) | **필요함** (`-fsanitize=address`) |
| 동작 방식 | 실행하면서 명령어를 하나하나 가상으로 대신 실행 | 컴파일할 때 검사 코드를 심어 둠 |
| 속도 | 매우 느림 (보통 수십 배) | 비교적 빠름 (보통 2배 정도) |
| 오류 발견 시 | 알려 주고 계속 실행 | 첫 오류에서 멈춤 |
| 언제 쓰나 | 소스가 없거나 다시 빌드하기 어려울 때 | 테스트에서 자주 돌릴 때 |

(속도 배수는 프로그램에 따라 다르다.)

## 범위 밖 쓰기가 실제 서비스에서 일으키는 문제
1. **다른 데이터가 조용히 바뀜**: 관계없는 곳에서 이상한 값이 나온다.
2. **나중에 엉뚱한 곳에서 크래시**: malloc 관리 정보가 깨지면 한참 뒤 다른 `malloc`/`free`에서 죽는다. 원인과 증상이 멀리 떨어져 있어 찾기 어렵다.
3. **보안 취약점**: 공격자가 넘치는 데이터를 조작해 프로그램 동작을 바꿀 수 있다 (버퍼 오버플로 공격).

## 프로젝트와의 연결
- 프로젝트 A의 "24시간 구동 시 메모리 누수 여부" 검증에 valgrind(누수)와 ASan(메모리 오류)을 사용할 수 있다.
- 단, CUDA·TensorRT 코드는 GPU 메모리를 따로 관리하므로 이 도구들이 GPU 쪽 오류까지 잡지는 못한다. (GPU 메모리 검사는 NVIDIA의 별도 도구가 필요하며, Jetson 지원 여부는 확인 필요)

---

## 틀렸던 점 (기록)
| 처음 생각 | 실제 |
|---|---|
| 이중 해제를 malloc이 눈치채지 못한다 | glibc가 tcache를 확인해 **감지하고 프로그램을 멈춘다** |
| 해제 후 읽기 결과는 반환에 시간이 걸려서 랜덤이다 | 랜덤이 아니라 **malloc이 반납된 메모리에 관리 정보를 적어서** 덮어써졌다 (일반 실행 0, valgrind 42) |
| valgrind는 재컴파일이 필요하고, ASan은 필요 없다 | **반대다.** valgrind는 기존 실행 파일에 바로 쓰고, ASan은 `-fsanitize=address`로 재컴파일한다 |
| 범위 밖 쓰기로 쓴 값은 저장되지 않는다 | **저장된다.** 옆 메모리를 덮어써서 데이터 손상, 지연된 크래시, 보안 취약점이 생긴다 |

## 면접 대비 한 줄 요약
- "누수, 해제 후 사용, 범위 밖 쓰기는 대부분 바로 죽지 않고 정상처럼 보여서 도구로 잡아야 합니다. valgrind는 재컴파일 없이 실행 파일을 가상 실행하며 검사해 느리고, ASan은 컴파일 시 shadow memory 검사 코드를 심어 상대적으로 빠릅니다. 해제 후 사용은 할당기 구현에 따라 결과가 달라지는 정의되지 않은 동작이라, 테스트에서 정상이어도 실제 환경에서 데이터가 깨질 수 있습니다."

## 남은 실험
- [ ] `valgrind ./heap 4`로 범위 밖 쓰기를 valgrind가 잡는지 확인
- [ ] `./heap_asan 1`로 ASan도 누수를 알려 주는지 확인 (LeakSanitizer)

## 다음
Day 07: 미니 과제 - 직접 만드는 메모리 풀 할당기
