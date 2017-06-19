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
    else { 
        int ndims = 0;
        
        int xdims[2];
        int *dims;

        double *data;
        
        if (isHypermatType(pvApiCtx, piAddressVar)) {    
            sciprint("double_sci_to_jl: Hypermat Double\n");

            sciErr = getHypermatOfDouble(pvApiCtx, piAddressVar, &dims, &ndims, &data);
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
        }
        else {
            sciprint("double_sci_to_jl: Matrix Double\n");
            
            int m, n;

            // getting data from Scilab
            sciErr = getMatrixOfDouble(pvApiCtx, piAddressVar, &m, &n, &data);
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
            ndims = 2;
            dims = xdims;

            dims[0] = m;
            dims[1] = n;
        }

        int len = 1;
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

        

        *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
        JL_GC_POP();

        double *xData = (double*) jl_array_data(*ret);
        
        for (int i = 0; i != len; i++)
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

    if (jl_is_array(input)) {
        int ndims = jl_array_ndims(input);
        if(jl_typeis(input, jl_apply_array_type(jl_float64_type, ndims))) {
            int m, n;
            double *data;
            sciprint("double_jl_to_sci: argument #%d: Matrix Double\n", position);
            jl_array_t *matrix = (jl_array_t *) input;
            data = (double*) jl_array_data(matrix);
            if (jl_exception_occurred())
                sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

            // Get number of dimensions
            if (jl_exception_occurred())
                sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));



            // Get the size of the matrix
            int dims[ndims];

            sciprint("size: (");
            for (int i = 0; i != ndims; i++){
                dims[i] = jl_array_dim(matrix, i);
                sciprint("%d, ", dims[i]);
            }
            sciprint(")\n");

            m = jl_array_dim(matrix,0);
            n = jl_array_dim(matrix,1);
            
            int len = jl_array_len(matrix);

            // for (int i = 0; i != len; i++)
            //     sciprint("%f\n", data[i]);

            if (ndims == 2){
                sciErr = createMatrixOfDouble(pvApiCtx, position, dims[0], dims[1], data);
            }
            else {
                sciErr = createHypermatOfDouble(pvApiCtx, position, dims, ndims, data);
            }
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
        }
    }
    else if(jl_typeis(input, jl_float64_type)){
        sciprint("double_jl_to_sci: argument #%d: Scalar Double\n", position);
        double data = jl_unbox_float64(input);
        err = createScalarDouble(pvApiCtx, position, data);
        if (err) {
            return 0;
        }
    }
    else {
        jl_value_t *var_type = jl_typeof(input);
        ssize_t *x = (ssize_t*) var_type;
        sciprint("%d %d \n", x[0], x[1]);

        return 0;
    }

    


    return 1;
}
