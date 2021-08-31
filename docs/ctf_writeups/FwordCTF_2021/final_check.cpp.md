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
