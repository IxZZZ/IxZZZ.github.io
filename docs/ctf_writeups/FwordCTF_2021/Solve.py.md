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
