exec builder.sce

exec loader.sce
A = [32, 34]
B = 4
callJulia('divrem', [32, 34], 4)
// exec unloader.sce

// clear A B C D E