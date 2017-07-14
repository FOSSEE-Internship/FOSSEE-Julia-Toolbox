lines(0);
try
 getversion('scilab');
catch
 error(gettext('Scilab 5.0 or more is required.'));
end;

// ====================================================================
TOOLBOX_NAME = "Julia_Interface";
TOOLBOX_TITLE = "Julia Interface";
// ====================================================================

toolbox_dir = get_absolute_file_path("builder.sce");

tbx_builder_macros(toolbox_dir);
tbx_builder_gateway(toolbox_dir);

// if getscilabmode()=="STD" then
	// tbx_builder_help(toolbox_dir);
// end

tbx_build_loader(TOOLBOX_NAME, toolbox_dir);
tbx_build_cleaner(TOOLBOX_NAME, toolbox_dir);

clear toolbox_dir TOOLBOX_NAME TOOLBOX_TITLE;