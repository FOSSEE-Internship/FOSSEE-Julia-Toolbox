#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

extern int double_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int double_jl_to_sci(jl_value_t *input, int position);

extern int int_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int int_jl_to_sci(jl_value_t *input, int position);


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

    ////////// Julia Init Code //////////
    /* required: setup the Julia context */
    jl_init(NULL);

    // get function using the function name provided
    jl_function_t *func = jl_get_function(jl_base_module, functionName);
    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        jl_atexit_hook(0);
        return 0;
    }


    jl_value_t **inpArgs;
    JL_GC_PUSHARGS(inpArgs, nInputArgs - 1); 

    int *piAddressVar;
    for (i = 1; i != nInputArgs; i++) {
        sciErr = getVarAddressFromPosition(pvApiCtx, i + 1, &piAddressVar);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            jl_atexit_hook(0);
            return 0;
        }

        if (isIntegerType(pvApiCtx, piAddressVar)) {
            sciprint("%s: argument #%d: Integer array\n", fname, i);
            int_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
        }
        else if (isDoubleType(pvApiCtx, piAddressVar)) {
            sciprint("%s: argument #%d: Double array\n", fname, i);
            double_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
        }
        else {
            Scierror(999, "%s: argument #%d not implemented yet: double expected\n", fname, i + 1);
            jl_atexit_hook(0);
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
        jl_atexit_hook(0);
        return 0;
    }

    sciprint("%s: convert julia variables back to scilab\n", fname);
    if(jl_is_tuple(ret)) {
        sciprint("%s: multiple return values\n", functionName);
        size_t al = jl_nfields(ret);
        for (i = 0; i != al; i++) {
            jl_value_t *newargs = jl_fieldref(ret, i);
            
            if(jl_typeis(newargs, jl_apply_array_type(jl_float64_type, 2))) {
                sciprint("%s: Double 2D array\n", fname);
                err = double_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if(jl_typeis(newargs, jl_apply_array_type(jl_int8_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_uint8_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_int16_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_uint16_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_int32_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_uint32_type, 2)) ) {
                sciprint("%s: Integer 2D array\n", fname);

                err = int_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + 1);
            }
            else if(jl_typeis(newargs, jl_apply_array_type(jl_int64_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_int64_type, 2))) {
                JL_GC_POP();
                jl_atexit_hook(0);
                Scierror(999, "%s: integer64 types not supported in Scilab\n", fname);
            }
            else {
                JL_GC_POP();
                jl_atexit_hook(0);
                Scierror(999, "%s: non double types not implemented yet: double/integer return expected\n", fname);
            }

            if (err == 0) {
                JL_GC_POP();
                jl_atexit_hook(0);
                Scierror(999, "%s: error in converting julia variable to scilab variable\n", fname);
            }
            AssignOutputVariable(pvApiCtx, i + 1) = nbInputArgument(pvApiCtx) + i + 1;

        }
    }
    else {
        sciprint("%s: single return value\n", functionName);
        if(jl_typeis(ret, jl_float64_type) || jl_typeis(ret, jl_apply_array_type(jl_float64_type, 2))) {
            
            sciprint("%s: Float variable\n", fname);
            err = double_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
        }
        else if(jl_typeis(ret, jl_apply_array_type(jl_int8_type, 2)) || 
            jl_typeis(ret, jl_apply_array_type(jl_uint8_type, 2)) || 
            jl_typeis(ret, jl_apply_array_type(jl_int16_type, 2)) || 
            jl_typeis(ret, jl_apply_array_type(jl_uint16_type, 2)) || 
            jl_typeis(ret, jl_apply_array_type(jl_int32_type, 2)) || 
            jl_typeis(ret, jl_apply_array_type(jl_uint32_type, 2)) ) {
            
            sciprint("%s: Integer variable\n", fname);
            err = int_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
        }
        else if(jl_typeis(ret, jl_apply_array_type(jl_int64_type, 2)) || 
                jl_typeis(ret, jl_apply_array_type(jl_int64_type, 2))) {
                JL_GC_POP();
                jl_atexit_hook(0);
                Scierror(999, "%s: integer64 types not supported in Scilab\n", fname);
        }
        else {
            JL_GC_POP();
            jl_atexit_hook(0);
            Scierror(999, "%s: non double types not implemented yet: double return expected\n", fname);
        }
        
        if (err == 0) {
            JL_GC_POP();
            jl_atexit_hook(0);
            return 0;
        }

        AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;

    }

    JL_GC_POP();

    // sciprint("%s: exiting...");



    /* strongly recommended: notify Julia that the
         program is about to terminate. this allows
         Julia time to cleanup pending write requests
         and run all finalizers
    */
    jl_atexit_hook(0);

    sciprint("%s: exiting...");

    return 0;
}