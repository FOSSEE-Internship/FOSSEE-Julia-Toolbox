#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

int double_scilab_to_julia(int *piAddressVar, jl_value_t **ret) {
	// Error management variable
    SciErr sciErr;

	int m, n;
	double *data = NULL;
	sciErr = getMatrixOfDouble(pvApiCtx, piAddressVar, &m, &n, &data);
	if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    jl_value_t *int_array_type = jl_apply_array_type(jl_int32_type, 1);
    JL_GC_PUSH1(&int_array_type);
    int *dimensions = (int*) malloc(sizeof(int) * 2);
    dimensions[0] = m;
    dimensions[1] = n;
    jl_value_t *dims = (jl_value_t*) jl_ptr_to_array_1d(int_array_type, dimensions, 2, 0);
    JL_GC_PUSH1(&dims);

    jl_value_t *array_type = jl_apply_array_type(jl_float64_type, 2);
    JL_GC_PUSH1(&array_type);
    *ret  = (jl_value_t *) jl_ptr_to_array(array_type, data, dims, 0);
    JL_POP();
    JL_POP();
    JL_POP();
    return 1;

}

int double_julia_to_scilab(jl_value_t *input, int position) {
	SciErr sciErr;

	jl_array_t *matrix = (jl_array_t *) input;

	int m, n;
	double *data = (double*) jl_array_data(matrix);

	// Get number of dimensions
	int ndims = jl_array_ndims(matrix);
	// Get the size of the i-th dim
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