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
    if (jl_exception_occurred())
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));


    jl_value_t **inpArgs;
    JL_GC_PUSHARGS(inpArgs, nInputArgs - 1); 

    int *piAddressVar;
    for (i = 1; i != nInputArgs; i++) {
        sciErr = getVarAddressFromPosition(pvApiCtx, i + 1, &piAddressVar);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        if (isDoubleType(pvApiCtx, piAddressVar)) {
            double_scilab_to_julia(piAddressVar, &(inpArgs[i - 1]));
        }
        else {
            Scierror(999, "%s: argument #%d not implemented yet: double expected\n", fname, i + 1);
            return 0;
        }
    }

    /* run Julia function */
    sciprint("Calling the actual function \n");
    jl_value_t *ret = jl_call(func, inpArgs, nInputArgs - 1);
    
    JL_GC_POP();
    JL_GC_PUSH1(&ret);
    
    if(jl_is_tuple(ret)) {
        sciprint("%s: multiple return values\n", functionName);
    }
    else {
        sciprint("%s: single return value\n", functionName);
    }

    sciprint("Convert julia variables back to scilab\n");
    double_julia_to_scilab(ret, nbInputArgument(pvApiCtx) + 1);
    JL_GC_POP();


    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;

    /* strongly recommended: notify Julia that the
         program is about to terminate. this allows
         Julia time to cleanup pending write requests
         and run all finalizers
    */
    jl_atexit_hook(0);

    return 0;
}