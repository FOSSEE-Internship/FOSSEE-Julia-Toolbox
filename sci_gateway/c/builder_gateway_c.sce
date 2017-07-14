function builder_gw_cpp()
	files = ['sci_call_julia.c', 'double_conv.c', 'integer_conv.c', 'bool_conv.c', 'string_conv.c', 'sparse_conv.c']
	functions = ['callJulia','sci_call_julia'; 'initJulia', 'sci_init_julia'; 'evalJulia', 'sci_eval_julia'; 'getGlobalJulia', 'sci_get_global_julia'; 'exitJulia', 'sci_exit_julia'; 'importJuliaPackage', 'sci_import_package';]
	root = get_absolute_file_path('builder_gateway_c.sce') + filesep() + ".." + filesep() + ".." + filesep();
	third_party_dir = root + 'thirdparty'
	
	julia_dir = third_party_dir + filesep() + getos() + filesep() + 'julia'
	julialibpath = julia_dir + filesep() + 'lib'

	// Loading dependencies 
	if getos() == 'Windows' then 
		include = "-DJULIA_ENABLE_THREADING=1 -DJULIA_INIT_DIR=""" + julialibpath + """ -I""C:\Program Files (x86)\Windows Kits\10\Include\10.0.15063.0\um"" -I""C:\Program Files (x86)\Windows Kits\10\Include\10.0.15063.0\shared"" -I""" + julia_dir + "\include\julia"" "
		ldflag = "-L""" + julia_dir + "\bin"" -ljulia -lopenlibm "
	elseif getos() == 'Darwin' then
		include = "-g -DJULIA_ENABLE_THREADING=1 -fPIC -DJULIA_INIT_DIR=" + julia_dir + "/lib -I" + julia_dir + "/include/julia"
		ldflag = "-L" + julialibpath + " -Wl,-rpath," + julialibpath + " -ljulia"
		link(julialibpath + '/julia/libcurl.5' + getdynlibext())
	elseif getos() == 'Linux' then
		include = '-g -I/home/harshith/scilab-5.5.2/include -I' + julia_dir + '/include/julia -DJULIA_ENABLE_THREADING'
		ldflag = '-L' + julia_dir + '/lib/julia -L' + julialibpath + ' -Wl,--verbose -Wl,-rpath,' + julialibpath + ' -Wl,-Bstatic -ljulia' 
	end


	setenv('JULIA_DIR', julia_dir)
	setenv('JULIA_HOME', julia_dir + filesep() + 'bin')

	// ilib_build('build_lib', functions, files, [julialibpath + filesep() + 'libjulia'], [], ldflag, include);
	// ilib_build('build_lib', ['callJulia','sci_call_julia'; 'initJulia', 'sci_init_julia'; 'exitJulia', 'sci_exit_julia'], files, [], [], ldflag, include);

    WITHOUT_AUTO_PUTLHSVAR = %t;

    
    tbx_build_gateway("juliainterface", ..
    functions, ..
    [files], ..
    get_absolute_file_path("builder_gateway_c.sce"),[julialibpath + filesep() + 'libjulia'],[ldflag],[include],[]);

endfunction

builder_gw_cpp();
clear builder_gw_cpp; 