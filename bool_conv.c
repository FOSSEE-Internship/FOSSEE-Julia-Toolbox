#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

int bool_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;

    // getting data from Scilab
    int m, n;
    int8_t *data = NULL;
    sciErr = getMatrixOfBoolean(pvApiCtx, piAddressVar, &m, &n, &data);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }


    jl_value_t *array_type = jl_apply_array_type(jl_bool_type, 2);
    
    // TODO: rather than creating a new julia variable
    // just create a thin wrapper using 'jl_ptr_to_array'

    // *ret = (jl_value_t *) jl_ptr_to_array(array_type, data, dims, 0);
    *ret = (jl_value_t*) jl_alloc_array_2d(array_type, m, n);


    // copying data to julia data structure
    int8_t *xData = (int8_t*) jl_array_data(*ret);
    for(int i = 0; i != m * n; i++ ) 
        xData[i] = data[i];

    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }
    return 1;

}

int bool_jl_to_sci(jl_value_t *input, int position) {
    // Error management
    SciErr sciErr;

    jl_array_t *matrix = (jl_array_t *) input;
    sciprint("%d \n", matrix);

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


    return 1;
}
