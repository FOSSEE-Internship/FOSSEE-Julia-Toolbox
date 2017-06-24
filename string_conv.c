#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

int string_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;
    int err;

    if(isScalar(pvApiCtx, piAddressVar)){
        sciprint("string_sci_to_jl: Scalar String\n");
        char *data;
        err = getAllocatedSingleString(pvApiCtx, piAddressVar, &data);
        if (err)
        {
            return 0;
        }

        *ret = jl_cstr_to_string(data);
    }
    else { 
        int ndims = 0;
        
        int xdims[2];
        int *dims;

        char **data;
        int stringLengths;
        
        if (isHypermatType(pvApiCtx, piAddressVar)) {    
            sciprint("string_sci_to_jl: Hypermat String\n");

            sciErr = getHypermatOfString(pvApiCtx, piAddressVar, &dims, &ndims, &stringLengths, &data);
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
        }
        else {
            sciprint("string_sci_to_jl: Matrix String\n");
            
            int m, n;
            int *strLen;

            // getting data from Scilab

            //first call to retrieve dimensions
            sciErr = getMatrixOfString(pvApiCtx, piAddressVar, &m, &n, NULL, NULL);
            if(sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }

            strLen = (int*)malloc(sizeof(int) * m * n);

            //second call to retrieve length of each string
            sciErr = getMatrixOfString(pvApiCtx, piAddressVar, &m, &n, strLen, NULL);
            if(sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }

            data = (char**)malloc(sizeof(char*) * m * n);
            for(int i = 0 ; i < m * n ; i++)
            {
                data[i] = (char*)malloc(sizeof(char) * (strLen[i] + 1));//+ 1 for null termination
            }

            //third call to retrieve data
            sciErr = getMatrixOfString(pvApiCtx, piAddressVar, &m, &n, strLen, data);
            if(sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }


            // err = getAllocatedMatrixOfString(pvApiCtx, piAddressVar, &m, &n, &data);
            // if (err == 0)
            // {
            //     sciprint("string_sci_to_jl: couldn't get strings from scilab\n");
            //     return 0;
            // }

            ndims = 2;
            dims = xdims;

            dims[0] = m;
            dims[1] = n;
        }

        int len = 1;
        sciprint("string_sci_to_jl: size: (");
        for(int i = 0; i != ndims; i++) {
            len *= dims[i];
            sciprint("%d", dims[i]);
            if (i != ndims - 1) 
                sciprint(", ");
        }
        sciprint(")\n");

        jl_value_t *array_type = jl_apply_array_type(jl_string_type, ndims);

        // *ret = jl_alloc_array_2d(array_type, m, n);
        // char **xData = (char **) jl_array_data(ret);

        // for (int i = 0; i != m * n; i++) 
        //     xData[i] = data[i];
        


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

        
        *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
        JL_GC_POP();

        char **xData = (char **) jl_array_data(*ret);

        for (int i = 0; i != len; i++)
            sciprint("%s\n", xData[i]);
    }


    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }

    return 1;
}


int string_jl_to_sci(jl_value_t *input, int position) {
    SciErr sciErr;
    int err;

    if (jl_is_array(input)) {
        jl_array_t *matrix = (jl_array_t *) input;
        
        // Get number of dimensions
        int ndims = jl_array_ndims(matrix);
        
        if(jl_typeis(matrix, jl_apply_array_type(jl_string_type, ndims))) {
            // Get the size of the matrix
            int dims[ndims];
            int len = jl_array_len(matrix);
            char **data;
            
            sciprint("string_jl_to_sci: argument #%d: Matrix String\n", position);

            data = (char**) jl_array_data(matrix);
            if (jl_exception_occurred())
                sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));


            sciprint("size: (");
            for (int i = 0; i != ndims; i++){
                dims[i] = jl_array_dim(matrix, i);
                sciprint("%d, ", dims[i]);
            }
            sciprint(")\n");
            

            // for (int i = 0; i != len; i++)
            //     sciprint("%f\n", data[i]);

            if (ndims == 2){
                sciErr = createMatrixOfString(pvApiCtx, position, dims[0], dims[1], data);
            }
            else {
                sciErr = createHypermatOfString(pvApiCtx, position, dims, ndims, data);
            }
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
        }
    }
    else if(jl_typeis(input, jl_string_type)){
        sciprint("string_jl_to_sci: argument #%d: Scalar String\n", position);
        char *data = jl_string_data(input);
        sciprint("data: %f\n", data);
        err = createSingleString(pvApiCtx, position, data);
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
