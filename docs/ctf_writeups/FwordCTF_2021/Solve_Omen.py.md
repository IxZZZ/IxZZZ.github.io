```python
from z3 import *


input = [BitVec(f"input_{i}", 8) for i in range(0x5f+1)]

file = open('data.txt', 'r')
data = file.read()
file.close()

S = Solver()
count = 0
temp = 0
for line in data.split('\n'):
    if 'jnz' in line:
        temp = 0
        continue
    if 'cmp' in line:
        num = line.split(',')[1].strip(' ')
        S.add(temp == int(num, 16))
        continue
    offset = 0
    if 'edi+' in line:
        offset_str = line.split('+')[1].strip(']')
        offset = int(offset_str, 16)

    operator = line[:3].strip(' ')
    if operator == 'mov':
        temp = input[offset]
    elif operator == 'xor':
        temp ^= input[offset]
    elif operator == 'add':
        temp = (temp+input[offset]) & 0xff
    elif operator == 'sub':
        temp = (temp-input[offset]) & 0xff
    elif operator == 'and':
        temp &= input[offset]
    elif operator == 'or':
        temp |= input[offset]
    else:
        print('wrong operator')

for i in range(len(input)):
    S.add(input[i] >= 48)
    S.add(input[i] <= 126)


assert S.check() == sat

ans  = S.model()

for i in input:
    print(chr(ans[i].as_long()),end='')
```
