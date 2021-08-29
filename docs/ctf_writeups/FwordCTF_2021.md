**Rev Challenges Write Up FwordCTF 2021**
# Time machine (961 points)

## Description

No description needed just discover it by your self :D

Flag Format: FWORDctf{}

Author: Joezid 

File: [Time_Machine.exe]()

## Overview 
- A flag checker program
- Statically analysis code and I find out the function to check and print out `Correct Flag :)`
- Check's function caculate value from flag character then compare to constant value which was caculate in another process

## Analyze
Firstly in the main function, I met the condition to check the environment variable `joezidsecret` is exist or not. Then call the `handle` function respectively.

Pseudocode main:

```c
int __cdecl main(int argc, const char **argv, const char **envp)
{
  int result; // eax

  sub_401E80();
  if ( getenv("joezidsecret") )
    result = handle_1();
  else
    result = handle_2(*argv);
  return result;
}
```
After creating the `joezidsecret` environment variable. The `handle_1` was called and this will receive flag input and then caculate out the constant value to compare in check flag function in other process.

I was analyzed overall all the instruction below and see that, [rcx] was flag character, with each flag character this will have total 10 constant value was caculate with the same operator but different constant, then the same with [rcx+i] is flag[i] will have 10 constant value.
### Note: 
- r12: flag character
- r11: constant value
- r13 : 0 or 1

The instrucion to calculate the constant value. (just a small part)
```bash
data:0000000000412020 sub_412020      proc near               ; DATA XREF: handle_1+73↑o
.data:0000000000412020                 mov     r12b, [rcx]
.data:0000000000412023                 mov     r11, 1A6232C39h
.data:000000000041202D                 xor     r11, 1337h
.data:0000000000412034                 ror     r11, 0Dh
.data:0000000000412038                 mov     r13, 0
.data:0000000000412042                 ud2
.data:0000000000412042 sub_412020      endp
.data:0000000000412042
.data:0000000000412044 ; ---------------------------------------------------------------------------
.data:0000000000412044                 mov     r12b, [rcx]
.data:0000000000412047                 mov     r11, 568A8C79h
.data:0000000000412051                 xor     r11, 1337h
.data:0000000000412058                 ror     r11, 0Dh
.data:000000000041205C                 mov     r13, 0
.data:0000000000412066                 ud2
.data:0000000000412068 ; ---------------------------------------------------------------------------
.data:0000000000412068                 mov     r12b, [rcx]
.data:000000000041206B                 mov     r11, 13E06DEA5h
.data:0000000000412075                 xor     r11, 1337h
.data:000000000041207C                 ror     r11, 0Dh
.data:0000000000412080                 mov     r13, 0
.data:000000000041208A                 ud2
.data:000000000041208C ; ---------------------------------------------------------------------------
.data:000000000041208C                 mov     r12b, [rcx]
.data:000000000041208F                 mov     r11, 83BDBECD337h
.data:0000000000412099                 xor     r11, 1337h
.data:00000000004120A0                 ror     r11, 0Dh
.data:00000000004120A4                 mov     r13, 1
.data:00000000004120AE                 ud2
.data:00000000004120B0 ; ---------------------------------------------------------------------------
.data:00000000004120B0                 mov     r12b, [rcx]
.data:00000000004120B3                 mov     r11, 1D4562E11h
.data:00000000004120BD                 xor     r11, 1337h
.data:00000000004120C4                 ror     r11, 0Dh
.data:00000000004120C8                 mov     r13, 0
.data:00000000004120D2                 ud2
.data:00000000004120D4 ; ---------------------------------------------------------------------------
.data:00000000004120D4                 mov     r12b, [rcx]
.data:00000000004120D7                 mov     r11, 1A81CE1FBh
.data:00000000004120E1                 xor     r11, 1337h
.data:00000000004120E8                 ror     r11, 0Dh
.data:00000000004120EC                 mov     r13, 0
.data:00000000004120F6                 ud2

.......
......
....
~ a lot more ... ~
```


Continuously, I was tried default condition in case the `joezidsecret` does not exist and `handle_2` was called. This function will call the check flag function.

I just show the check flag function below:

```c
int __fastcall final_check(__int64 a1, void *a2, void *a3)
{
  int result; // eax
  __int64 v4; // [rsp+0h] [rbp-80h] BYREF
  __int16 v5; // [rsp+36h] [rbp-4Ah] BYREF
  SIZE_T NumberOfBytesRead; // [rsp+38h] [rbp-48h] BYREF
  char v7[2]; // [rsp+46h] [rbp-3Ah] BYREF
  char v8[2]; // [rsp+48h] [rbp-38h] BYREF
  __int16 Buffer; // [rsp+4Ah] [rbp-36h] BYREF
  BOOL v10; // [rsp+4Ch] [rbp-34h]
  struct _CONTEXT Context; // [rsp+50h] [rbp-30h] BYREF
  int v12; // [rsp+52Ch] [rbp+4ACh]

  memset(&v4 + 10, 0, 0x4D0ui64);
  Context.ContextFlags = 1048579;
  v10 = GetThreadContext(a3, &Context);
  if ( !v10 )
    sub_4033E0("ret", "nano.cc", 88i64);
  Buffer = 0;
  v8[0] = -112;
  v8[1] = -112;
  v7[0] = -61;
  v7[1] = -112;
  NumberOfBytesRead = 0i64;
  v10 = ReadProcessMemory(a2, (LPCVOID)Context.Rip, &Buffer, 2ui64, &NumberOfBytesRead);
  if ( !v10 )
    sub_4033E0("ret", "nano.cc", 97i64);
  v5 = 0;
  result = (unsigned __int8)Buffer;
  if ( (_BYTE)Buffer == 15 )
  {
    result = HIBYTE(Buffer);
    if ( HIBYTE(Buffer) == 11 )
    {
      LOBYTE(v5) = Context.R12;
      v12 = sub_401584((unsigned __int8 *)&v5);
      if ( v12 == Context.R11 && Context.R13 == 1 )
        ++dword_419030;
      if ( dword_419030 == 39 )
      {
        sub_40F190("Correct Flag :)\n");
        v10 = WriteProcessMemory(a2, (LPVOID)Context.Rip, v7, 2ui64, &NumberOfBytesRead);
        SetThreadContext(a3, &Context);
      }
      result = dword_419030;
      if ( dword_419030 != 39 )
      {
        v10 = WriteProcessMemory(a2, (LPVOID)Context.Rip, v8, 2ui64, &NumberOfBytesRead);
        result = SetThreadContext(a3, &Context);
      }
    }
  }
  return result;
}
```
Can see that, the condition to print `Correct Flag :)` is `dword_419030 == 39`, with 39 was the length of flag.

In order to `dword_419030 == 39` the before condition `if ( v12 == Context.R11 && Context.R13 == 1 )` was met at least 39 times.

With a little dynamically analysics, I know that the Context.R12,Context.R11,Context.R13 are r12 (flag character), r11 (constant value), r13 (0 or 1) respectively. `v12` was compare to `r11` and `r13` has to equal `1`. `v12` was result of `sub_401584` (input is flag character - `r12`).

Pseudocode `sub_401584`:

```c
__int64 __fastcall sub_401584(unsigned __int8 *character)
{
  int v2; // [rsp+20h] [rbp-10h]
  int v3; // [rsp+24h] [rbp-Ch]
  signed int v4; // [rsp+28h] [rbp-8h]
  int len; // [rsp+28h] [rbp-8h]
  int lena; // [rsp+28h] [rbp-8h]
  unsigned int lenb; // [rsp+28h] [rbp-8h]
  int lenc; // [rsp+28h] [rbp-8h]
  unsigned int lend; // [rsp+28h] [rbp-8h]
  unsigned int c; // [rsp+28h] [rbp-8h]
  unsigned int lene; // [rsp+28h] [rbp-8h]
  int i; // [rsp+2Ch] [rbp-4h]
  unsigned __int8 *v13; // [rsp+40h] [rbp+10h]

  v13 = character;
  v4 = (unsigned int)sub_401550(character);
  v2 = v4 & 3;
  for ( i = v4 >> 2; i > 0; --i )
  {
    len = (v13[1] << 8) + *v13 + v4;
    v3 = len ^ (((v13[3] << 8) + v13[2]) << 11);
    v13 += 4;
    v4 = (((len << 16) ^ (unsigned int)v3) >> 11) + ((len << 16) ^ v3);
  }
  switch ( v2 )
  {
    case 2:
      lenc = (v13[1] << 8) + *v13 + v4;
      v4 = (((lenc << 11) ^ (unsigned int)lenc) >> 17) ^ (lenc << 11) ^ lenc;
      break;
    case 3:
      lena = (v13[1] << 8) + *v13 + v4;
      lenb = (v13[2] << 18) ^ (lena << 16) ^ lena;
      v4 = (lenb >> 11) + lenb;
      break;
    case 1:
      lend = ((*v13 + v4) << 10) ^ (*v13 + v4);
      v4 = (lend >> 1) + lend;
      break;
  }
  c = (((8 * v4) ^ (unsigned int)v4) >> 5) + ((8 * v4) ^ v4);
  lene = (((16 * c) ^ c) >> 17) + ((16 * c) ^ c);
  return (((lene << 25) ^ lene) >> 6) + ((lene << 25) ^ lene);
}
```

## Solution

After comprehensive understood the function, I was proposed solution that I will code a script python to brutce force the flag character because the `sub_401584` is likely irreversible, so I can not mapping from constant value to the flag character.

```python
import string


# rebuild sub_401584 function
def func(val):
    v9 = (((val+1) << 10) ^ (val+1)) & 0xffffffff
    len = ((v9 >> 1)+v9) & 0xffffffff
    c = ((((8*len) ^ len) >> 5) + ((8*len) ^ len)) & 0xffffffff
    v11 = ((((16*c) ^ c) >> 17) + ((16*c) ^ c)) & 0xffffffff
    res = ((((v11 << 25) & 0xffffffff ^ v11) >> 6) +
           ((v11 << 25) & 0xffffffff ^ v11)) & 0xffffffff
    return res & 0xffffffff


def ror(val, n):
    p1 = val >> n
    p2 = val << (64-n)
    return (p1 | p2) & 0xffffffffffffffff

extract_arr = []

offset = 0x0000000000412023
arr_sub = []
# extract the constant value
for j in range(10):
        b = ida_bytes.get_bytes(offset+2+j*36, 8)
        num = int.from_bytes(b, byteorder='little')
        num = num^0x1337
        num = ror(num,0xd)
        arr_sub.append(num)
extract_arr.append(arr_sub)

offset = 0x0000000000412023 + 36*10+1
for i in range(0, 0x26):
    arr_sub = []
    for j in range(10):
        b = ida_bytes.get_bytes(offset+2+37*10*i+j*37, 8)
        num = int.from_bytes(b, byteorder='little')
        num = num^0x1337
        num = ror(num,0xd)
        arr_sub.append(num)
    extract_arr.append(arr_sub)

ascii = string.printable


# brute force the character through `sub_401584` with constant value was extracted
for arr in extract_arr:
    for c in ascii:
        if func(ord(c)) in arr:
            print(c,end='')

```

This is script of idapython, it contains three primary part:

- Rebuild `sub_401584` function
- Extract and caculate constant value (in this case, we can just extract the value with `r13 == 1`, but in my code was extract all -> this is not really necessary)
- Brute Force the printable character input to rebuild `sub_401584` function and constant value then print matched

### Solve
![image](https://user-images.githubusercontent.com/31529599/131258401-5a158dab-b7e5-48d2-9a4b-ece6a62c7111.png)




