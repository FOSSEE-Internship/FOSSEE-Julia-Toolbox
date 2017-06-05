// files = ['sci_call_julia.c', 'double_conv.c']
files = ['sci_call_julia.c', 'double_conv.c', 'integer_conv.c', 'bool_conv.c']

root = get_absolute_file_path('builder.sce')

third_party_dir = root + 'thirdparty'

// Loading dependencies 
if getos() == 'Windows' then 
	disp("Not yet implemented for Windows")
	return;
elseif getos() == 'Darwin' then
	julia_dir = third_party_dir + '/darwin/julia'
	julialibpath = julia_dir + '/lib/'

	setenv('LD_LIBRARY_PATH', third_party_dir + '/darwin/julia/lib/julia:' + getenv('LD_LIBRARY_PATH'))
	setenv('DYLD_LIBRARY_PATH', third_party_dir + '/darwin/julia/lib/julia:' + getenv('DYLD_LIBRARY_PATH'))

	// link(julialibpath + 'julia/libstdc++' + getdynlibext() + '.6')
	link(julialibpath + 'libjulia' + getdynlibext())

elseif getos() == 'Linux' then
	julia_dir = third_party_dir + '/linux/julia'
	julialibpath = julia_dir + '/lib/'

	link(julialibpath + 'julia/libstdc++' + getdynlibext() + '.6')
	link(julialibpath + 'libjulia' + getdynlibext())
end


setenv('JULIA_HOME', julia_dir + '/bin')

include = ' -g -I' + julia_dir + '/include/julia -DJULIA_ENABLE_THREADING'
ldflag = '-L' + julia_dir + '/lib/julia -L' + julia_dir + '/lib -ljulia'
ilib_build('build_lib', ['callJulia','sci_call_julia'; 'initJulia', 'sci_init_julia'; 'exitJulia', 'sci_exit_julia'], files, [], [], ldflag, include);

clear files julia_dir julialibpath root third_party_dir include ldflag