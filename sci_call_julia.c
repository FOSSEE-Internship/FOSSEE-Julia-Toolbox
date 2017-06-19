#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

extern int double_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int double_jl_to_sci(jl_value_t *input, int position);

extern int int_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int int_jl_to_sci(jl_value_t *input, int position);

extern int bool_sci_to_jl(int *piAddressVar, jl_value_t **ret);
extern int bool_jl_to_sci(jl_value_t *input, int position);


int sci_init_julia(char* fname, unsigned long fname_len) {
    
    ////////// Julia Init Code //////////
    /* required: setup the Julia context */
    jl_init(NULL);
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
    jl_function_t *func = jl_get_function(jl_base_module, functionName);
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
        
        int iType = 0;
        sciErr = getVarType(pvApiCtx, piAddressVar, &iType);

        if (sciErr.iErr)
        {
            JL_GC_POP();
            printError(&sciErr, 0);
            return 0;
        }

        sciprint("%s: argument #%d: VarType: %d\n", fname, i + 1, iType);
        if (isDoubleType(pvApiCtx, piAddressVar)) {
            sciprint("%s: argument #%d: Double variable\n", fname, i + 1);
            err = double_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
        }
        else if (isBooleanType(pvApiCtx, piAddressVar)) {
            sciprint("%s: argument #%d: Boolean variable\n", fname, i + 1);
            err = bool_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));    
        }
        else if (isIntegerType(pvApiCtx, piAddressVar)) {
            sciprint("%s: argument #%d: Integer variable\n", fname, i + 1);
            err = int_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
        }
        else if(isHypermatType(pvApiCtx, piAddressVar)) {
            sciprint("%s: argument #%d: Hypermat variable\n", fname, i + 1);
            int varType;
            sciErr = getHypermatType(pvApiCtx, piAddressVar, &varType);
            if (sciErr.iErr)
            {
                JL_GC_POP();
                printError(&sciErr, 0);
                return 0;
            }

            sciprint("%s: argument #%d: Variable type %d \n", fname, i + 1, varType);
            
            switch(varType) {
                case 1: // double
                    err = double_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
                    break;
                case 4: // boolean
                    err = bool_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
                    break;
                case 8: // integer
                    err = int_sci_to_jl(piAddressVar, &(inpArgs[i - 1]));
                    break;
                default:
                    break;
            }
        }
        else {
            Scierror(999, "%s: argument #%d not implemented yet\n", fname, i + 1);
            return 0;
        }
        if(err == 0) {
            JL_GC_POP();
            return err;
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
    if(jl_is_tuple(ret)) {
        sciprint("%s: multiple return values\n", functionName);
        size_t al = jl_nfields(ret);
        for (i = 0; i != al; i++) {
            jl_value_t *newargs = jl_fieldref(ret, i);
            
            if(jl_typeis(newargs, jl_float64_type) ||
                jl_typeis(newargs, jl_apply_array_type(jl_float64_type, 2))) {
                
                sciprint("%s: Double variable\n", fname);
                err = double_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else if(jl_typeis(newargs, jl_bool_type) || 
                jl_typeis(newargs, jl_apply_array_type(jl_bool_type, 2))) {

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
                jl_typeis(newargs, jl_uint64_type) || 

                jl_typeis(newargs, jl_apply_array_type(jl_int8_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_uint8_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_int16_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_uint16_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_int32_type, 2)) || 
                jl_typeis(newargs, jl_apply_array_type(jl_uint32_type, 2)) ||
                jl_typeis(newargs, jl_apply_array_type(jl_int64_type, 2)) ||
                jl_typeis(newargs, jl_apply_array_type(jl_uint64_type, 2))) {
                
                sciprint("%s: Integer variable\n", fname);

                err = int_jl_to_sci(newargs, nbInputArgument(pvApiCtx) + i + 1);
            }
            else {
                JL_GC_POP();
                Scierror(999, "%s: non double types not implemented yet: double/integer return expected\n", fname);
                return 0;
            }

            if (err == 0) {
                JL_GC_POP();
                Scierror(999, "%s: error in converting julia variable to scilab variable\n", fname);
                return 0;
            }
            sciprint("%s: assigning output variable #%d\n", fname, i + 1);
            AssignOutputVariable(pvApiCtx, i + 1) = nbInputArgument(pvApiCtx) + i + 1;

        }
    }
    else {
        sciprint("%s: single return value\n", functionName);
        if(jl_is_array(ret)) {
            int ndims = jl_array_ndims(ret);

            if (jl_typeis(ret, jl_apply_array_type(jl_float64_type, ndims))) {
                sciprint("%s: Float variable\n", fname);
                err = double_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
            }
            else if (jl_typeis(ret, jl_apply_array_type(jl_bool_type, ndims))) {
                sciprint("%s: Boolean variable\n", fname);
                err = bool_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
            }
            else if (jl_typeis(ret, jl_apply_array_type(jl_int8_type, ndims)) || 
                jl_typeis(ret, jl_apply_array_type(jl_uint8_type, ndims)) || 
                jl_typeis(ret, jl_apply_array_type(jl_int16_type, ndims)) || 
                jl_typeis(ret, jl_apply_array_type(jl_uint16_type, ndims)) || 
                jl_typeis(ret, jl_apply_array_type(jl_int32_type, ndims)) || 
                jl_typeis(ret, jl_apply_array_type(jl_uint32_type, ndims)) ||
                jl_typeis(ret, jl_apply_array_type(jl_int64_type, ndims)) ||
                jl_typeis(ret, jl_apply_array_type(jl_uint64_type, ndims)) ) {

                sciprint("%s: Integer variable\n", fname);
                err = int_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
            }
        }
        else {
            if(jl_typeis(ret, jl_float64_type)) {
                sciprint("%s: Float variable\n", fname);
                err = double_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
            }
            else if(jl_typeis(ret, jl_bool_type)) {

                sciprint("%s: Boolean variable\n", fname);
                err = bool_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
            }
            else if(jl_typeis(ret, jl_int8_type) || 
                    jl_typeis(ret, jl_uint8_type) || 
                    jl_typeis(ret, jl_int16_type) || 
                    jl_typeis(ret, jl_uint16_type) || 
                    jl_typeis(ret, jl_int32_type) || 
                    jl_typeis(ret, jl_uint32_type) || 
                    jl_typeis(ret, jl_int64_type) || 
                    jl_typeis(ret, jl_uint64_type) ) {
                
                sciprint("%s: Integer variable\n", fname);
                err = int_jl_to_sci(ret, nbInputArgument(pvApiCtx) + 1);
            }
            else {
                jl_value_t *var_type = jl_typeof(ret);

                JL_GC_POP();
                Scierror(999, "%s: non double types not implemented yet: double return expected\n", fname);
                return 0;
            }
        }

        
        if (err == 0) {
            JL_GC_POP();
            return 0;
        }

        sciprint("%s: assigning output variable #%d\n", fname, 1);
        AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;

    }

    JL_GC_POP();
    sciprint("%s: exiting...\n", fname);

    return 0;
}