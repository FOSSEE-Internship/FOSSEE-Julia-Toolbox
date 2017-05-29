#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

int double_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;

    // getting data from Scilab

    int m, n;
    double *data = NULL;
    sciErr = getMatrixOfDouble(pvApiCtx, piAddressVar, &m, &n, &data);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    if (m == 1 && n == 1) {
        *ret = jl_box_float64(*data);
    }
    else {
        jl_value_t *array_type = jl_apply_array_type(jl_float64_type, 2);
        
        // TODO: rather than creating a new julia variable
        // just create a thin wrapper using 'jl_ptr_to_array'

        // *ret = (jl_value_t *) jl_ptr_to_array(array_type, data, dims, 0);
        *ret = (jl_value_t*) jl_alloc_array_2d(array_type, m, n);


        // copying data to julia data structure
        double *xData = (double*) jl_array_data(*ret);
        for(int i = 0; i != m * n; i++ ) 
            xData[i] = data[i];
            // sciprint("%f\n", xData[i]);
    }

    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }
    
    

    return 1;
}


int double_jl_to_sci(jl_value_t *input, int position) {
    SciErr sciErr;

    jl_array_t *matrix = (jl_array_t *) input;

    int m, n;
    double *data = (double*) jl_array_data(matrix);
    if (jl_exception_occurred())
        sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

    // Get number of dimensions
    int ndims = jl_array_ndims(matrix);
    if (jl_exception_occurred())
        sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

    // Get the size of the matrix
    m = jl_array_dim(matrix,0);
    n = jl_array_dim(matrix,1);

    sciErr = createMatrixOfDouble(pvApiCtx, position, m, n, data);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }


    return 1;
}
