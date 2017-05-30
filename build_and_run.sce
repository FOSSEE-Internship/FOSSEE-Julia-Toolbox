exec builder.sce

exec loader.sce
A = [12, 23.23]
B = [971, 132.23]
callJulia('+', A, B)
exec unloader.sce

clear A B C D E