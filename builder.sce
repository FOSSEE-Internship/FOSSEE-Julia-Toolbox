function builder_gw_cpp()
	// This file is released under the 3-clause BSD license. See COPYING-BSD.
	
	// files = ['sci_call_julia.c', 'double_conv.c']
	files = ['sci_call_julia.c', 'double_conv.c', 'integer_conv.c', 'bool_conv.c', 'string_conv.c']

	root = get_absolute_file_path('builder.sce')
	third_party_dir = root + 'thirdparty'
	
	julia_dir = third_party_dir + '/' + getos() + '/julia'
	julialibpath = julia_dir + '/lib/'

	// Loading dependencies 
	if getos() == 'Windows' then 
		disp("Not yet implemented for Windows")
		return;
	elseif getos() == 'Darwin' then
		// setenv('LD_LIBRARY_PATH', third_party_dir + '/darwin/julia/lib/julia:' + getenv('LD_LIBRARY_PATH'))
		// setenv('DYLD_LIBRARY_PATH', third_party_dir + '/darwin/julia/lib/julia:' + getenv('DYLD_LIBRARY_PATH'))
		include = "-g -DJULIA_ENABLE_THREADING=1 -fPIC -DJULIA_INIT_DIR=" + julia_dir + "/lib -I" + julia_dir + "/include/julia"
		ldflag = "-L" + julialibpath + " -Wl,-rpath," + julialibpath + " -ljulia"
		link(julialibpath + 'julia/libcurl.5' + getdynlibext())
	elseif getos() == 'Linux' then
		include = '-g -I/home/harshith/scilab-5.5.2/include -I' + julia_dir + '/include/julia -DJULIA_ENABLE_THREADING'
		ldflag = '-L' + julia_dir + '/lib/julia -L' + julialibpath + ' -Wl,--verbose -Wl,-rpath,' + julialibpath + ' -Wl,-Bstatic -ljulia' 
	end


	setenv('JULIA_DIR', julia_dir)
	setenv('JULIA_HOME', julia_dir + '/bin')

	ilib_build('build_lib', ['callJulia','sci_call_julia'; 'initJulia', 'sci_init_julia'; 'evalJulia', 'sci_eval_julia'; 'exitJulia', 'sci_exit_julia'], files, [julialibpath + 'libjulia'], [], ldflag, include);
	// ilib_build('build_lib', ['callJulia','sci_call_julia'; 'initJulia', 'sci_init_julia'; 'exitJulia', 'sci_exit_julia'], files, [], [], ldflag, include);

	// setenv('LD_LIBRARY_PATH', third_party_dir + '/linux/julia/lib/julia:' + third_party_dir + '/linux/julia/lib:' + getenv('LD_LIBRARY_PATH'))

    WITHOUT_AUTO_PUTLHSVAR = %t;

    
    // tbx_build_gateway("juliainterface", ..
    // ['callJulia','sci_call_julia'; 'initJulia', 'sci_init_julia'; 'exitJulia', 'sci_exit_julia'], ..
    // [files], ..
    // get_absolute_file_path("builder.sce"),[],[ldflag],[include],[]);

endfunction

builder_gw_cpp();
clear builder_gw_cpp; // remove builder_gw_cpp on stack-