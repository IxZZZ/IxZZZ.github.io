```asm
debug046:00A317AE mov     edi, [ebp+8]
debug046:00A317B1 mov     cl, [edi]
debug046:00A317B3 xor     cl, [edi+4Eh]
debug046:00A317B6 xor     cl, [edi+31h]
debug046:00A317B9 xor     cl, [edi+3]
debug046:00A317BC add     cl, [edi+21h]
debug046:00A317BF sub     cl, [edi+31h]
debug046:00A317C2 sub     cl, [edi+1Ah]
debug046:00A317C5 cmp     cl, 1Ch
debug046:00A317C8 jnz     loc_A322FA
debug046:00A317CE mov     cl, [edi+1]
debug046:00A317D1 and     cl, [edi+3Ah]
debug046:00A317D4 and     cl, [edi+2Ah]
debug046:00A317D7 add     cl, [edi+15h]
debug046:00A317DA add     cl, [edi+2Bh]
debug046:00A317DD xor     cl, [edi+45h]
debug046:00A317E0 or      cl, [edi+2Ch]
debug046:00A317E3 cmp     cl, 0F7h ; '÷'
debug046:00A317E6 jnz     loc_A322FA
debug046:00A317EC mov     cl, [edi+2]
debug046:00A317EF and     cl, [edi+0Dh]
debug046:00A317F2 xor     cl, [edi+40h]
debug046:00A317F5 add     cl, [edi+2Dh]
debug046:00A317F8 xor     cl, [edi+1Ah]
debug046:00A317FB sub     cl, [edi+22h]
debug046:00A317FE sub     cl, [edi+46h]
debug046:00A31801 cmp     cl, 0CAh ; 'Ê'
debug046:00A31804 jnz     loc_A322FA
debug046:00A3180A mov     cl, [edi+3]
debug046:00A3180D sub     cl, [edi+29h]
debug046:00A31810 or      cl, [edi+28h]
debug046:00A31813 and     cl, [edi+52h]
debug046:00A31816 add     cl, [edi+50h]
debug046:00A31819 add     cl, [edi+47h]
debug046:00A3181C and     cl, [edi+4Ch]
debug046:00A3181F cmp     cl, 31h ; '1'
......
...
..
.
~ a lot more ~
```
