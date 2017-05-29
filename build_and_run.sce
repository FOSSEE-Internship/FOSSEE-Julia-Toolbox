exec builder.sce

exec loader.sce
A = 12
B = [971, 132]
callJulia('divrem', A, B)
exec unloader.sce

clear A B C D E