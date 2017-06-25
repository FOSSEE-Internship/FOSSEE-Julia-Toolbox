function set_env()
	root = get_absolute_file_path('load_and_test.sce')
	third_party_dir = root + 'thirdparty/'

	julia_dir = third_party_dir + getos() + '/julia'
	setenv('JULIA_DIR', julia_dir)
	setenv('JULIA_HOME', julia_dir + '/bin')
endfunction

set_env()
clear builder_gw_cpp

exec loader.sce
initJulia()
exec test.sce
exitJulia()
exec unloader.sce