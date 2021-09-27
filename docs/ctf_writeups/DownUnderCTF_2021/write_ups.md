# **Rev Challenges Write Up FwordCTF 2021 - (24-26/09/2021)**
# Flag Loader 218 points

## Description

What more could you ask for than a program that loads the flag for you? Just answer a few simple questions and the flag is yours!

Author: joseph#8210
 `nc pwn-2021.duc.tf 31919`
 
 File: [flag_loader](./flag_loader)
 
 ## Overview 
 
 - A program with three layers check conditions (random math expression). Just input the correct answer and pass all checks, server will print out the flag.
 - Specially, the input will be calculated and then multiply together, The program will call sleep for the number of seconds equals to the final result.
 - One more thing the author was made this chall a little more difficult that the total time live of the program is not more than 1 minute.
 
 ## Analyze
 Pseudocode of `main` function:
 
 ```c
 int __cdecl main(int argc, const char **argv, const char **envp)
{
  int v4; // [rsp+Ch] [rbp-124h]
  int v5; // [rsp+10h] [rbp-120h]
  int v6; // [rsp+14h] [rbp-11Ch]
  FILE *stream; // [rsp+18h] [rbp-118h]
  char s[264]; // [rsp+20h] [rbp-110h] BYREF
  unsigned __int64 v9; // [rsp+128h] [rbp-8h]

  v9 = __readfsqword(0x28u);
  init();
  v4 = check1();
  v5 = check2();
  v6 = check3();
  puts("You've passed all the checks! Please be patient as the flag loads.");
  puts("Loading flag... (this may or may not take a while)");
  sleep(v6 * v5 * v4);
  stream = fopen("flag.txt", "r");
  fgets(s, 255, stream);
  printf("%s", s);
  return 0;
}
 ```
 
 The code was easy to understand and the same with the overview, let inspect to the check fuctions:
 
 ### First check
 
 Pseudocode:
 ```c
 __int64 check1()
{
  char v1; // [rsp+Ah] [rbp-16h]
  unsigned __int8 v2; // [rsp+Bh] [rbp-15h]
  int i; // [rsp+Ch] [rbp-14h]
  char buf[6]; // [rsp+12h] [rbp-Eh] BYREF
  unsigned __int64 v5; // [rsp+18h] [rbp-8h]

  v5 = __readfsqword(0x28u);
  v1 = 0;
  v2 = 1;
  printf("Give me five letters: ");
  read(0, buf, 5uLL);
  for ( i = 0; i <= 4; ++i )
  {
    v1 += X[i] ^ buf[i];
    v2 *= ((_BYTE)i + 1) * buf[i];
  }
  if ( v1 || !v2 )
    die();
  return v2;
}
 ```
 
 ```c
 void __noreturn die()
{
  puts("You failed the check! No flag for you :(");
  exit(-1);
}
 ```
 
 To pass the check 1 function, the input string have to meet the conditions, `v1 == 0` and `v2 != 0`:
 - `v1` was caculated by the `sum` of `xor` input string characters with `X` characters has value `DUCTF`
 -  `v2` was caculated by the `multiply` the index add 1 with input string chacracters respectively.
 
In first time, I was tried the of course value `DUCTF` but it not pass the check, because the `v2` is just a byte type so the multiply operation was overflow byte size and the result of 8 bit was equal `0`.

We can manually recaculated the input string but I was lazy and then I used `z3`:

```python
def solve_check1():
    five_chars = [BitVec(f'char_{i}', 16) for i in range(5)]

    arr = [68, 85, 67, 84, 70]

    sum = 0
    for i in range(len(arr)):
        sum += arr[i] ^ five_chars[i]

    S = Solver()

    S.add(sum & 0xff == 0)

    for i in range(len(arr)):
        S.add(five_chars[i] >= 33, five_chars[i] <= 126)

    assert sat == S.check()

    ans = S.model()

    print(ans)

    res = []
    for i in five_chars:
        print(chr(ans[i].as_long()), end='')
        res.append(ans[i].as_long())

    return res
```

Then the input for the check 1: `%dgZz`


### Second Check
Pseudocode:

```c
__int64 check2()
{
  unsigned int v1; // [rsp+Ch] [rbp-14h] BYREF
  unsigned int v2; // [rsp+10h] [rbp-10h] BYREF
  unsigned int v3; // [rsp+14h] [rbp-Ch]
  unsigned __int64 v4; // [rsp+18h] [rbp-8h]

  v4 = __readfsqword(0x28u);
  LOWORD(v3) = rand();
  v3 = (unsigned __int16)v3;
  printf("Solve this: x + y = %d\n", (unsigned __int16)v3);
  __isoc99_scanf("%u %u", &v1, &v2);
  if ( !v1 || !v2 || v1 <= v3 || v2 <= v3 )
    die();
  if ( v2 + v1 != v3 || (unsigned __int16)(v1 * v2) <= 0x3Bu )
    die();
  return (unsigned __int16)(v1 * v2);
}
```

This is just `x+y = random number` expression and some more condition with x,y, I used `z3` for solving it:

```python
def solve_check2(sum, v1):
    x, y = BitVecs('x y', 33)
    S = Solver()
    v1 = IntVal(v1)
    v1 = Int2BV(v1, 33)
    S.add(x > sum, y > sum, (x+y) & 0xffffffff ==
          sum, (x*y) & 0xffff > 0x3b, (x*y*v1) & 0x3fffff == 0)

    assert S.check() == sat
    ans = S.model()
    print(ans)
    print(len(bin(ans[x].as_long())[2:]))
    print(len(bin(ans[y].as_long())[2:]))
    return ans[x].as_long() * ans[y].as_long()
```

### Final Check

Pseudocode:

```c
__int64 check3()
{
  unsigned int v1; // [rsp+0h] [rbp-20h] BYREF
  unsigned int v2; // [rsp+4h] [rbp-1Ch] BYREF
  unsigned int v3; // [rsp+8h] [rbp-18h] BYREF
  unsigned int v4; // [rsp+Ch] [rbp-14h] BYREF
  unsigned int v5; // [rsp+10h] [rbp-10h] BYREF
  int v6; // [rsp+14h] [rbp-Ch]
  unsigned __int64 v7; // [rsp+18h] [rbp-8h]

  v7 = __readfsqword(0x28u);
  LOWORD(v6) = rand();
  v6 = (unsigned __int16)v6;
  printf("Now solve this: x1 + x2 + x3 + x4 + x5 = %d\n", (unsigned __int16)v6);
  __isoc99_scanf("%u %u %u %u %u", &v1, &v2, &v3, &v4, &v5);
  if ( !v1 || !v2 || !v3 || !v4 || !v5 )
    die();
  if ( v1 >= v2 || v2 >= v3 || v3 >= v4 || v4 >= v5 )
    die();
  if ( v5 + v4 + v3 + v2 + v1 != v6 || (unsigned __int16)((v3 - v2) * (v5 - v4)) <= 0x3Bu )
    die();
  return (unsigned __int16)((v3 - v2) * (v5 - v4));
}
```
This is the same with the previous check. Five inputs instead of two inputs. I also code a python script with z3 to solve this check. But the sleep time got a huge number of seconds.

## Solution

- With res1,res2,res3 was return values of check1, check2, check3 functions respectively.
- The first solution, I was thought of `multiply overflow` (res1*res2*res3) 32 bit that make the sleep time equal to zero. But in each check , particulatly in check2 and check3, the conditions check the `16 first bits` of res2,res3 greater than 0x3B, so we can make the res2 == 0 or res3 == 0.
- Inspecting more for multiply operation, So we can make the output of the multiply overflow equal zeros through sum of k first bits of each operand. That mean, with x,y,z were the number of first bits in res1,res2,res3. So, `x+y+z >= 32` to make overflow multiply equal to zeros.
- res1 = `3921153664`, contains 7 first bits `0`
- res2 will be maximum contains 15 first bits `0` to meet the condition `16 first bits of res2 > 0x3b`, then res1*res2 can be 22 first bits 0 (`(x*y*v1) & 0x3fffff == 0`)
- So res3 will easy to contain >= 10 bits `0` and meet the condition `16 first bits of res3 > 0x3b` (`(cal*Int2BV(c, 32)) & 0xffffffff == 0`) => the sleep overflow equals to zero.

Check3 solve python script:

```python
def solve_check3(sum, mul):
    x1, x2, x3, x4, x5 = BitVecs('x1 x2 x3 x4 x5', 32)

    S = Solver()
    S.add(ULT(x1, x2))
    S.add(ULT(x2, x3))
    S.add(ULT(x3, x4))
    S.add(ULT(x4, x5))
    S.add((x1+x2+x3+x4+x5) & 0xffffffff ==
          sum)
    cal = (x3-x2)*(x5-x4)
    c = IntVal(mul)
    S.add(UGT(cal & 0xffff, 0x3b), (cal*Int2BV(c, 32)) & 0xffffffff == 0)
    assert S.check() == sat
    ans = S.model()
    print(ans)
    res = [ans[x1].as_long(), ans[x2].as_long(), ans[x3].as_long(),
           ans[x4].as_long(), ans[x5].as_long()]
    ret = ((res[2]-res[1])*(res[4]-res[3])) & 0xffffffff
    print(ret)
    print((ret*mul) & 0xffffffff)
  ```
 
## Final script solution

```python
from z3 import *


def solve_check1():
    five_chars = [BitVec(f'char_{i}', 16) for i in range(5)]

    arr = [68, 85, 67, 84, 70]

    sum = 0
    for i in range(len(arr)):
        sum += arr[i] ^ five_chars[i]

    S = Solver()

    S.add(sum & 0xff == 0)

    for i in range(len(arr)):
        S.add(five_chars[i] >= 33, five_chars[i] <= 126)

    assert sat == S.check()

    ans = S.model()

    print(ans)

    res = []
    for i in five_chars:
        print(chr(ans[i].as_long()), end='')
        res.append(ans[i].as_long())

    return res


def solve_check2(sum, v1):
    x, y = BitVecs('x y', 33)
    S = Solver()
    v1 = IntVal(v1)
    v1 = Int2BV(v1, 33)
    S.add(x > sum, y > sum, (x+y) & 0xffffffff ==
          sum, (x*y) & 0xffff > 0x3b, (x*y*v1) & 0x3fffff == 0)

    assert S.check() == sat
    ans = S.model()
    print(ans)
    print(len(bin(ans[x].as_long())[2:]))
    print(len(bin(ans[y].as_long())[2:]))
    return ans[x].as_long() * ans[y].as_long()


def solve_check3(sum, mul):
    x1, x2, x3, x4, x5 = BitVecs('x1 x2 x3 x4 x5', 32)

    S = Solver()
    S.add(ULT(x1, x2))
    S.add(ULT(x2, x3))
    S.add(ULT(x3, x4))
    S.add(ULT(x4, x5))
    S.add((x1+x2+x3+x4+x5) & 0xffffffff ==
          sum)
    cal = (x3-x2)*(x5-x4)
    c = IntVal(mul)
    S.add(UGT(cal & 0xffff, 0x3b), (cal*Int2BV(c, 32)) & 0xffffffff == 0)
    assert S.check() == sat
    ans = S.model()
    print(ans)
    res = [ans[x1].as_long(), ans[x2].as_long(), ans[x3].as_long(),
           ans[x4].as_long(), ans[x5].as_long()]
    ret = ((res[2]-res[1])*(res[4]-res[3])) & 0xffffffff
    print(ret)
    print((ret*mul) & 0xffffffff)


res = solve_check1()

v1 = 1
for i in range(len(res)):
    v1 *= res[i]*(i+1)
print()
print(v1 & 0xffffffff)

sum = int(input('sum2: '))
v2 = solve_check2(sum, v1)


mul = (v1*v2) & 0xffffffff
print(mul)
sum = int(input('sum3: '))

solve_check3(sum, mul)

```

## Result

![image](https://user-images.githubusercontent.com/31529599/134936788-a6bd7c97-58a8-4ffb-9083-b8465ea58989.png)

![image](https://user-images.githubusercontent.com/31529599/134936825-4a597ecc-d48f-450b-a693-eba458bfbc0d.png)

Flag: `DUCTF{y0u_sur3_kn0w_y0ur_int3gr4l_d4t4_typ3s!}`

# Connect the dot 416 points

## Description

Can you see the forest for the trees?

Author: joseph#8210

File: [connect_the_dot](./connect_the_dots)

## Overview

- This is just a maze challenges, colve maze and retrieve flag.

## Analysis

Pseudocode `main`:

```c
__int64 __fastcall main(int a1, char **a2, char **a3)
{
  int i; // [rsp+0h] [rbp-E10h]
  int v5; // [rsp+4h] [rbp-E0Ch]
  int j; // [rsp+8h] [rbp-E08h]
  char buf[3576]; // [rsp+10h] [rbp-E00h] BYREF
  unsigned __int64 v8; // [rsp+E08h] [rbp-8h]

  v8 = __readfsqword(0x28u);
  buf[(int)(read(0, buf, 0xDEFuLL) - 1)] = 0;
  sub_55E550A901E0(buf);
  puts("yes :>");
  for ( i = 0; i <= 43; ++i )
  {
    v5 = 1;
    for ( j = 0; j <= 80; ++j )
      v5 = v5 * buf[81 * i + j] % 255;
    putchar(v5 ^ byte_55E550A968C0[i]);
  }
  puts(&byte_55E550A91057);
  return 0LL;
}
```

Just see that the output of the program is the flag. But it depends on the input string.

Let inspect the maze function:


Maze function pseudocode:

```c
__int64 __fastcall sub_55E550A901E0(_BYTE *a1)
{
  _BYTE *v1; // rax
  int v2; // eax
  int v5; // [rsp+14h] [rbp-4Ch]
  int v6; // [rsp+18h] [rbp-48h]
  int v7; // [rsp+1Ch] [rbp-44h]
  int v8; // [rsp+24h] [rbp-3Ch]
  int v9[10]; // [rsp+30h] [rbp-30h] BYREF
  unsigned __int64 v10; // [rsp+58h] [rbp-8h]

  v10 = __readfsqword(0x28u);
  v5 = 0;
  v6 = 0;
  v9[0] = -1;
  v9[1] = -1;
  v9[2] = -1;
  v9[3] = -1;
  v9[4] = -1;
  v9[5] = -1;
  v9[6] = -1;
  v9[7] = -1;
  v7 = 0;
  while ( 2 )
  {
    if ( !*a1 )
      no_and_exit();
    v1 = a1++;
    switch ( *v1 )
    {
      case 'h':
        if ( (dword_55E550A93080[v6] & 1) != 0 )
          no_and_exit();
        --v6;
        continue;
      case 'j':
        if ( (dword_55E550A93080[v6] & 2) != 0 )
          no_and_exit();
        v6 += 60;
        continue;
      case 'k':
        if ( (dword_55E550A93080[v6] & 8) != 0 )
          no_and_exit();
        v6 -= 60;
        continue;
      case 'l':
        if ( (dword_55E550A93080[v6] & 4) != 0 )
          no_and_exit();
        ++v6;
        continue;
      case 'x':
        if ( (dword_55E550A93080[v6] & 0x80) == 0 )
          no_and_exit();
        v8 = (dword_55E550A93080[v6] >> 4) & 7;
        if ( (int)sub_55E550A90196((__int64)v9, 8, v8) >= 0 )
          no_and_exit();
        v2 = v7++;
        v9[v2] = v8;
        v5 = (unsigned __int8)dword_55E550A93060[v8] ^ (dword_55E550A93060[v8] >> 8) & v5;
        if ( v7 != 8 )
          continue;
        if ( v5 != 0xFF )
          no_and_exit();
        return 1LL;
      default:
        no_and_exit();
    }
  }
}
```

We just need to solve the maze and escape the maze fucntion to get flag. If the move wrong the program will exit.

Maze info:
- `h` to move left
- `k` to move up
- `j` to move down
- `l` to move left
- `x` to check destination valid

To make the function return `1`. we have to reach the 8 correct destination in the correct order. In the `x` case. The program will check valid destination and `v5&0xff` to check whether the reaching order was correct when reach all 8 destination.

## Solution

- A absolute solution for this chall is find out the correct order of destination and reach them one by one

### find order destination

From array `dword_55E550A93060` and condition `v5 != 0xff`. we can brute force the order

Python script:

```python
import random

dword_4060 = [65407, 60999, 31620, 5074, 27425, 53858, 20942, 3721]

arr_v8 = [0,1,2,3,4,5,6]


arr_su = [0, 1, 2, 3, 4, 5, 6, 7]
res = 0
count = 5
while True:
    res = 0
    for i in arr_su:
        res = dword_4060[i] ^ (dword_4060[i] >> 8) & res
    res = res & 0xff
    if res == 0xff:
        print(arr_su)
        break
    random.shuffle(arr_su)


# arr_destination = [6, 3, 7, 5, 1, 4, 0, 2]

```

Run the script and the result was: `[6, 3, 7, 5, 1, 4, 0, 2]`, So this is the order we have to reach destinations.

Now, solve this challenges with bfs algorithm

```python
def bfs(x_first, y_first, x_des, y_des, can_go):
    save = [['' for i in range(60)] for j in range(60)]
    best = [[[] for i in range(60)] for j in range(60)]
    que = []
    i = 0
    que.append([x_first, y_first])
    save[x_first][y_first] = '.'
    while i < len(que):
        x, y = que[i][0], que[i][1]
        if x == x_des and y == y_des:
            path = ''
            while x != x_first or y != y_first:
                path += save[x][y]
                x, y = best[x][y]
            return path[::-1]
            break
        for go in can_go[x][y]:
            if save[go[0]][go[1]] == '':
                que.append([go[0], go[1]])
                save[go[0]][go[1]] = go[2]
                best[go[0]][go[1]] = [x, y]
        i += 1

arr_maze = [[11, 10, 12, 9, 10, 10, 8, 8, 10, 14, 9, 8, 10, 10, 12, 11, 8, 10, 12, 9, 10, 12, 9, 10, 10, 10, 12, 11, 10, 10, 10, 8, 12, 9, 8, 14, 9, 10, 12, 9, 8, 12, 11, 10, 10, 10, 8, 12, 11, 8, 10, 10, 8, 14, 9, 12, 9, 8, 12, 13], [13, 9, 6, 5, 9, 12, 5, 7, 9, 12, 5, 5, 9, 12, 3, 10, 6, 9, 6, 5, 9, 6, 3, 12, 9, 14, 3, 10, 10, 10, 12, 7, 1, 6, 3, 12, 3, 12, 5, 5, 5, 3, 10, 10, 10, 10, 6, 3, 12, 5, 9, 12, 7, 9, 6, 5, 5, 5, 3, 4], [5, 5, 11, 2, 6, 5, 3, 10, 6, 5, 5, 3, 6, 3, 10, 10, 12, 5, 9, 6, 3, 12, 9, 6, 1, 10, 10, 10, 8, 14, 3, 12, 3, 12, 13, 5, 9, 6, 3, 6, 5, 9, 10, 10, 10, 10, 12, 9, 6, 5, 5, 3, 10, 2, 12, 3, 6, 5, 9, 6], [5, 3, 12, 11, 12, 1, 14, 9, 12, 3, 6, 9, 12, 9, 10, 12, 5, 3, 6, 13, 9, 6, 5, 11, 2, 10, 12, 13, 3, 10, 10, 6, 11, 2, 6, 5, 3, 10, 12, 9, 6, 5, 11, 12, 9, 10, 6, 5, 11, 0, 2, 14, 9, 12, 3, 10, 14, 5, 5, 13], [1, 12, 3, 12, 3, 6, 9, 6, 3, 10, 12, 5, 3, 6, 13, 3, 6, 9, 10, 6, 5, 11, 2, 10, 12, 13, 3, 2, 10, 10, 12, 9, 10, 10, 12, 3, 8, 10, 6, 3, 12, 3, 8, 6, 3, 10, 12, 3, 12, 5, 9, 10, 6, 3, 10, 10, 10, 6, 5, 5], [5, 3, 12, 3, 10, 12, 3, 12, 9, 10, 6, 1, 12, 11, 2, 8, 12, 1, 10, 12, 3, 12, 9, 10, 6, 5, 9, 8, 8, 14, 5, 1, 10, 12, 3, 12, 3, 10, 12, 13, 5, 9, 6, 9, 10, 12, 1, 12, 5, 7, 5, 9, 14, 9, 8, 14, 9, 10, 6, 5], [1, 12, 3, 10, 12, 5, 9, 6, 3, 12, 11, 6, 5, 9, 12, 7, 1, 6, 9, 6, 9, 6, 5, 9, 14, 1, 6, 5, 5, 9, 6, 3, 12, 3, 12, 5, 11, 10, 2, 4, 5, 3, 12, 5, 11, 2, 6, 7, 5, 9, 6, 1, 10, 6, 5, 9, 2, 10, 10, 6], [5, 5, 9, 12, 7, 5, 3, 10, 12, 3, 10, 10, 6, 5, 3, 12, 5, 13, 5, 9, 6, 9, 6, 3, 8, 6, 9, 6, 7, 3, 10, 12, 3, 12, 5, 3, 12, 9, 12, 5, 3, 12, 5, 3, 10, 10, 10, 10, 6, 3, 12, 5, 9, 12, 3, 6, 9, 10, 10, 12], [5, 5, 5, 3, 10, 6, 9, 10, 6, 9, 10, 10, 10, 6, 9, 6, 5, 5, 5, 3, 12, 3, 10, 12, 3, 12, 3, 10, 12, 9, 10, 6, 9, 6, 5, 9, 2, 6, 5, 3, 12, 5, 5, 9, 10, 12, 9, 10, 10, 8, 6, 3, 6, 5, 9, 10, 6, 9, 12, 5], [5, 5, 5, 9, 10, 12, 5, 9, 10, 4, 9, 10, 12, 13, 3, 12, 3, 6, 1, 12, 3, 10, 10, 6, 13, 5, 11, 12, 5, 3, 10, 12, 7, 9, 4, 7, 9, 12, 3, 12, 7, 5, 5, 3, 14, 1, 6, 9, 10, 6, 9, 12, 13, 5, 3, 12, 11, 6, 5, 5], [5, 5, 3, 6, 13, 3, 6, 5, 13, 3, 6, 13, 5, 3, 10, 2, 10, 10, 6, 3, 10, 10, 12, 11, 4, 5, 9, 4, 1, 10, 14, 3, 12, 5, 7, 9, 6, 5, 13, 3, 10, 6, 3, 10, 12, 3, 14, 3, 10, 12, 5, 5, 3, 4, 9, 2, 10, 8, 6, 5], [7, 5, 9, 10, 2, 10, 12, 3, 4, 11, 8, 4, 3, 10, 10, 140, 11, 10, 10, 12, 9, 12, 1, 10, 6, 5, 5, 5, 3, 10, 12, 9, 6, 3, 10, 2, 14, 5, 1, 10, 10, 8, 10, 14, 3, 10, 10, 10, 12, 3, 4, 3, 12, 5, 5, 9, 12, 7, 9, 4], [9, 6, 5, 13, 9, 10, 4, 13, 5, 9, 6, 5, 11, 10, 12, 3, 10, 10, 12, 3, 4, 5, 5, 9, 10, 6, 5, 5, 9, 10, 4, 3, 12, 9, 10, 12, 9, 6, 5, 13, 9, 4, 9, 10, 10, 10, 8, 12, 3, 12, 3, 14, 5, 5, 5, 5, 3, 10, 6, 5], [5, 9, 6, 5, 3, 12, 3, 6, 3, 6, 13, 3, 10, 12, 1, 8, 10, 12, 3, 12, 7, 3, 6, 5, 11, 8, 6, 5, 5, 11, 4, 13, 3, 6, 13, 3, 6, 13, 3, 4, 5, 5, 3, 8, 10, 14, 5, 3, 14, 5, 9, 10, 6, 7, 5, 5, 11, 12, 9, 6], [1, 6, 11, 0, 12, 3, 10, 8, 12, 13, 1, 10, 12, 5, 5, 5, 11, 6, 9, 6, 9, 10, 10, 6, 9, 6, 13, 5, 3, 12, 3, 2, 10, 10, 2, 10, 12, 3, 10, 6, 5, 7, 9, 6, 9, 12, 5, 9, 10, 6, 1, 12, 9, 10, 6, 3, 12, 3, 4, 13], [3, 10, 12, 5, 3, 14, 9, 6, 3, 4, 5, 13, 3, 6, 5, 5, 9, 8, 6, 9, 6, 9, 10, 10, 6, 9, 6, 5, 9, 6, 9, 10, 10, 8, 10, 14, 3, 10, 10, 10, 4, 9, 6, 13, 5, 1, 6, 5, 9, 10, 6, 5, 5, 11, 8, 12, 3, 12, 3, 6], [13, 9, 6, 3, 10, 12, 5, 9, 14, 5, 5, 3, 10, 10, 6, 5, 5, 7, 9, 6, 9, 4, 9, 14, 9, 2, 10, 6, 5, 11, 2, 12, 11, 6, 9, 10, 10, 10, 10, 10, 6, 3, 12, 5, 5, 5, 9, 6, 3, 12, 9, 6, 5, 9, 6, 3, 12, 5, 9, 12], [5, 5, 11, 8, 12, 5, 5, 1, 10, 6, 3, 10, 10, 10, 12, 5, 3, 10, 6, 13, 5, 5, 3, 12, 5, 9, 10, 12, 3, 10, 12, 5, 9, 12, 5, 11, 10, 8, 12, 11, 8, 14, 5, 5, 5, 7, 5, 9, 10, 6, 3, 10, 2, 6, 9, 14, 5, 3, 6, 5], [1, 2, 10, 6, 5, 5, 5, 3, 10, 12, 9, 14, 9, 10, 6, 1, 10, 12, 9, 2, 6, 5, 9, 6, 3, 6, 9, 6, 13, 9, 6, 5, 5, 3, 2, 10, 8, 6, 3, 10, 4, 9, 6, 5, 3, 12, 5, 3, 12, 9, 10, 12, 9, 12, 3, 12, 5, 9, 14, 5], [5, 13, 9, 12, 5, 5, 5, 9, 12, 5, 3, 12, 3, 10, 10, 6, 13, 5, 3, 12, 9, 6, 3, 10, 10, 12, 5, 9, 6, 5, 9, 2, 6, 9, 10, 12, 7, 9, 12, 9, 6, 3, 10, 2, 12, 3, 6, 11, 4, 3, 14, 5, 5, 3, 12, 3, 2, 6, 9, 4], [3, 4, 5, 5, 3, 6, 3, 6, 5, 3, 10, 4, 9, 10, 8, 14, 1, 6, 13, 5, 3, 10, 12, 9, 12, 5, 3, 4, 9, 6, 3, 10, 12, 3, 12, 147, 10, 6, 5, 7, 9, 10, 8, 12, 3, 10, 10, 12, 3, 8, 10, 6, 5, 13, 5, 9, 10, 10, 6, 5], [9, 6, 5, 3, 8, 10, 10, 12, 5, 11, 12, 5, 3, 14, 5, 9, 6, 9, 4, 3, 12, 13, 3, 6, 5, 5, 9, 6, 3, 12, 9, 12, 5, 13, 5, 9, 10, 14, 5, 9, 6, 9, 6, 5, 9, 12, 13, 5, 11, 4, 9, 10, 4, 3, 2, 6, 9, 12, 9, 6], [3, 12, 3, 12, 7, 9, 12, 3, 6, 9, 6, 3, 10, 8, 6, 3, 12, 5, 3, 12, 5, 1, 12, 9, 6, 5, 3, 10, 12, 3, 6, 5, 3, 6, 5, 1, 10, 12, 3, 6, 13, 3, 12, 5, 5, 3, 6, 3, 12, 7, 3, 12, 7, 9, 10, 12, 7, 1, 6, 13], [13, 5, 13, 3, 10, 6, 5, 13, 9, 2, 8, 14, 9, 6, 9, 10, 6, 3, 12, 5, 3, 4, 7, 5, 9, 2, 10, 10, 4, 9, 8, 6, 9, 10, 6, 3, 12, 3, 12, 9, 2, 12, 5, 3, 6, 9, 10, 12, 5, 9, 10, 6, 9, 2, 14, 1, 12, 3, 10, 4], [5, 3, 2, 12, 9, 12, 5, 5, 1, 14, 3, 10, 2, 12, 3, 12, 11, 10, 164, 1, 12, 5, 9, 6, 5, 9, 10, 12, 7, 5, 7, 9, 6, 9, 10, 10, 6, 13, 3, 2, 14, 5, 5, 9, 12, 5, 13, 3, 6, 5, 9, 10, 6, 9, 12, 7, 5, 11, 12, 5], [1, 8, 14, 3, 6, 5, 5, 3, 6, 9, 12, 9, 12, 5, 13, 3, 10, 12, 7, 5, 7, 5, 3, 12, 7, 3, 12, 3, 12, 3, 12, 3, 12, 1, 10, 10, 10, 2, 10, 14, 9, 6, 3, 6, 3, 6, 1, 8, 10, 6, 3, 12, 9, 6, 3, 10, 6, 9, 6, 5], [7, 5, 9, 10, 12, 5, 3, 10, 10, 6, 3, 6, 5, 3, 4, 11, 12, 3, 12, 5, 9, 6, 13, 3, 8, 14, 1, 14, 3, 12, 3, 12, 5, 7, 9, 10, 10, 12, 9, 12, 5, 9, 10, 8, 12, 13, 5, 3, 12, 9, 10, 6, 5, 9, 10, 10, 8, 4, 9, 6], [9, 6, 3, 12, 3, 6, 9, 12, 9, 10, 10, 10, 6, 13, 3, 10, 4, 9, 6, 3, 2, 12, 1, 12, 3, 10, 6, 9, 10, 2, 12, 5, 3, 12, 5, 9, 14, 3, 6, 5, 3, 2, 14, 5, 5, 1, 6, 13, 3, 2, 10, 14, 5, 3, 14, 9, 6, 5, 5, 13], [1, 10, 12, 5, 11, 10, 4, 5, 3, 10, 12, 11, 8, 2, 10, 10, 6, 3, 10, 12, 13, 5, 5, 5, 11, 8, 14, 5, 13, 9, 6, 1, 12, 3, 6, 5, 9, 10, 12, 5, 9, 10, 10, 6, 5, 3, 10, 2, 12, 9, 10, 10, 6, 9, 188, 5, 11, 4, 5, 5], [1, 14, 3, 4, 9, 10, 6, 5, 9, 14, 3, 12, 3, 10, 10, 12, 11, 12, 9, 6, 3, 2, 6, 3, 12, 5, 9, 6, 5, 3, 12, 5, 5, 11, 10, 2, 2, 14, 5, 5, 5, 9, 12, 13, 3, 12, 11, 10, 6, 5, 9, 10, 10, 4, 5, 3, 12, 5, 3, 4], [7, 9, 12, 3, 6, 11, 12, 5, 5, 9, 12, 5, 13, 9, 10, 6, 9, 6, 3, 12, 9, 10, 10, 12, 5, 5, 3, 12, 1, 10, 6, 5, 3, 10, 10, 10, 10, 12, 5, 3, 6, 5, 3, 2, 12, 5, 9, 10, 10, 6, 1, 10, 14, 5, 3, 10, 4, 3, 14, 5], [9, 6, 3, 12, 9, 8, 6, 3, 4, 5, 5, 5, 5, 5, 11, 8, 2, 8, 14, 3, 6, 11, 12, 5, 3, 0, 14, 5, 3, 10, 12, 5, 9, 10, 10, 12, 11, 2, 2, 8, 10, 6, 9, 12, 7, 5, 1, 12, 11, 10, 6, 9, 12, 5, 9, 12, 5, 9, 10, 6], [5, 11, 10, 4, 7, 3, 10, 10, 6, 5, 3, 6, 5, 3, 12, 5, 13, 3, 8, 10, 8, 10, 6, 3, 12, 7, 9, 6, 9, 10, 6, 3, 6, 9, 10, 4, 9, 10, 12, 7, 9, 10, 6, 3, 10, 6, 7, 1, 10, 10, 12, 5, 5, 5, 5, 3, 6, 3, 12, 13], [5, 9, 12, 5, 9, 10, 10, 10, 10, 6, 11, 12, 1, 10, 6, 5, 1, 14, 3, 12, 7, 9, 8, 14, 5, 9, 6, 11, 2, 10, 10, 12, 13, 5, 11, 4, 5, 13, 3, 10, 6, 11, 10, 10, 8, 10, 12, 3, 14, 9, 6, 5, 5, 5, 3, 10, 8, 14, 5, 5], [5, 5, 5, 3, 6, 9, 12, 11, 10, 8, 10, 6, 3, 12, 13, 5, 3, 8, 14, 3, 12, 5, 3, 12, 5, 3, 10, 10, 12, 9, 12, 3, 4, 3, 12, 5, 3, 2, 12, 11, 8, 10, 12, 9, 6, 9, 6, 9, 10, 6, 9, 6, 3, 6, 9, 12, 7, 9, 6, 5], [5, 5, 3, 10, 10, 6, 3, 10, 12, 1, 12, 201, 10, 6, 3, 2, 10, 6, 9, 10, 4, 5, 13, 3, 6, 9, 10, 12, 5, 5, 5, 9, 6, 9, 6, 3, 12, 13, 3, 10, 6, 13, 3, 6, 9, 6, 11, 2, 12, 9, 4, 9, 10, 10, 6, 3, 10, 6, 9, 6], [5, 3, 10, 12, 9, 10, 10, 12, 5, 7, 3, 2, 10, 12, 9, 10, 10, 12, 3, 12, 5, 3, 4, 11, 10, 4, 9, 2, 6, 5, 3, 6, 13, 5, 11, 10, 2, 2, 10, 10, 8, 6, 9, 10, 6, 11, 8, 12, 5, 5, 7, 5, 9, 10, 10, 14, 9, 10, 2, 12], [3, 8, 12, 3, 6, 11, 12, 5, 3, 10, 12, 9, 12, 3, 4, 9, 12, 3, 10, 6, 5, 13, 3, 12, 9, 4, 7, 9, 12, 5, 9, 10, 6, 3, 8, 12, 9, 10, 12, 9, 6, 9, 6, 11, 8, 10, 6, 3, 6, 3, 12, 5, 3, 8, 10, 12, 3, 12, 9, 4], [9, 6, 3, 14, 9, 8, 6, 1, 10, 14, 1, 6, 5, 11, 6, 5, 5, 9, 12, 9, 6, 1, 12, 3, 6, 5, 9, 6, 5, 3, 6, 9, 10, 12, 7, 5, 3, 12, 3, 6, 13, 5, 11, 8, 6, 11, 10, 10, 8, 10, 4, 3, 12, 3, 14, 3, 10, 6, 5, 5], [5, 9, 12, 9, 6, 3, 10, 6, 9, 12, 3, 14, 1, 12, 9, 6, 5, 7, 3, 2, 10, 6, 5, 13, 9, 6, 5, 11, 2, 10, 10, 4, 11, 2, 218, 6, 13, 5, 9, 10, 4, 3, 10, 6, 9, 10, 10, 10, 6, 9, 6, 13, 3, 10, 10, 10, 10, 10, 6, 5], [1, 6, 5, 3, 12, 13, 9, 10, 6, 3, 10, 12, 7, 3, 2, 14, 1, 10, 10, 10, 10, 12, 3, 4, 5, 13, 3, 10, 10, 10, 12, 7, 9, 10, 10, 12, 3, 2, 6, 13, 3, 12, 9, 12, 5, 9, 8, 10, 12, 5, 9, 2, 10, 8, 8, 10, 10, 10, 14, 5], [3, 14, 1, 12, 5, 5, 5, 9, 12, 9, 14, 5, 9, 10, 10, 12, 7, 9, 12, 9, 12, 3, 12, 7, 3, 4, 9, 10, 14, 9, 6, 9, 6, 9, 14, 3, 12, 9, 8, 0, 14, 5, 5, 3, 6, 5, 3, 12, 5, 5, 7, 9, 12, 7, 5, 9, 10, 12, 9, 6], [9, 8, 6, 7, 3, 4, 3, 6, 5, 1, 12, 1, 6, 11, 8, 2, 10, 6, 3, 6, 3, 12, 3, 12, 9, 6, 3, 12, 9, 6, 9, 6, 11, 2, 8, 12, 5, 7, 5, 7, 9, 6, 1, 10, 12, 3, 12, 5, 7, 3, 10, 6, 3, 12, 1, 6, 9, 6, 5, 13], [5, 7, 9, 12, 9, 2, 14, 9, 6, 5, 3, 6, 9, 12, 5, 9, 10, 12, 9, 10, 14, 5, 9, 6, 3, 10, 10, 4, 5, 9, 6, 9, 10, 10, 6, 5, 3, 12, 3, 10, 2, 12, 3, 14, 3, 12, 7, 5, 9, 10, 12, 9, 12, 5, 7, 9, 6, 9, 6, 5], [3, 10, 6, 3, 2, 14, 9, 6, 13, 5, 13, 9, 6, 5, 7, 5, 13, 5, 5, 9, 10, 6, 1, 10, 10, 8, 14, 5, 5, 3, 12, 5, 9, 10, 14, 5, 13, 3, 10, 12, 11, 2, 12, 9, 10, 6, 9, 6, 5, 13, 3, 6, 5, 3, 10, 4, 9, 6, 9, 4], [9, 10, 10, 10, 10, 12, 1, 12, 3, 2, 6, 1, 14, 5, 9, 2, 6, 5, 5, 3, 12, 11, 0, 10, 14, 5, 9, 6, 3, 12, 3, 6, 3, 12, 9, 6, 3, 8, 12, 5, 9, 12, 5, 5, 9, 10, 2, 10, 6, 3, 8, 14, 3, 10, 12, 5, 5, 11, 4, 5], [5, 11, 8, 10, 10, 6, 5, 3, 10, 10, 10, 6, 9, 4, 1, 14, 9, 6, 1, 14, 3, 12, 7, 9, 10, 4, 5, 9, 14, 3, 10, 8, 10, 6, 3, 12, 9, 6, 5, 3, 6, 5, 1, 6, 3, 10, 10, 12, 9, 10, 6, 9, 8, 14, 3, 6, 3, 12, 5, 5], [1, 12, 3, 10, 12, 11, 2, 10, 10, 14, 9, 8, 6, 7, 5, 9, 6, 11, 2, 10, 10, 2, 10, 6, 11, 6, 5, 5, 9, 10, 12, 3, 10, 10, 12, 3, 6, 9, 6, 13, 9, 6, 7, 9, 12, 9, 12, 5, 3, 10, 10, 4, 3, 10, 12, 9, 10, 6, 5, 7], [7, 5, 9, 12, 5, 9, 10, 12, 9, 10, 6, 5, 9, 10, 6, 3, 10, 10, 12, 9, 10, 12, 9, 12, 9, 12, 5, 5, 5, 13, 3, 12, 9, 12, 5, 9, 12, 3, 10, 4, 3, 10, 10, 6, 5, 5, 5, 3, 10, 10, 12, 7, 9, 10, 6, 3, 10, 236, 3, 12], [9, 6, 5, 7, 5, 5, 13, 3, 6, 9, 14, 5, 3, 12, 9, 10, 10, 12, 3, 6, 9, 6, 5, 3, 6, 3, 6, 1, 2, 6, 9, 6, 5, 5, 3, 6, 1, 10, 14, 5, 9, 10, 12, 9, 6, 7, 3, 8, 10, 12, 3, 8, 6, 9, 10, 10, 12, 3, 12, 5], [1, 12, 3, 8, 6, 5, 1, 10, 14, 1, 10, 2, 14, 5, 5, 9, 12, 3, 10, 12, 3, 10, 6, 11, 10, 8, 12, 7, 9, 12, 5, 9, 6, 3, 12, 9, 6, 9, 12, 3, 6, 13, 5, 3, 12, 9, 12, 5, 9, 4, 9, 6, 9, 6, 9, 12, 1, 14, 5, 5], [5, 3, 12, 3, 12, 5, 3, 8, 10, 6, 9, 10, 10, 6, 3, 6, 3, 12, 13, 5, 9, 10, 10, 8, 10, 6, 3, 10, 6, 5, 5, 3, 12, 11, 2, 6, 9, 6, 3, 10, 10, 4, 3, 12, 5, 5, 5, 7, 5, 7, 5, 9, 2, 14, 5, 5, 3, 12, 5, 5], [1, 14, 5, 13, 5, 3, 12, 5, 9, 10, 6, 11, 10, 8, 10, 12, 11, 2, 6, 5, 5, 9, 10, 6, 11, 10, 8, 14, 9, 6, 1, 12, 3, 10, 12, 9, 6, 9, 8, 10, 12, 3, 12, 5, 3, 6, 3, 12, 5, 9, 6, 5, 13, 9, 6, 3, 10, 6, 3, 4], [5, 9, 6, 3, 0, 10, 6, 7, 5, 9, 10, 10, 10, 6, 13, 3, 12, 9, 12, 3, 6, 5, 9, 8, 10, 12, 3, 12, 3, 12, 5, 3, 10, 14, 3, 6, 9, 6, 5, 13, 3, 12, 5, 3, 12, 9, 12, 5, 3, 6, 9, 6, 5, 3, 10, 10, 10, 10, 10, 6], [5, 3, 10, 12, 7, 9, 10, 10, 6, 3, 12, 9, 8, 12, 3, 12, 5, 5, 3, 12, 9, 6, 5, 5, 11, 2, 10, 4, 13, 5, 5, 9, 10, 10, 10, 10, 6, 9, 6, 5, 9, 6, 1, 12, 5, 7, 5, 3, 10, 10, 6, 9, 4, 9, 8, 14, 9, 8, 12, 13], [3, 12, 9, 6, 9, 6, 13, 9, 8, 10, 6, 7, 5, 3, 10, 4, 5, 3, 12, 5, 5, 11, 4, 3, 12, 9, 10, 2, 6, 5, 5, 3, 8, 14, 9, 10, 10, 6, 9, 2, 6, 9, 6, 5, 3, 12, 1, 8, 10, 10, 10, 6, 5, 5, 3, 10, 6, 5, 5, 5], [9, 6, 3, 12, 5, 9, 6, 5, 5, 9, 10, 12, 5, 9, 12, 7, 3, 10, 6, 5, 1, 12, 5, 9, 6, 7, 9, 12, 9, 6, 1, 10, 6, 9, 6, 9, 12, 13, 5, 11, 12, 5, 13, 3, 12, 5, 7, 1, 12, 9, 12, 9, 6, 3, 14, 9, 12, 5, 3, 4], [3, 12, 9, 4, 5, 3, 10, 6, 3, 2, 14, 5, 5, 5, 3, 8, 12, 13, 9, 6, 7, 5, 5, 3, 10, 12, 5, 3, 6, 11, 2, 10, 14, 5, 11, 4, 5, 5, 5, 9, 4, 5, 3, 10, 6, 3, 12, 7, 3, 4, 5, 3, 10, 12, 9, 6, 5, 5, 9, 6], [9, 6, 5, 7, 3, 12, 9, 10, 10, 12, 9, 6, 1, 6, 9, 6, 3, 6, 3, 10, 12, 5, 5, 13, 9, 6, 3, 10, 10, 10, 12, 9, 10, 6, 9, 6, 3, 6, 3, 6, 5, 3, 12, 9, 10, 12, 3, 10, 12, 7, 3, 12, 13, 5, 5, 9, 6, 5, 3, 12], [3, 14, 3, 10, 10, 2, 6, 11, 10, 2, 6, 11, 2, 14, 3, 10, 10, 10, 10, 10, 2, 6, 3, 6, 3, 10, 10, 10, 10, 10, 2, 6, 11, 10, 2, 10, 10, 10, 10, 10, 2, 14, 3, 2, 14, 243, 10, 10, 2, 10, 10, 6, 3, 2, 6, 3, 10, 6, 11, 6]]

can_go = [[[] for i in range(60)] for j in range(60)]


pos_to_reach = {}

for i in range(len(arr_maze)):
  for j in range(len(arr_maze[i])):
    step = ['0','0','0']
    if arr_maze[i][j]&1==0:
      step = ['0','0','0']
      step[0] = i
      step[1] = j-1
      step[2] = 'h'
      can_go[i][j].append(step)

    if arr_maze[i][j] & 2 == 0:
      step = ['0', '0', '0']
      step[0] = i+1
      step[1] = j
      step[2] = 'j'
      can_go[i][j].append(step)
    
    if arr_maze[i][j]&8==0:
      step = ['0', '0', '0']
      step[0] = i-1
      step[1] = j
      step[2] = 'k'
      can_go[i][j].append(step)
    
    if arr_maze[i][j]&4==0:
      step = ['0', '0', '0']
      step[0] = i
      step[1] = j+1
      step[2] = 'l'
      can_go[i][j].append(step)
    
    if arr_maze[i][j] & 0x80 != 0:
      pos_to_reach[(arr_maze[i][j] >> 4) & 7] = [i, j, hex(arr_maze[i][j])]

print(pos_to_reach)


arr_destination = [6, 3, 7, 5, 1, 4, 0, 2]

current_pos = [0,0]
res = ''
for des in arr_destination:
  res += bfs(current_pos[0],current_pos[1],pos_to_reach[des][0],pos_to_reach[des][1],can_go) +'x'
  current_pos[0] = pos_to_reach[des][0]
  current_pos[1] = pos_to_reach[des][1]
print(res)

```

## Result

![image](https://user-images.githubusercontent.com/31529599/134941420-1df3a9cd-9d28-4690-a412-97da2f459e8a.png)

Flag: `DUCTF{bfs_dfs_ffs_no_more_mazes_a8fb66c12cd}`

# Short Write-up for another challenges

## No strings

![image](https://user-images.githubusercontent.com/31529599/134941594-2895ad54-ca03-436c-a12b-b3cd755f2c19.png)

This just warm-up challenge with hard code strings flag

Flag: `DUCTF{stringent_strings_string}`

## Juniperus

![image](https://user-images.githubusercontent.com/31529599/134942324-78f37445-9c55-45ed-88b6-bf4062f81b85.png)

This look like a secure login challenges, with password was hash sum by sha3-512, then compare to the input hash. But a tricky CTF chall, a small mistake in code.

Flag: `DUCTF{57r1ng5-4bl3... k1nd4}`

## FLag checker

![image](https://user-images.githubusercontent.com/31529599/134942355-f04589ae-6980-4176-bc4a-4098bafe6ee5.png)

Recaculated and swap the input string (flag) in 16 loops and compare to the fixed values in memory. A weakness was the same swap order. So z3 is solvable.

Python script
```python
from z3 import *

def sub_cal(a1):
    return ((2*a1) ^ (27*(LShR(a1, 7))))


def cal(a1, a2):
    v9 = a1[a2[0]]
    v10 = a1[a2[1]]
    v11 = a1[a2[2]]
    v12 = a1[a2[3]]
    v13 = a1[a2[4]]
    v14 = a1[a2[5]]

    v2 = v11 ^ v9 ^ sub_cal(v9)
    v3 = sub_cal(v11) ^ v2
    v15 = v3 ^ sub_cal(v13)
    v4 = sub_cal(v10)
    v5 = sub_cal(v12) ^ v12 ^ v10 ^ v4
    v16 = v5 ^ sub_cal(v14)
    v17 = v13 ^ sub_cal(v9)
    v18 = v14 ^ sub_cal(v10)
    v6 = v9 ^ sub_cal(v9)
    v19 = v6 ^ sub_cal(v11)
    v7 = v10 ^ sub_cal(v10)
    v20 = v7 ^ sub_cal(v12)
    a1[a2[0]] = v15
    a1[a2[1]] = v16
    a1[a2[2]] = v17
    a1[a2[3]] = v18
    a1[a2[4]] = v19
    a1[a2[5]] = v20
    return a1


def first_func(flag):

    v2 = [0]*6
    v3 = [0]*6
    v4 = [0]*6
    v5 = [0]*6
    v6 = [0]*6
    v7 = [0]*6

    v2[0] = 0
    v2[1] = 1
    v2[2] = 2
    v2[3] = 6
    v2[4] = 12
    v2[5] = 18
    v3[0] = 3
    v3[1] = 4
    v3[2] = 5
    v3[3] = 11
    v3[4] = 17
    v3[5] = 23
    v4[0] = 7
    v4[1] = 8
    v4[2] = 9
    v4[3] = 13
    v4[4] = 14
    v4[5] = 15
    v5[0] = 10
    v5[1] = 16
    v5[2] = 22
    v5[3] = 28
    v5[4] = 29
    v5[5] = 35
    v6[0] = 19
    v6[1] = 20
    v6[2] = 24
    v6[3] = 25
    v6[4] = 26
    v6[5] = 30
    v7[0] = 21
    v7[1] = 27
    v7[2] = 31
    v7[3] = 32
    v7[4] = 33
    v7[5] = 34

    flag = cal(flag, v2)
    flag = cal(flag, v3)
    flag = cal(flag, v4)
    flag = cal(flag, v5)
    flag = cal(flag, v6)
    flag = cal(flag, v7)
    return flag


swap = [23, 16, 19, 12, 31, 24, 17, 22, 13, 18, 25, 30, 9, 2, 11, 4, 33,
        26, 3, 8, 5, 10, 27, 32, 21, 14, 35, 28, 7, 0, 15, 20, 29, 34, 1, 6]


arr = [15, 79, 115, 60, 65, 198, 164, 175, 180, 65, 214, 101, 200, 153, 170, 179,
       108, 153, 97, 60, 78, 221, 112, 70, 21, 102, 60, 27, 127, 22, 166, 111, 35, 19, 18, 110]
for i in range(16):
    next_arr = [0]*36
    for j in range(36):
        next_arr[swap[j]] = arr[j]
    arr = next_arr
    flag = [BitVec(f'flag_{k}', 8) for k in range(36)]
    a = first_func(flag)
    S = Solver()

    for k in range(36):
        S.add(a[k] == arr[k])

    assert sat == S.check()
    ans = S.model()
    arr_res = []
    flag = [BitVec(f'flag_{k}', 8) for k in range(36)]
    for f in flag:
        arr_res.append(ans[f].as_long())
    arr = arr_res
    # print(arr_res)
for i in arr_res:
    print(chr(i),end='')

```

Flag: `DUCTF{rev3rs1bl3___and___1nv3rtibl3}`



