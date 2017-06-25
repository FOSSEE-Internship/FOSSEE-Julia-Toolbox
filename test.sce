// // test for hypermat
// M = hypermat([2 3 2 2],[%T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F])
// callJulia('+', M, M)


// // // testing for boolean
// [z] = callJulia('!', %T)
// [z] = callJulia('!', [%T, %T, %T; %F, %T, %F])
// M = hypermat([2 3 2 2], [%T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F, %T, %T, %T, %F, %T, %F])
// [x] = callJulia('!', M)



// // test for double
// A = callJulia('+', [6945233.43, 435.2], [43425.43, 23])
// M = hypermat([2 3 2 2],1:24)
// B = callJulia('+', M, M)
// [a, b] = callJulia('divrem', 43, 3)

// // test for integers

// [e] = callJulia('+', uint32(43), uint32(4))
// [a, b] = callJulia('divrem', uint32(43), uint32(4))
// [c, d] = callJulia('divrem', [uint32(43), uint32(4)], uint32(5))
// M = hypermat([2 3 2 2],uint32(1:24))
// [x, y] = callJulia('divrem', M, uint32(5))

// // test for strings
// callJulia('string', "This is", " cool")
// callJulia('*', "This is", " cool", ".\n")
// a = callJulia('string', ["This is", "This is", "This is"], " cool")
// M = hypermat([2 3 2 2], ["Yeah!", "Yeah!", "Yeah!", "No!!", "Yeah!", "No!!", "Yeah!", "Yeah!", "Yeah!", "No!!", "Yeah!", "No!!", "Yeah!", "Yeah!", "Yeah!", "No!!", "Yeah!", "No!!", "Yeah!", "Yeah!", "Yeah!", "No!!", "Yeah!", "No!!"])
// b = callJulia('copy', M)

// complex
n = [2 + %i, 23.3]
// n = 2 + %i
callJulia('+', n, n)
[s, b] = evalJulia('([2 * im, 2.3 + 3*im], [2 * im, 2.433 + 3*im])')