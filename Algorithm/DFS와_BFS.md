# DFS와 BFS

## 한 줄 정의
- **DFS (깊이 우선 탐색)**: 한 길을 **끝까지** 파고든 뒤, 막히면 돌아와서 다음 길을 탐색
- **BFS (너비 우선 탐색)**: 시작점에서 **가까운 곳부터 한 층씩** 퍼져 나가며 탐색

```
       1
     /   \
    2     3
   / \   / \
  4   5 6   7

DFS: 1 → 2 → 4 → 5 → 3 → 6 → 7   (아래로 먼저)
BFS: 1 → 2 → 3 → 4 → 5 → 6 → 7   (옆으로 먼저)
```

## 비교
| | DFS | BFS |
|---|---|---|
| 순서 | 한 길을 끝까지 먼저 | 가까운 것부터 한 층씩 |
| 구현 | **재귀** 또는 스택 | **큐 (deque)** |
| 잘 맞는 문제 | **모든 경우의 수**, 연결된 덩어리 세기 | **최단 거리, 최소 횟수** |

- 문제에 **"최소", "최단", "가장 빠른"**이 나오고 이동 비용이 모두 같으면 → **BFS**

---

## DFS: 재귀 흐름 (타겟 넘버)
```python
def solution(numbers, target):
    def dfs(idx, total):
        if idx == len(numbers):                  # ① 종료 조건
            return 1 if total == target else 0
        plus = dfs(idx + 1, total + numbers[idx])   # ② 갈림길: + 길
        minus = dfs(idx + 1, total - numbers[idx])  #    - 길
        return plus + minus                      # ③ 결과 합치기
    return dfs(0, 0)
```

### 재귀 3요소
1. **종료 조건**: 언제 멈출까? (숫자를 다 썼을 때)
2. **갈림길**: 자기 자신을 다시 호출
3. **결과 합치기**: 두 갈래의 결과를 더해 위로 반환

### `return plus + minus`가 개수가 되는 이유
- 맨 아래 함수는 정답이면 1, 아니면 0을 돌려준다
- 부모는 두 자식의 값을 더해 위로 보고한다
- 맨 위 `dfs(0, 0)`에 모든 1이 모여 **전체 정답 개수**가 된다
- `answer = 0` + `nonlocal answer` + `answer += 1` 방식과 결과가 같다

```
맨 아래:  0   1   1   0
          \  /     \  /
중간:      1         1      ← 0+1, 1+0
            \       /
맨 위:          2           ← 1+1
```

### 콜 스택
- 함수를 부르면 그 함수가 값을 돌려줄 때까지 **지금 줄에서 멈춰서 기다린다**
- 함수 호출은 **콜 스택**에 쌓이고, 스택은 **LIFO**
- dfs(0) → dfs(1) → dfs(2)를 불렀다면 **dfs(2)가 가장 먼저 끝난다**

---

## DFS: 덩어리 세기 (네트워크)
```python
def solution(n, computers):
    visited = [False] * n
    def dfs(i):
        visited[i] = True
        for j in range(n):
            if computers[i][j] == 1 and not visited[j]:
                dfs(j)
    answer = 0
    for i in range(n):
        if not visited[i]:      # 아직 안 간 컴퓨터 = 새 네트워크
            answer += 1
            dfs(i)              # 그 네트워크 전체를 방문 표시
    return answer
```
- **visited**: 이미 방문한 곳을 다시 가거나 다시 세지 않게 막는다
- dfs = 한 덩어리를 전부 칠하는 붓, 바깥 for문 = 새 덩어리를 세는 카운터
- 섬 개수, 단지 번호 붙이기 등에도 그대로 쓰는 패턴

---

## BFS: 최단 거리 (게임 맵 최단거리)
```python
from collections import deque
def solution(maps):
    n, m = len(maps), len(maps[0])
    dist = [[0] * m for _ in range(n)]
    dist[0][0] = 1
    q = deque([(0, 0)])
    dx = [1, -1, 0, 0]
    dy = [0, 0, 1, -1]
    while q:
        x, y = q.popleft()
        for d in range(4):
            nx, ny = x + dx[d], y + dy[d]
            if 0 <= nx < n and 0 <= ny < m:
                if maps[nx][ny] == 1 and dist[nx][ny] == 0:
                    dist[nx][ny] = dist[x][y] + 1
                    q.append((nx, ny))
    return dist[n-1][m-1] if dist[n-1][m-1] else -1
```
- 큐(FIFO)라서 가까운 칸부터 처리 → **처음 도착한 거리 = 최단 거리**
- `dist`가 visited 역할도 한다 (0이 아니면 이미 방문)
- `dx`, `dy` = 상하좌우 이동 표
- DFS로 모든 경로를 보면 100×100 맵에서 시간초과

### BFS 기본 틀
```python
q = deque([시작]); 시작 방문 표시
while q:
    cur = q.popleft()
    for 다음 in 인접한 곳(cur):
        if 범위 안 and 갈 수 있음 and 아직 안 감:
            방문 표시(다음)      # 큐에 넣을 때 바로 표시
            q.append(다음)
```
- 리스트 `pop(0)`은 O(N) → BFS는 항상 `deque.popleft()` (O(1))

## 내 말로 설명하기
> DFS는 한 길을 끝까지 파고든 뒤 돌아오는 방식으로 재귀로 구현하며, 모든 경우의 수를 볼 때 좋다.
> BFS는 큐를 써서 가까운 곳부터 한 층씩 퍼지기 때문에 처음 도착한 거리가 최단 거리가 되어,
> 최단 거리 문제에 적합하다.
