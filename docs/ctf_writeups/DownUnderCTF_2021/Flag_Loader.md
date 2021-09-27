# **Rev Challenges Write Up FwordCTF 2021 - (24-26/09/2021)**
# Flag Loader 218

## Description

What more could you ask for than a program that loads the flag for you? Just answer a few simple questions and the flag is yours!

Author: joseph#8210
 `nc pwn-2021.duc.tf 31919`
 
 File: [flag_loader]()
 
 ## Overview 
 
 - A program with three layers check conditions (random math expression). Just input the correct answer and pass all checks, server will print out the flag.
 - Specially, the input will be calculated and then multiply together,The program will call sleep for the number of seconds equals to the final result.
 - One more thing the author was made this chall a little more diffucult that the total time live of the program is not more than 1 minute.
 
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
 
In first time, I was try the of course value `DUCTF` but it not pass the check, because the `v2` is just a byte type so the multiply operation was overflow byte size and the result of 8 bit was equal `0`.

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

# Connect the dot
