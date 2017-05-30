#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

int int_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;

    // getting data from Scilab
    int m, n;
    void *data;

    int precision = 0;
    sciErr = getMatrixOfIntegerPrecision(pvApiCtx, piAddressVar, &precision);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    if (precision > 10){
        if(precision%10 == 1){
            unsigned char *temp;
            sciErr = getMatrixOfUnsignedInteger8(pvApiCtx, piAddressVar, &m, &n, &temp);
            data = temp;
        }
        else if(precision%10 == 2){
            short unsigned int *temp;
            sciErr = getMatrixOfUnsignedInteger16(pvApiCtx, piAddressVar, &m, &n, &temp);
            data = temp;
        }
        else if(precision%10 == 4){
            unsigned int *temp;
            sciErr = getMatrixOfUnsignedInteger32(pvApiCtx, piAddressVar, &m, &n, &temp);
            data = temp;
        }
    }
    else {
        if(precision%10 == 1){
            char *temp;
            sciErr = getMatrixOfInteger8(pvApiCtx, piAddressVar, &m, &n, &temp);
            data = temp;
        }
        else if(precision%10 == 2){
            short int *temp;
            sciErr = getMatrixOfInteger16(pvApiCtx, piAddressVar, &m, &n, &temp);
            data = temp;
        }
        else if(precision%10 == 4){
            int *temp;
            sciErr = getMatrixOfInteger32(pvApiCtx, piAddressVar, &m, &n, &temp);
            data = temp;
        }
    }
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    if (m == 1 && n == 1) {
        if (precision > 10){
            if(precision%10 == 1){
                *ret = jl_box_uint8(*(uint8_t*)data);
            }
            else if(precision%10 == 2){
                *ret = jl_box_uint16(*(uint16_t*)data);
            }
            else if(precision%10 == 4){
                *ret = jl_box_uint32(*(uint32_t*)data);
            }
        }
        else {
            if(precision%10 == 1){
                *ret = jl_box_int8(*(int8_t*)data);
            }
            else if(precision%10 == 2){
                *ret = jl_box_int16(*(int16_t*)data);
            }
            else if(precision%10 == 4){
                *ret = jl_box_int32(*(int32_t*)data);
            }
        }
    }
    else {
        jl_value_t *array_type;

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

        if (precision > 10){
            if(precision%10 == 1){
                array_type = jl_apply_array_type(jl_uint8_type, 2);
            }
            else if(precision%10 == 2){
                array_type = jl_apply_array_type(jl_uint16_type, 2);
            }
            else if(precision%10 == 4){
                array_type = jl_apply_array_type(jl_uint32_type, 2);
            }
        }
        else {
            if(precision%10 == 1){
                array_type = jl_apply_array_type(jl_int8_type, 2);
            }
            else if(precision%10 == 2){
                array_type = jl_apply_array_type(jl_int16_type, 2);
            }
            else if(precision%10 == 4){
                array_type = jl_apply_array_type(jl_int32_type, 2);
            }
        }

        *ret = (jl_value_t *) jl_ptr_to_array(array_type, data, (jl_value_t*) tuple, 0);   
        JL_GC_POP();     
    }

    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }
    
    

    return 1;
}


int int_jl_to_sci(jl_value_t *input, int position) {
    SciErr sciErr;

    jl_array_t *matrix = (jl_array_t *) input;

    int m, n;
    // Get number of dimensions
    int ndims = jl_array_ndims(matrix);
    if (jl_exception_occurred())
        sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

    // Get the size of the matrix
    m = jl_array_dim(matrix,0);
    n = jl_array_dim(matrix,1);

    if(jl_typeis(input, jl_apply_array_type(jl_int8_type, 2))) {
        int8_t *data = (int8_t*) jl_array_data(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

        sciErr = createMatrixOfInteger8(pvApiCtx, position, m, n, data);
    }
    else if(jl_typeis(input, jl_apply_array_type(jl_int16_type, 2))) {
        int16_t *data = (int16_t*) jl_array_data(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

        sciErr = createMatrixOfInteger16(pvApiCtx, position, m, n, data);
    }
    else if(jl_typeis(input, jl_apply_array_type(jl_int32_type, 2))) {
        int32_t *data = (int32_t*) jl_array_data(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

        sciErr = createMatrixOfInteger32(pvApiCtx, position, m, n, data);
    }
    else if(jl_typeis(input, jl_apply_array_type(jl_uint8_type, 2))) {
        uint8_t *data = (uint8_t*) jl_array_data(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
        
        sciErr = createMatrixOfUnsignedInteger8(pvApiCtx, position, m, n, data);
    }
    else if(jl_typeis(input, jl_apply_array_type(jl_uint16_type, 2))) {
        uint16_t *data = (uint16_t*) jl_array_data(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
        
        sciErr = createMatrixOfUnsignedInteger16(pvApiCtx, position, m, n, data);
    }
    else if(jl_typeis(input, jl_apply_array_type(jl_uint32_type, 2))) {
        uint32_t *data = (uint32_t*) jl_array_data(matrix);
        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
        
        sciErr = createMatrixOfUnsignedInteger32(pvApiCtx, position, m, n, data);
    }
    else {
        Scierror(999, "integer_conv: not a correct integer variable");
    }

    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }


    return 1;
}
