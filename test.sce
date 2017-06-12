// // test for double
A = callJulia('+', [6945233.43, 435.2], [43425.43, 23])

// // test for integers
// [e] = callJulia('+', uint32(43), uint32(4))
// [a, b] = callJulia('divrem', uint32(43), uint32(4))
// [c, d] = callJulia('divrem', [uint32(43), uint32(4)], uint32(5))

// // testing for boolean
// [x] = callJulia('+', %T, %T)
// [y] = callJulia('+', %T, %F)
// [z] = callJulia('!', [%T, %T, %T; %F, %T, %F])

// test for hypermat
// M = hypermat([2 3 2 2],[%T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F])
// callJulia('+', M, M)
// M = hypermat([2 3 2 2],uint32(1:24))
// callJulia('+', M, M)

M = hypermat([2 3 2 2],1:24)
callJulia('+', M, M)