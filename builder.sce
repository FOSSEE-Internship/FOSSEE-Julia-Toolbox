files = ['sci_call_julia.c', 'double_conv.c']
// files = ['sci_call_julia.c', 'double_conv.c', 'bool_conv.c']

root = get_absolute_file_path('builder.sce')

julia_dir = root + 'julia'
julialibpath = julia_dir + '/lib/'

// Loading dependencies 

if getos() == 'Windows' then 
	disp("Not yet implemented for Windows")
	return;
else 
	link(julialibpath + 'julia/libstdc++' + getdynlibext() + '.6')
	link(julialibpath + 'libjulia' + getdynlibext())

setenv('JULIA_HOME', julia_dir + '/bin')

include = ' -g -I' + julia_dir + '/include/julia -DJULIA_ENABLE_THREADING'
ldflag = '-L' + julia_dir + '/lib/julia -L' + julia_dir + '/lib -ljulia'
ilib_build('build_lib', ['callJulia','sci_call_julia'], files, [], [], ldflag, include);
exec loader.sce
A = [1.1, 2.2]
B = [971.801, 132.132]
callJulia('divrem', A, B)
exec unloader.sce

clear A B C D E julia files julia_dir julialibpath root include ldflag