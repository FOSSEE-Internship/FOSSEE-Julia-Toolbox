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
            sciprint("bool_sci_to_jl: matrix boolean argument\n");
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
        

        jl_value_t *array_type = jl_apply_array_type((jl_value_t*)jl_bool_type, ndims);

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
    if (jl_is_array(input) || jl_isa(input, jl_get_global(jl_base_module, jl_symbol("BitArray")))) {
        int ndims;
        int *dims;
        int *data;

        if (jl_is_array(input)) { 
            jl_array_t *matrix = (jl_array_t *) input;
            
            // Get number of dimensions
            int ndims = jl_array_ndims(matrix);

            // Get the size of the matrix
            dims = (int*)malloc(ndims * sizeof(int));
            int len = jl_array_len(matrix);

            sciprint("%d \n", matrix);

            sciprint("bool_jl_to_sci: matrix boolean output\n");

            // data from the 
            int8_t *zData = (int8_t*) jl_array_data(matrix);
            if (jl_exception_occurred())
                sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

            sciprint("size: (");
            for (int i = 0; i != ndims; i++){
                dims[i] = jl_array_dim(matrix, i);
                sciprint("%d, ", dims[i]);
            }
            sciprint(")\n");

            data = malloc(len * sizeof(int));
            for (int i = 0; i != len; i++) {
                data[i] = zData[i];
                // if (zData[i] == 1) 
                //     data[i] = 1; 
                // else 
                //     data[i] = 0; 
                // sciprint("%d: before: %d, after: %d\n", i, zData[i], data[i]);
            }
        }
        else if (jl_isa(input, jl_get_global(jl_base_module, jl_symbol("BitArray")))) {
            typedef struct {
                jl_array_t *chunks;
                int64_t len;
                int64_t *dims;
            } jl_bit_array_t;

            jl_bit_array_t *ret = (jl_bit_array_t*) input;

            int64_t len = ret->len;
            sciprint("%s: BitArray, len: %d \n", "bool_jl_to_sci", len);
            uint64_t *zData = (uint64_t*) jl_array_data(ret->chunks);
            for (int i = 0; i != len; i++) 
                sciprint("%d, ", zData[i/64] & 1 << (i%64));
            sciprint("\n");

            int64_t *dimensions = &(ret->dims);
            ndims = 0;
            while(dimensions[ndims] != 0) {
                ndims ++;
            }
            // sciprint("ndims: %d\n", sizeof(dimensions)/sizeof(int32_t));
            // sciprint("dims: %s type \n",jl_typeof_str(ret->dims));
            // ndims = jl_nfields(dimensions);

            dims = (int*) malloc(ndims * sizeof(int));
            for (int i = 0; i != ndims; i++){
                dims[i] = dimensions[i];
                // if (i == 0)
                //     dims[i] = len;
                // else 
                //     dims[i] = 1;
                // dims[i] = jl_unbox_int64(jl_get_field(dimensions, i));
            }

            data = (int*) malloc(len * sizeof(int));
            for (int i = 0; i != len; i++) {
                data[i] = zData[i/64] & 1 << (i%64);
            }

            sciprint("size: (");
            for (int i = 0; i != ndims; i++){
                sciprint("%d, ", dims[i]);
            }
            sciprint(")\n");
        }

        if (ndims == 2)
            sciErr = createMatrixOfBoolean(pvApiCtx, position, dims[0], dims[1], data);
        else 
            sciErr = createHypermatOfBoolean(pvApiCtx, position, dims, ndims, data);
        free(data);
        free(dims);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
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
