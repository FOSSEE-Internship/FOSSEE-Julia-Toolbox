// This file is released under the 3-clause BSD license. See COPYING-BSD.
// Generated by builder.sce : Please, do not edit this file
// ----------------------------------------------------------------------------
//
libjuliainterface_path = get_absolute_file_path('loader.sce');
//
// ulink previous function with same name
[bOK, ilib] = c_link('libjuliainterface');
if bOK then
  ulink(ilib);
end
//
link('/Users/harshithgoka/Documents/GitHub/scilab-julia-interface/sci_gateway/c//../../thirdparty/Darwin/julia/lib/libjulia' + getdynlibext());
list_functions = [ 'callJulia';
                   'initJulia';
                   'evalJulia';
                   'getGlobalJulia';
                   'exitJulia';
                   'importJuliaPackage';
];
addinter(libjuliainterface_path + filesep() + 'libjuliainterface' + getdynlibext(), 'libjuliainterface', list_functions);
// remove temp. variables on stack
clear libjuliainterface_path;
clear bOK;
clear ilib;
clear list_functions;
// ----------------------------------------------------------------------------