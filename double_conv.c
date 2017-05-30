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
        
        jl_value_t *types[] = {(jl_value_t*)jl_long_type, (jl_value_t*)jl_long_type};
        jl_tupletype_t *tt = jl_apply_tuple_type_v(types, 2);
        typedef struct {
            ssize_t a;
            ssize_t b;
        } ntuple2int;
        ntuple2int *tuple = (ntuple2int*)jl_new_struct_uninit(tt);
        JL_GC_PUSH1(&tuple);
        tuple->a = m;
        tuple->b = n;

        *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
        JL_GC_POP();
    }

    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }

    return 1;
}


int double_jl_to_sci(jl_value_t *input, int position) {
    SciErr sciErr;

    int m, n;
    if(jl_typeis(input, jl_float64_type)){
        double data = jl_unbox_float64(input);
        sciErr = createMatrixOfDouble(pvApiCtx, position, 1, 1, &data);        
    }
    else if(jl_typeis(input, jl_apply_array_type(jl_float64_type, 2))) {
        double *data;
        jl_array_t *matrix = (jl_array_t *) input;
        data = (double*) jl_array_data(matrix);
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
    }

    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }


    return 1;
}
