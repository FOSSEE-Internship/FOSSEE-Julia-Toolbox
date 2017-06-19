#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

int double_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;
    int err;

    if(isScalar(pvApiCtx, piAddressVar)){
        sciprint("double_sci_to_jl: Scalar Double\n");
        double data;
        err = getScalarDouble(pvApiCtx, piAddressVar, &data);
        if (err)
        {
            return 0;
        }

        *ret = jl_box_float64(data);
    }
    else if (isHypermatType(pvApiCtx, piAddressVar)) {
        int *dims;
        int ndims;
        double *data;
        int len = 1;
        
        sciprint("double_sci_to_jl: Hypermat Double\n");

        sciErr = getHypermatOfDouble(pvApiCtx, piAddressVar, &dims, &ndims, &data);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        sciprint("double_sci_to_jl: size: (");
        for(int i = 0; i != ndims; i++) {
            len *= dims[i];
            sciprint("%d", dims[i]);
            if (i != ndims - 1) 
                sciprint(", ");
        }
        sciprint(")\n");

        jl_value_t *array_type = jl_apply_array_type(jl_float64_type, ndims);

        // *ret = jl_alloc_array_2d(array_type, m, n);
        // double *xData = (double*) jl_array_data(ret);

        // for (int i = 0; i != m * n; i++) 
        //     xData[i] = data[i];
        
        jl_value_t *types[ndims];
        for (int i = 0; i != ndims; i++)
            types[i] = (jl_value_t*)jl_long_type;

        jl_tupletype_t *tt = jl_apply_tuple_type_v(types, ndims);
        typedef struct {
            ssize_t a[ndims];
        } ntupleint;
        
        ntupleint *tuple = (ntupleint*)jl_new_struct_uninit(tt);
        JL_GC_PUSH1(&tuple);

        
        for (int i = 0; i != ndims; i++)
            (tuple->a)[i] = dims[i];

        size_t *temp = (size_t*) tuple;

        *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
        JL_GC_POP();

        double *xData = (double*) jl_array_data(ret);

        for (int i = 0; i != len; i++)
            sciprint("%f\n", xData[i]);

    }
    else {
        int m, n;
        double *data = NULL;
        sciprint("double_sci_to_jl: Matrix Double\n");
        
        // getting data from Scilab
        sciErr = getMatrixOfDouble(pvApiCtx, piAddressVar, &m, &n, &data);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        jl_value_t *array_type = jl_apply_array_type(jl_float64_type, 2);

        // *ret = jl_alloc_array_2d(array_type, m, n);
        // double *xData = (double*) jl_array_data(ret);

        // for (int i = 0; i != m * n; i++) 
        //     xData[i] = data[i];
        
        jl_value_t *types[] = {(jl_value_t*)jl_long_type, (jl_value_t*)jl_long_type};
        jl_tupletype_t *tt = jl_apply_tuple_type_v(types, 2);
        typedef struct {
            ssize_t a[2];
        } ntuple2int;
        
        ntuple2int *tuple = (ntuple2int*)jl_new_struct_uninit(tt);
        JL_GC_PUSH1(&tuple);

        (tuple->a)[0] = m;
        (tuple->a)[1] = n;

        *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
        JL_GC_POP();

        double *xData = (double*) jl_array_data(ret);

        for (int i = 0; i != m * n; i++) 
            sciprint("%f\n", xData[i]);
    }

    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }

    return 1;
}


int double_jl_to_sci(jl_value_t *input, int position) {
    SciErr sciErr;
    int err;

    if(jl_typeis(input, jl_float64_type)){
        sciprint("double_jl_to_sci: argument #%d: Scalar Double\n", position);
        double data = jl_unbox_float64(input);
        err = createScalarDouble(pvApiCtx, position, data);
        if (err) {
            return 0;
        }      
    }
    else if(jl_typeis(input, jl_apply_array_type(jl_float64_type, 2))) {
        int m, n;
        double *data;
        sciprint("double_jl_to_sci: argument #%d: Matrix Double\n", position);
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

        for (int i = 0; i != m * n; i++)
            sciprint("%f\n", data[i]);
        
        sciErr = createMatrixOfDouble(pvApiCtx, position, m, n, data);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
    }
    else {
        return 0;
    }

    


    return 1;
}
