# **Rev Challenges Write Up FwordCTF 2021 - (28-29/08/2021)**
# Time machine (961 points)

## Description

No description needed just discover it by your self :D

Flag Format: FWORDctf{}

Author: Joezid 

File: [Time_Machine.exe](https://github.com/IxZZZ/CTF_JOINED/blob/main/FwordCTF/RE/Time%20Machine/Time_Machine.exe?raw=true)

## Overview 

- A flag checker program
- Statically analysis code and I find out the function to check and print out `Correct Flag :)`
- Check's function caculate value from flag character then compare to constant value which was caculate in another process

## Analyze

Firstly in the main function, I met the condition to check the environment variable `joezidsecret` is exist or not. Then call the `handle` function respectively.

Pseudocode main:

[main-pseudocode.cpp](main-pseudocode.cpp.md ':include')

After creating the `joezidsecret` environment variable. The `handle_1` was called and this will receive flag input and then caculate out the constant value to compare in check flag function in other process.

I was analyzed overall all the instruction below and see that, [rcx] was flag character, with each flag character this will have total 10 constant value was caculate with the same operator but different constant, then the same with [rcx+i] is flag[i] will have 10 constant value.
- Note: 
  - r12: flag character
  - r11: constant value
  - r13 : 0 or 1

The instrucion to calculate the constant value. (just a small part)

[asm_nominates.asm6502](asm_nominates.asm6502.md ':include')


Continuously, I was tried default condition in case the `joezidsecret` does not exist and `handle_2` was called. This function will call the check flag function.

I just show the check flag function below:

[final_check.cpp](final_check.cpp.md ':include')

Can see that, the condition to print `Correct Flag :)` is `dword_419030 == 39`, with 39 was the length of flag.

In order to `dword_419030 == 39` the before condition `if ( v12 == Context.R11 && Context.R13 == 1 )` was met at least 39 times.

With a little dynamically analysics, I know that the Context.R12,Context.R11,Context.R13 are r12 (flag character), r11 (constant value), r13 (0 or 1) respectively. `v12` was compare to `r11` and `r13` has to equal `1`. `v12` was result of `sub_401584` (input is flag character - `r12`).

Pseudocode `sub_401584`:

[sub_401582.cpp](sub_401584.cpp.md ':include')

## Solution

After comprehensive understood the function, I was proposed solution that I will code a script python to brutce force the flag character because the `sub_401584` is likely irreversible, so I can not mapping from constant values to the flag characters.

[Solve_Time_Machine.py](Solve_Time_Machine.py.md ':include')

This is script of idapython, it contains three primary parts:

- Rebuild `sub_401584` function
- Extract and caculate constant value (in this case, we can just extract the value with `r13 == 1`, but in my code was extracted all values-> this is not really necessary)
- Brute Force the printable character input to rebuild `sub_401584` function and constant value then print matched

### Solve
![image](https://user-images.githubusercontent.com/31529599/131258401-5a158dab-b7e5-48d2-9a4b-ece6a62c7111.png)


# Omen (988 Points)

## Description

They fear death, they should fear so much more than that.

Flag format : FWORDctf{}

Author : Joezid

File: [Omen_Final.exe](https://github.com/IxZZZ/CTF_JOINED/blob/main/FwordCTF/RE/Omen/Omen_Final.exe?raw=true)

## Overview

- Also a flag checker program.
- This challenges was use an anti-analysis (vm) like two stages which first stage use self inject to execute the second stage.
- Fortunately, I still can trace out and debug to the check flag instruction and everything was solved.

## Analyze

I was set a break point at the `Enter The Flag:` and `%s` string to find out the check flag instruction.

This instruction call the `unk_A313A7` to check flag before print `Correct Flag :)\n`

![image](https://user-images.githubusercontent.com/31529599/131258958-7b25bbde-e668-4d55-a5ad-c4ea70c696a7.png)

[asm_flag_checker.asm](asm_flag_checker.asm6502.md ':include')

an above instruction caculate and check flag input. after overall anaylising, I know that:
- `[edi+i]` is the flag character (`flag[i]`)
- The flag has length `0x60` (96)
- Check flag instrusion contains a lot of small check, if one not meet condition this will return `eax = 0` and not print anything
- each small check was contain different operation caculator

## Solution

With a 96 flag character length and just a brute force approach is possible in this case because each small check can not be reverse to the character flag. `Z3` in my mind =)))

I also generate a script python to brute force the flag:

[Solve_Omen.py](Solve_Omen.py.md ':include')


My code was extract each small check in data.txt file and add to Solver() in `Z3` (file [data.txt](https://raw.githubusercontent.com/IxZZZ/CTF_JOINED/main/FwordCTF/RE/Omen/data.txt) contains the check flag instruction).

## Result

`FWORDctf{Wh4t_4b0ut_th1s_w31rd_L0ng_fL4g_th4t_m4k3_n0_s3ns3_but_st1LL_w1LL_g1v3_y0u_s0m3_p01ntz}`





