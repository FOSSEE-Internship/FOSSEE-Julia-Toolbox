#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

int bool_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;
    int err;

    // getting data from Scilab
    
    if(isScalar(pvApiCtx, piAddressVar)) {
        int data;
        sciprint("bool_sci_to_jl: scalar boolean argument");
        err = getScalarBoolean(pvApiCtx, piAddressVar, &data);
        if (err) {
            return 0;
        }

        *ret = jl_box_bool((int8_t) data);
    }
    else {
        int m, n;
        int *data = NULL;
        sciprint("bool_sci_to_jl: matrix boolean argument");
        sciErr = getMatrixOfBoolean(pvApiCtx, piAddressVar, &m, &n, &data);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        jl_value_t *array_type = jl_apply_array_type(jl_bool_type, 2);
        
        // cannot just copy the pointer to Julia
        *ret = (jl_value_t*) jl_alloc_array_2d(array_type, m, n);


        // copying data to julia data structure
        int8_t *xData = (int8_t*) jl_array_data(*ret);
        for(int i = 0; i != m * n; i++ ) 
            xData[i] = (int8_t) data[i];

    }


    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }
    return 1;

}

int bool_jl_to_sci(jl_value_t *input, int position) {
    // Error management
    SciErr sciErr;
    int err;

    if (jl_typeis(input, jl_bool_type)){
        int8_t data = jl_unbox_bool(input);
        sciprint("bool_jl_to_sci: scalar boolean output");
        err = createScalarBoolean(pvApiCtx, position, (int) data);
        if (err) {
            return 0;
        }
    }
    else if (jl_typeis(input, jl_apply_array_type(jl_bool_type, 2))) {
        jl_array_t *matrix = (jl_array_t *) input;
        sciprint("%d \n", matrix);

        sciprint("bool_jl_to_sci: matrix boolean output");

        // data from the 
        int m, n;
        int8_t *data = (int8_t*) jl_array_data(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

        // Get number of dimensions
        int ndims = jl_array_ndims(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

        // Get the size of the matrix
        m = jl_array_dim(matrix,0);
        n = jl_array_dim(matrix,1);

        sciErr = createMatrixOfBoolean(pvApiCtx, position, m, n, data);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
    }
    return 1;
}
