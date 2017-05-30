exec builder.sce

exec loader.sce
A = [32, 34]
B = 4
[C, D] = callJulia('divrem', [32, 34], 4)
exec unloader.sce

clear A B