# **Flare-on 7 2020 Research write up**

This is all my short decription and learned thing from all challenges from the Flare-on 7. During do and research all the challenges, I have learnt a lot of thing from two write up, one is an official write-up from [fire-eye](https://www.real-sec.com/2020/10/flare-on-7-challenge-solutions/) and another one is also the great write-up for learning [explained-re web site](https://explained.re/posts/flare-on-7-write-up-of-all-write-ups/)


# 1 - Filder
> This is the first most challenge and so easy to solve, This program will self print out the flag with the expected value 100 milion coins, with the known expected value we can trick the decode_flag function to print to us the flag `idle_with_kitty@flare-on.com`

# 2 - garbage

## Learned Thing
- [ ]  Using CFF Explorer to read and analysis PE Header

## Challenge Flow
- This is a windows challenges but with a corrupted binary. Overall analysis, We knew that it was packed by UPX. However, we can not intermediately unpack by the standard tool before fix it
- Using CFF explorer to inspect the PE header and fix the file by padding some null bytes to the end of the file then the `upx` tool work correctly but can not be run
- Beginning statically analysis, we meet a suspicious function to decrypt data, implement that function in python then we can receive challenge flag inform `MsgBox("Congrats! Your key is: C0rruptGarbag3@flare-on.com")`

# 3 - wednesday 

## Challenge Flow

- This is a reverse game challenges then I guess just win the game and then retrieve flag
- Yeah, my idea alright but not at all, If we directly jump to the win function so we will receive nothing so the score might have effect to our final result
- This game just a simple game where we have to control a toad to avoid all obstacle to win the game
- Solve: I just patch the condition that jump to instruction that increase the score one by one so the toad always avoid obstacle and win the game → flag: `1t_i5_wEdn3sd4y_mY_Dud3s@flare-on.com`

# 4 - report

## Learned thing
- [ ]  OLE2 Format and Macro script embemded into OLE file
- [ ]  Some useful tools to analysis challenges related to OLE format
- [ ]  VBA stomping (the VBA code different than shown)

## Challenge Flow
- The first of all, we using the the `olevba` tool to extract all macros from file `excel`, the most notice thing here is that `VBA stomping` was detected, it define that the VBA code was dumped not equal to the real one, then just using the `pcode2code` tool for better extract the `stomping` code
- After has the right code so We can easy statically analysis and retrieve flag in form of a `png` image
- flag: `1t_i5_wEdn3sd4y_mY_Dud3s@flare-on.com`

# 5 - TKapp
## Learned Thing
- [ ]  Structure to construct a smart watch from .Net dll

## Challenges Flow

- This challenge provide a zip compress file which contain a `dll` file
- Load `dll` file to `DNSpy` then do statically analysis
- Because it was implemented in .Net environment so it easy to understand the code flow
- The final flag challenge was in form of an image but encrypted by AES so just simple reverse all stage in `dll` and retrieve all the parameter for AES then decrypted our flag
- flag → `n3ver_go1ng_to_recov3r@flare-on.com`

# 6 - codeit
## Learned Thing

- [ ]  Auto It executable

## Challenge Flow

- Using `DIE` tool to inspect the provided windows binary, we knew that it using Auto It executable
- Using `Exe2Aut` to convert `.exe` to `.au3` file, then start statically analysis
- The program has text box to receive input then generate a qr-code respectively
- First of all in a processing of analysis, We should deobfuscate the code for easily reversing, The program using 3 type of obfuscation that are string, number, constant obfuscation. Just a small python with regular expression to deobfuscate the code
- Understand of auto it code, the program will have two conditional flows, the first one is the original we mentioned when run binary, just generate the qr-code depend on the string input, the second one might be execute when it meet some condition then load some crypto library then decrypt something.
- It may be the challenge flag is in the second flow, follow this flow, we know that the program read the computer name then pass it to a function and recalculated final return the new string and will be compare to the hardcode hash if the condition meet the new string will be the key of the decryption algorithm to decrypt something.
- So inspect the processing computer name function then we recover the correct `computer name` so just rename my own computer then restart
- Run the Binary then the qr-code generator now just generate only one qr-code with any random input. Scan that qr-code and receive flag → `L00ks_L1k3_Y0u_D1dnt_Run_Aut0_Tim3_0n_Th1s_0ne!@flare-on.com`

# 7 - re_crowd
## Learned Thing

- [ ]  How to dynamically analysis shellcode
- [ ]  Search for keyword, search any suspicious information we meet during analysis
- [ ]  Recognize important information from the general information we have (Forensics technique)
- [ ]  Wireshark: using `Statistic -> Conversations` to view the statistics of transfer data then just notice to several one which has the most amount transmission
- [ ]  Shellcode decode type (utf-8, unicode-utf16)
- [ ]  How the malware shellcode will be load the api function without depending on the built-in library (using the PEB)

## Challenge Flow

- Challenge provide the pcap file, the just export all object in shellcode then start analysis it
- With the general information we know that the file `account.txt` was stolen
- Moreover, Inspecting to the transmission data of port `4444` we saw that some weird signature so search for it and knew that the traffic is the exploit payload of a CVE
- In the exploit payload, the most considered part is that the shellcode so just dump the shellcode then continuously analysis
- Using `shellcode_launcher` tool to debug the dump shellcode

### The first stage

- For knew of the type of shellcode is the module exploit of metasploit  then we also knew the key xor `XORK` to encrypt the length payload and key xor `killervulture123` to encryp the payload
- In shellcode we will know the the next stage will be transfer in port `4444` and the payload will be encrypt by the mentioned previous parameter.
- Dump the content of port `4444` and then decrypt it, We now have the second stage shellcode

### The second stage

- In this stage the shellcode using a function to load the api function with two argument, dll and api name hash
- The api resolving function explain:
    - Iterate the process environment block (PEB) of loaded module list and capitalize each DLL name and uses `ROR-13` hashing algorithm to compute a dll name then compare the provided input hash
    - If match found the function will iterate the Loaded DLL export table and calculate the an API hash for each export name using the same previous hash algorithm, but without capitalization
    - If match found the function return pointer to the api function
    - Otherwise it return zero
- In the debugging, we also known which api function will be loaded from the api resolving function, then the flow is just we predict before that:
    - the shellcode will open and read the `accounts.txt` file
    - then encrypted it using `RC4` algorithm with hardcode key `intrepidmango`
    - next, the encrypted data will be sent to the server through port `1337`
- Solve: Just extract data transmission in port `1337` from pcap file then decrypt it with the knew key and retrieve flag → `h4ve_you_tri3d_turning_1t_0ff_and_0n_ag4in@flare-on.com`


# 8 - aardvark
## Learned Thing

- [ ]  The `exe` binary can leverage the `wsl` to run another linux binary such as a backend for `exe`

## Challenge Flow

- The challenges is just the `Tic-Tac-Toe` game but it require to meet some windows configuration to run it. It should be run in windows 10 installed a wsl 1 version of distro linux
- The final purpose of the challenge just might win the game
- It is the easy challenges when we can patch the binary to win the game and receive flag → `c1ArF/P2CjiDXQIZ@flare-on.com`


# 9 - crackinstaller
## Learned Thing

- [ ]  it take me almost 5 days to understand about just 70% write up of this challenges, it really is a complicated challenge and there are a lot of new thing to learn in this challenge
- [ ]  COM model and related thing such as IID, GUID, CLSID, ...
- [ ]  CPP global initializer

![image](https://user-images.githubusercontent.com/31529599/150123842-656fc758-43b1-4f64-88b7-2ad32bfab1a1.png)


- [ ]  Some new knowledge about the windows driver

## Challenges Flow

### Overview

- The challenge binary are contain 2 major path of execution:
    - The first one was put on the startup code section (CRT initializer), it will be call before main
    - The second one, of course it will be in main
- In the first path execution, it will be execute the 64 bit signed driver to bypass the Supervisor Mode Execution Prevention (SMEP) then the program will leverage the bypass of SMEP to load a unsigned driver, the second loaded driver will be used to register registry callbacks and filter on pre-key-creation → build challenge information (password, flag,...) and save to registry
- The second path execution in the main function, will register a COM server

### Reverse the COM server in main

- The COM server will be in a `dll`and be decrypted then load and called in main, we can set breakpoint and dump the `dll` to further analysis this COM server
- Analysis the COM `dll` , we will know that the COM server will read the registry `CLSID\Config\Password` then using the value in that key to decrypt the known data through RC4 and set it to the `CLSID\Config\Flag`
- At this point, we know that , we just need the password then the challenges solved
- And How to receive Password?

### Second Path execution for driver

- The driver will call the [`CmRegisterCallbackEx`](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-cmregistercallbackex) which registers a callback function for the configuration manager (registry). The pointer to the `RegistryCallback` will calculate the `password` then call the `ZwCreateKey` to write the password to registry
- Then just debug and retrieve the `password` → `"H@n $h0t FiRst!"`
- Now, we can decrypt the flag using the RC4 with the known password
- flag → `S0_m@ny_cl@sse$_in_th3_Reg1stry@flare-on.com`

> This is all my understanding of the write up, it may be have some missing... !!!


# 10 - break
## Learned thing

- [ ]  First of all should use `findcrypt` to detect all crypto constant
- [ ]  if binary contain no anti-patching techinique, in some case we can patch binary into infinite loop then dump `/proc/<pid>/mem` (Using code seek() and read())
- [ ]  hooking function

![image](https://user-images.githubusercontent.com/31529599/150123790-4ece9cd3-2ff6-4b99-8158-8afd6494cd34.png)


```c
// hook.c
// gcc -m32 -shared -fPIC -D_GNU_SOURCE hook.c -o hook.so
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>

int pivot_root (int arg1, int arg2)
{	
	static int (*func_pivot_root) (const void*, size_t) = NULL;
	int retval = 0;

	if (! func_pivot_root)
		func_pivot_root = (int (*) (int, const void*, size_t)) dlsym (RTLD_NEXT, "pivot_root");
	fprintf(stdout, "[HOOK] pivot_root a1: 0x%x a2: 0x%x \n", arg1, arg2);

	retval = func_pivot_root (arg1, arg2);
	return retval;
}

int mlockall (uint64_t arg)
{	
	static int (*func_mlockall) ( size_t) = NULL;
	int retval = 0;

	if (! func_mlockall)
		func_mlockall = (int (*) (int, const void*, size_t)) dlsym (RTLD_NEXT, "mlockall");

	retval = func_mlockall (arg);
	fprintf(stdout, "[HOOK] mlockall() returned %d\n", retval);
	return retval;
}
```

![image](https://user-images.githubusercontent.com/31529599/150123738-8c91352e-5ef9-4dca-8187-5a0abfeddddb.png)

- [ ]  Guess and google with github
- [ ]  Nanomite can be debug by the way we can patch raise kill signal instruction to the `debugger` code respectively

```c
from malduck import unhex 

end_addr = idc.get_segm_end(0)

offset = ida_search.find_binary(0, end_addr,"83 EC 04 52 50 68 24 3C 2D B8 8B 45 D8 FF D0 83 C4 10", 16, idc.SEARCH_DOWN)
# inc    eax
ida_bytes.patch_bytes(offset,unhex("909090409090909090909090909090909090"))

offset = ida_search.find_binary(0, end_addr,"83 EC 04 68 FE CA 00 00 68 37 13 00 00 68 2A DB 85 7E 8B 45 D8 FF D0 83 C4 10", 16, idc.SEARCH_DOWN)
# mov    eax,0x9e3779b9
ida_bytes.patch_bytes(offset,unhex("909090B8B979379E909090909090909090909090909090909090"))

offset = ida_search.find_binary(0, end_addr,"83 EC 04 52 50 68 2C 10 4E 6B 8B 45 F0 FF D0 83 C4 10", 16, idc.SEARCH_DOWN)
# add    eax,edx
ida_bytes.patch_bytes(offset,unhex("90909001D090909090909090909090909090"))

offset = ida_search.find_binary(0, end_addr,"83 EC 04 52 50 68 2E 45 16 58 8B 45 F0 FF D0 83 c4 10", 16, idc.SEARCH_DOWN)
# push   edx
# push   eax
# call   0xffffff86
ida_bytes.patch_bytes(offset,unhex("9090905250E87CFFFFFF909090909083C408"))

offset = ida_search.find_binary(0, end_addr,"83 EC 04 52 50 68 30 7A DE 44 8B 45 F0 FF D0 83 C4 10", 16, idc.SEARCH_DOWN)
# xor    eax,edx
ida_bytes.patch_bytes(offset,unhex("90909031D090909090909090909090909090"))

offset = ida_search.find_binary(0, end_addr,"83 EC 04 52 50 68 28 A6 BD 91 8B 85 2C FF FF FF FF D0 83 C4 10", 16, idc.SEARCH_DOWN)
# dec    eax
# shl    eax,0x4
# dec    edx
# and    edx,0xf
# or     eax,edx
ida_bytes.patch_bytes(offset,unhex("48C1E0044A83E20F09D09090909090909090909090"))

offset = ida_search.find_binary(0, end_addr,"83 EC 08 68 80 90 04 08 6A 0E E8 8A F8 FF FF 83 C4 10 E8 E2 F8 FF FF 83 EC 0C 50 E8 5D 0F 00 00 83 C4 10", 16, idc.SEARCH_DOWN)
# Fill the second fork with nops
ida_bytes.patch_bytes(offset,unhex("9090909090909090909090909090909090909090909090909090909090909090909090"))
```

- [ ]  We can also patch the available function to the `write, printf, ...` to print out our expected value

## The challenges Flow

### Stage 1

- Using Nanomite to change the program flow
- Call fork to create `child1` as a `debugger` of `child`, in `child1` continously call fork to create `child2` as a `debugger` of `child1`
- The `child1` will trace and change the flow of the `child` and the `child2` will do the same with `child2`
- In the first stage the program just using the custom `AES128` with 32 bit block size to decrypt the ciphertext to plaintext and compare to 16 first bytes of input
- Solve: Just patch the infinite loop after decryption operation and dump the plaintext → `w3lc0mE_t0_Th3_l`

### Stage 2

- The second stage combine to all three of process to check the next part of input
- Using custom ARX Feistel Cipher to encrypt input and compare to the known cipher
- Because using Feistel Cipher so the decrypt operation with be the same as decrypt operation with the reverse key order, then just dump the key and decrypt the second part of flag → `4nD_0f_De4th_4nd_d3strUct1oN_4nd`

### Stage 3

- Yeah, the third check has some tricky things, because it using the buffer overflow to corrupt the stack then make the flow is not hit the right direction we think and then it redirect to a shellcode
- In the third stage we have to dynamically analysis through patching the program and attach debug to dump the shellcode
- About the analysis the shellcode, we can take advantage of the open-source code available on github and compare to our IDA pseudocode for easy to understand mathematics code
- Solve: The shellcode section contain implementation of a Big Num library but with some bug in code so we can reverse it and calculate the final part of our flag *`_n0_puppi3s@flare-on.com`*

## The Final Flag:

`w3lc0mE_t0_Th3_l4nD_0f_De4th_4nd_d3strUct1oN_4nd_n0_puppi3s@flare-on.com`

# 11 - rabithole
## Learned Thing

- [ ]  The one of the main tenets of malware reverse engineering: `"Always focus on the big picture, and do not get lost in the tiny details"`
- [ ]  TEB and PEB

[TEB and PEB](https://rvsec0n.wordpress.com/2019/09/13/routines-utilizing-tebs-and-pebs/)

- [ ]  Easy attach debug by pre-append infinite loop to the shellcode
- [ ]  question to compile file to execute shellcode just with simple `start`
- [ ]  Search gg and github for touching the weird stuff
- [ ]  Understood some flow of the common windows malware, how to call built-in api without lib/dll
- [ ]  Set break-point or hook decryption/encryption function and patch input argument to receive the output without implementation whole the function
- [ ]  Malware inject module to the exist common program such as explore.exe or powershell.exe, ...
- [ ]  automate script ida with python

## Challenge Flow

### Overall

This is the really really complicated challenges, It take me about several days to understand the writeup but almost 80%. The challenges using registry hive which is so new to me. Using the registry hive to hide the malicious binary and binary was executed when logon process.

### The fisrt stage

- This stage I do the statically analysis approach to extract all the interest thing from the challenge file and the most considered one is that `HKCU\SOFTWARE\Timerpro`
- Inspecting more to the `Timerpro,` it contains a shellcode. This main purpose of shellcode was just initialized all the the required part of malware
- Until reach the stuck wall, I have to put the next stage to the dymamically analysis and start debugging

### The second stage

- Dumping the shellcode and rewrite a C program to simulate and debug the shellcode then I can easily debug shellcode
- After some effort to analysis the program and search for stuff, we recognize the bank malware named Saigon
- Next, we meet the problem when the shellcode using the SID (security Identifier) to generate the string key name in registry then just patch the input `SID` to the found original `SID` to generate the same key name
- Then, we met some encryption and decryption function, one of it is serpent algorithm and a xor fucntion to encrypt and decrypt all the module
- Just set breakpoint and patch the input then receive all the decrypted result

### The third stage

- With all decrypted module, We can still not retrieve a challenge flag
- But there is one suspicious file, this might still not be readable named `DiMap`
- Tracing the operation how the `DiMap` was created and then We knew that it took 2 stage encryption, first one is `serpent encryption` with hardcode key and second `xor` which we had already decrypted it
- Then just Decrypted with `serpent` then retrieve the flag `r4d1x_m4l0rum_357_cup1d1745@flare-on.com`
