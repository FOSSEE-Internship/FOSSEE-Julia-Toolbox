[A, B] = callJulia('+', 6945233.43, 43425.43)
[e] = callJulia('+', uint32(43), uint32(4))
[a, b] = callJulia('divrem', uint32(43), uint32(4))
[c, d] = callJulia('divrem', [uint32(43), uint32(4)], uint32(5))

[x] = callJulia('+', %T, %T)
[y] = callJulia('+', %T, %F)
[z] = callJulia('!', [%T, %T, %T; %F, %T, %F])
