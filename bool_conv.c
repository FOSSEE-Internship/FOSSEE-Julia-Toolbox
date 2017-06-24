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
        int ndims;
        int *dims;
        int xdims[2];
        int *data;
        int len = 1;

        if (isHypermatType(pvApiCtx, piAddressVar)) {
            
            sciprint("bool_sci_to_jl: Hypermat Boolean\n");

            sciErr = getHypermatOfBoolean(pvApiCtx, piAddressVar, &dims, &ndims, &data);
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }         
        }
        else {
            int m, n;
            sciprint("bool_sci_to_jl: matrix boolean argument");
            sciErr = getMatrixOfBoolean(pvApiCtx, piAddressVar, &m, &n, &data);
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

        sciprint("bool_sci_to_jl: size: (");
        for(int i = 0; i != ndims; i++) {
            len *= dims[i];
            sciprint("%d", dims[i]);
            if (i != ndims - 1) 
                sciprint(", ");
        }
        sciprint(")\n");
        

        jl_value_t *array_type = jl_apply_array_type(jl_bool_type, ndims);

        // copying data to julia data structure
        int8_t *xData = (int8_t*) malloc(sizeof(int8_t) * len);
        for(int i = 0; i != len; i++ ) 
            xData[i] = (int8_t) data[i];
        
        jl_value_t *types[ndims];
        for (int i = 0; i != ndims; i++)
            types[i] = (jl_value_t*)jl_long_type;

        jl_tupletype_t *tt = jl_apply_tuple_type_v(types, ndims);
        // typedef struct {
        //     ssize_t a[ndims];
        // } ntupleint;
        
        ssize_t *tuple = (ssize_t*)jl_new_struct_uninit(tt);
        JL_GC_PUSH1(&tuple);

        for (int i = 0; i != ndims; i++)
            (tuple)[i] = dims[i];

        *ret = (jl_value_t*) jl_ptr_to_array(array_type, xData, (jl_value_t*)tuple, 0);
        JL_GC_POP();

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
    if (jl_is_array(input)) {
        jl_array_t *matrix = (jl_array_t *) input;
        
        // Get number of dimensions
        int ndims = jl_array_ndims(matrix);

        if (jl_typeis(matrix, jl_apply_array_type(jl_bool_type, ndims))) {
            // Get the size of the matrix
            int dims[ndims];
            int len = jl_array_len(matrix);

            sciprint("%d \n", matrix);

            sciprint("bool_jl_to_sci: matrix boolean output\n");

            // data from the 
            int8_t *data = (int8_t*) jl_array_data(matrix);
            if (jl_exception_occurred())
                sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

            sciprint("size: (");
            for (int i = 0; i != ndims; i++){
                dims[i] = jl_array_dim(matrix, i);
                sciprint("%d, ", dims[i]);
            }
            sciprint(")\n");

            int *xData = malloc(len * sizeof(int));
            for (int i = 0; i != len; i++) {
                xData[i] = data[i];
                // if (data[i] == 1) 
                //     xData[i] = 1; 
                // else 
                //     xData[i] = 0; 
                // sciprint("%d: before: %d, after: %d\n", i, data[i], xData[i]);
            }

            if (ndims == 2)
                sciErr = createMatrixOfBoolean(pvApiCtx, position, dims[0], dims[1], xData);
            else 
                sciErr = createHypermatOfBoolean(pvApiCtx, position, dims, ndims, xData);
            free(xData);
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
        }
    }
    else if (jl_typeis(input, jl_bool_type)){
        int8_t data = jl_unbox_bool(input);
        sciprint("bool_jl_to_sci: scalar boolean output\n");
        err = createScalarBoolean(pvApiCtx, position, (int) data);
        if (err) {
            return 0;
        }
    }
    
    return 1;
}
