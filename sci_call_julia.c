#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

extern int double_scilab_to_julia(int *piAddressVar, jl_value_t **ret);
extern int double_julia_to_scilab(jl_value_t *input, int position);


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
        Scierror(999, "%s: argument #1 not a string: string expected\n", fname, 1);
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
    if (jl_exception_occurred())
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
    
    jl_value_t **inpArgs;
    JL_GC_PUSHARGS(inpArgs, nInputArgs); 

    int *piAddressVar;
    for (i = 1; i != nInputArgs; i++) {
        sciErr = getVarAddressFromPosition(pvApiCtx, i + 1, &piAddressVar);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if (isDoubleType(pvApiCtx, piAddressVar)) {
            double_scilab_to_julia(piAddressVar, &(inpArgs[i]));
        }
    }

    /* run Julia commands */
    jl_value_t *ret = jl_call(functionName, inpArgs, nInputArgs - 1);
    JL_GC_PUSH1(&ret);
    double_julia_to_scilab(ret, nbInputArgument(pvApiCtx) + 1);
    JL_GC_POP();
    JL_GC_POP();
    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;

    // if (jl_is_type(ret)) {
        // ret_unboxed = jl_unbox_float64(ret);
    // }

    /* strongly recommended: notify Julia that the
         program is about to terminate. this allows
         Julia time to cleanup pending write requests
         and run all finalizers
    */
    jl_atexit_hook(0);


    // TODO : Check input/output arguments
    ////////// Check the number of input and output arguments //////////
    /* --> [c, d, e] = matopt(a, b) */
    /* check that we have only 2 input arguments */
    /* check that we have only 3 output argument */

    // CheckInputArgument(pvApiCtx, 2, 2) ;
    // CheckOutputArgument(pvApiCtx, 4, 4) ;


    
    return 0;
}