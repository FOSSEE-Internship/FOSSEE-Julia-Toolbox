#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"
#include <string.h>

extern int double_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int double_jl_to_sci(jl_value_t *input, int position);

extern int int_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int int_jl_to_sci(jl_value_t *input, int position);

extern int bool_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int bool_jl_to_sci(jl_value_t *input, int position);

extern int string_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int string_jl_to_sci(jl_value_t *input, int position);

extern int sparse_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int sparse_jl_to_sci(jl_value_t *input, int position);

int sci_init_julia(char* fname, unsigned long fname_len) {
    
    ////////// Julia Init Code //////////
    /* required: setup the Julia context */
    jl_init();
    return 0;
}

int sci_exit_julia(char* fname, unsigned long fname_len) {
    /* strongly recommended: notify Julia that the
         program is about to terminate. this allows
         Julia time to cleanup pending write requests
         and run all finalizers
    */
    jl_atexit_hook(0);
    return 0;
}

int sci_import_package(char* fname, unsigned long fname_len) {
    // Error management variable
    SciErr sciErr;

    ////////// Variables declaration //////////
    int *addrEvalString = NULL;
    char *package, *function;

    int nInputArgs = nbInputArgument(pvApiCtx);
    int nOutputArgs = nbOutputArgument(pvApiCtx);

    ////////// Manage the first input argument (double) //////////
    /* get Address of inputs */
    sciErr = getVarAddressFromPosition(pvApiCtx, 1, &addrEvalString);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    if(!isStringType(pvApiCtx, addrEvalString)) {
        Scierror(999, "%s: argument #%d not a string: string expected\n", fname, 1);
        return 0;
    }

    int err = getAllocatedSingleString(pvApiCtx, addrEvalString, &package);
    if (err != 0)
    {
        return 0;
    }

    sciprint("%s: adding %s\n", fname, package);
    char add[100]; 
    sprintf(add, "Pkg.add(%s)", package);
    jl_eval_string(add);

    int i = 0;
    char *importing = (char*) (malloc(sizeof(char) * (strlen(package) + strlen("using ") + 2))); 
    strcpy(importing, "using ");
    strcat(importing, package);


    sciprint("%s: importing \"%s\" \n", fname, importing);
    jl_value_t *temp = jl_eval_string(importing);

    sciprint("%s: %s\n", fname, jl_typeof_str(temp));
    return 0;
}

int jl_to_sci(jl_value_t *ret, int position) {
    if (jl_isa(ret, jl_get_global(jl_base_module, jl_symbol("Void")))) {
        return 1;
    }

    int err;
    
    size_t al;
    int tuple = 0, i;
    char *fname = "jl_to_sci";
    jl_value_t *newargs;
    if(jl_is_tuple(ret)) {
        sciprint("%s: multiple return values\n", fname);
        al = jl_nfields(ret);
        tuple = 1;
    }
    else {
        sciprint("%s: single return value\n", fname);
        al = 1;
        newargs = ret;
    }
    
    for (i = position; i != position + al; i++) {
        if (tuple)
            newargs = jl_fieldref(ret, i);
        
        if(jl_is_array(newargs) || jl_isa(newargs, jl_get_global(jl_base_module, jl_symbol("BitArray")))) {
            int ndims = jl_array_ndims(newargs);

            if (jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_float64_type, ndims)) || 
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_apply_type((jl_value_t*)jl_complex_type, (jl_value_t**)&(jl_float64_type), 1), ndims)) ) {
                sciprint("%s: Float variable\n", fname);
                err = double_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if (jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_bool_type, ndims)) || jl_isa(newargs, jl_get_global(jl_base_module, jl_symbol("BitArray")))) {
                sciprint("%s: Boolean variable\n", fname);
                err = bool_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if (jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_int8_type, ndims)) || 
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_uint8_type, ndims)) || 
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_int16_type, ndims)) || 
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_uint16_type, ndims)) || 
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_int32_type, ndims)) || 
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_uint32_type, ndims)) ||
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_int64_type, ndims)) ||
                jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_uint64_type, ndims)) ) {

                sciprint("%s: Integer variable\n", fname);
                err = int_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if (jl_typeis(newargs, jl_apply_array_type((jl_value_t*)jl_string_type, ndims))) {

                sciprint("%s: String variable\n", fname);
                err = string_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
        }
        else {
            if(jl_typeis(newargs, jl_float64_type) || 
                jl_typeis(newargs, jl_apply_type((jl_value_t*)jl_complex_type, (jl_value_t**)&(jl_float64_type), 1))) {
                sciprint("%s: Float variable\n", fname);
                err = double_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if(jl_typeis(newargs, jl_bool_type)) {

                sciprint("%s: Boolean variable\n", fname);
                err = bool_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if(jl_typeis(newargs, jl_int8_type) || 
                    jl_typeis(newargs, jl_uint8_type) || 
                    jl_typeis(newargs, jl_int16_type) || 
                    jl_typeis(newargs, jl_uint16_type) || 
                    jl_typeis(newargs, jl_int32_type) || 
                    jl_typeis(newargs, jl_uint32_type) || 
                    jl_typeis(newargs, jl_int64_type) || 
                    jl_typeis(newargs, jl_uint64_type) ) {
                
                sciprint("%s: Integer variable\n", fname);
                err = int_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if (jl_typeis(newargs, jl_string_type)) {
                sciprint("%s: String variable\n", fname);
                err = string_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if (jl_isa(newargs, jl_get_global(jl_base_module, jl_symbol("SparseMatrixCSC")))) {
                sciprint("%s: SparseMatrixCSC variable\n", fname);
                err = sparse_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else {
                sciprint("%s: %s variable\n", fname, jl_typeof_str(newargs));
                Scierror(999, "%s: non double types not implemented yet: double return expected\n", fname);
                return 0;
            }
        }

        
        if (err == 0) {
            return 0;
        }


        sciprint("%s: assigning output variable #%d\n", fname, i + 1);
        AssignOutputVariable(pvApiCtx, i + 1) = nbInputArgument(pvApiCtx) + i + 1;
    }

    return 1;
}

int sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    int err = 0;
    int iType = 0;
    char *fname = "sci_to_jl";
    SciErr sciErr = getVarType(pvApiCtx, piAddressVar, &iType);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    sciprint("%s: argument #: VarType: %d\n", fname, iType);
    if (isDoubleType(pvApiCtx, piAddressVar)) {
        sciprint("%s: argument #: Double variable\n", fname);
        err = double_sci_to_jl(piAddressVar, ret);
    }
    else if (isBooleanType(pvApiCtx, piAddressVar)) {
        sciprint("%s: argument #: Boolean variable\n", fname);
        err = bool_sci_to_jl(piAddressVar, ret);    
    }
    else if (isIntegerType(pvApiCtx, piAddressVar)) {
        sciprint("%s: argument #: Integer variable\n", fname);
        err = int_sci_to_jl(piAddressVar, ret);
    }
    else if (isStringType(pvApiCtx, piAddressVar)) {
        sciprint("%s: argument #: String variable\n", fname);
        err = string_sci_to_jl(piAddressVar, ret);
    }
    else if(isHypermatType(pvApiCtx, piAddressVar)) {
        sciprint("%s: argument #: Hypermat variable\n", fname);
        int varType;
        sciErr = getHypermatType(pvApiCtx, piAddressVar, &varType);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        sciprint("%s: argument #: Variable type %d \n", fname, varType);
        
        switch(varType) {
            case 1: // double
                err = double_sci_to_jl(piAddressVar, ret);
                break;
            case 4: // boolean
                err = bool_sci_to_jl(piAddressVar, ret);
                break;
            case 8: // integer
                err = int_sci_to_jl(piAddressVar, ret);
                break;
            case 10: // string
                err = string_sci_to_jl(piAddressVar, ret);
            default:
                break;
        }
    }
    else if(isSparseType(pvApiCtx, piAddressVar)) {
        sciprint("%s: argument #: Sparse variable\n", fname);
        err = sparse_sci_to_jl(piAddressVar, ret);
    }
    else {
        Scierror(999, "%s: argument # not implemented yet\n", fname);
        return 0;
    }
    if(err == 0) {
        return err;
    }
    return 1;
}

int sci_eval_julia(char *fname, unsigned long fname_len) {
    // Error management variable
    SciErr sciErr;

    ////////// Variables declaration //////////
    int *addrEvalString = NULL;
    char * string = NULL;

    int nInputArgs = nbInputArgument(pvApiCtx);
    int nOutputArgs = nbOutputArgument(pvApiCtx);

    ////////// Manage the first input argument (double) //////////
    /* get Address of inputs */
    sciErr = getVarAddressFromPosition(pvApiCtx, 1, &addrEvalString);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    if(!isStringType(pvApiCtx, addrEvalString)) {
        Scierror(999, "%s: argument #%d not a string: string expected\n", fname, 1);
        return 0;
    }

    int err = getAllocatedSingleString(pvApiCtx, addrEvalString, &string);
    if (err != 0)
    {
        return 0;
    }
    int i = 0;

    jl_value_t *ret = jl_eval_string(string);
    JL_GC_PUSH1(&ret);
    
    if (jl_exception_occurred()) {
        sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
        JL_GC_POP();
        return 0;
    }

    sciprint("%s: convert julia variables to scilab\n", fname);
    
    err = jl_to_sci(ret, 0);
    if (err == 0) {
        JL_GC_POP();
        Scierror(999, "%s: Error in convert julia variable to scilab\n", fname);
        return 0;
    }

    JL_GC_POP();
    sciprint("%s: exiting...\n", fname);

    return 0;
}

int sci_call_julia(char *fname, unsigned long fname_len) {
    // Error management variable
    SciErr sciErr;

    ////////// Variables declaration //////////
    int *addrFunctionName = NULL;
    char * functionName = NULL;

    int nInputArgs = nbInputArgument(pvApiCtx);
    int nOutputArgs = nbOutputArgument(pvApiCtx);

    ////////// Manage the first input argument (double) //////////
    /* get Address of inputs */
    sciErr = getVarAddressFromPosition(pvApiCtx, 1, &addrFunctionName);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    if(!isStringType(pvApiCtx, addrFunctionName)) {
        Scierror(999, "%s: argument #%d not a string: string expected\n", fname, 1);
        return 0;
    }

    int err = getAllocatedSingleString(pvApiCtx, addrFunctionName, &functionName);
    if (err != 0)
    {
        return 0;
    }    

    int i = 0;

    // get function using the function name provided
    jl_function_t *func = jl_get_function(jl_main_module, functionName);
    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }


    jl_value_t **inpArgs;
    JL_GC_PUSHARGS(inpArgs, nInputArgs - 1); 

    int *piAddressVar;
    for (i = 1; i != nInputArgs; i++) {
        sciErr = getVarAddressFromPosition(pvApiCtx, i + 1, &piAddressVar);
        if (sciErr.iErr)
        {
            JL_GC_POP();
            printError(&sciErr, 0);
            return 0;
        }
        
        err = sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
        if (err == 0) {
            JL_GC_POP();
            Scierror(999, "%s: Error in convert scilab variable to julia\n", fname);
            return 0;
        }
    }

    /* run Julia function */
    sciprint("%s: calling the actual function \n", fname);
    jl_value_t *ret = jl_call(func, inpArgs, nInputArgs - 1);
    JL_GC_POP();

    JL_GC_PUSH1(&ret);
    
    if (jl_exception_occurred()) {
        sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
        JL_GC_POP();
        return 0;
    }

    sciprint("%s: convert julia variables back to scilab\n", fname);
    err = jl_to_sci(ret, 0);
    if (err == 0) {
        JL_GC_POP();
        Scierror(999, "%s: Error in convert julia variable to scilab\n", fname);
        return 0;
    }

    JL_GC_POP();
    sciprint("%s: exiting...\n", fname);

    return 0;
}