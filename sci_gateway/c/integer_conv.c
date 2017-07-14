#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"
#include <limits.h>


int int_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;
    int err;

    int precision = 0;
    if (isHypermatType(pvApiCtx, piAddressVar)) 
        sciErr = getHypermatOfIntegerPrecision(pvApiCtx, piAddressVar, &precision);
    else 
        sciErr = getMatrixOfIntegerPrecision(pvApiCtx, piAddressVar, &precision);

    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }
    
    if (isScalar(pvApiCtx, piAddressVar)) {

        if (precision > 10){
            if(precision%10 == 1){
                unsigned char data;
                err = getScalarUnsignedInteger8(pvApiCtx, piAddressVar, &data);
                *ret = jl_box_uint8((uint8_t)data);
            }
            else if(precision%10 == 2){
                short unsigned int data;
                err = getScalarUnsignedInteger16(pvApiCtx, piAddressVar, &data);
                *ret = jl_box_uint16((uint16_t)data);
            }
            else if(precision%10 == 4){
                unsigned int data;
                err = getScalarUnsignedInteger32(pvApiCtx, piAddressVar, &data);
                *ret = jl_box_uint32((uint32_t)data);
            }
        }
        else {
            if(precision%10 == 1){
                char data;
                err = getScalarInteger8(pvApiCtx, piAddressVar, &data);
                *ret = jl_box_int8((int8_t)data);
            }
            else if(precision%10 == 2){
                short int data;
                err = getScalarInteger16(pvApiCtx, piAddressVar, &data);
                *ret = jl_box_int16((int16_t)data);
            }
            else if(precision%10 == 4){
                int data;
                err = getScalarInteger32(pvApiCtx, piAddressVar, &data);
                *ret = jl_box_int32((int32_t)data);
            }
        }

        if (err) {
            return 0;
        }
    }
    else {
        int ndims;
        int *dims;
        int xdims[2];
        void *data;
        if (isHypermatType(pvApiCtx, piAddressVar)) {

            sciErr = getHypermatOfIntegerPrecision(pvApiCtx, piAddressVar, &precision);
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }

            sciprint("int_sci_to_jl: Hypermat Integer\n");

            if (precision > 10){
                if(precision%10 == 1){
                    unsigned char *temp;
                    sciErr = getHypermatOfUnsignedInteger8(pvApiCtx, piAddressVar, &dims, &ndims, &temp);
                    data = temp;
                }
                else if(precision%10 == 2){
                    short unsigned int *temp;
                    sciErr = getHypermatOfUnsignedInteger16(pvApiCtx, piAddressVar, &dims, &ndims, &temp);
                    data = temp;
                }
                else if(precision%10 == 4){
                    unsigned int *temp;
                    sciErr = getHypermatOfUnsignedInteger32(pvApiCtx, piAddressVar, &dims, &ndims, &temp);
                    data = temp;
                }
            }
            else {
                if(precision%10 == 1){
                    char *temp;
                    sciErr = getHypermatOfInteger8(pvApiCtx, piAddressVar, &dims, &ndims, &temp);
                    data = temp;
                }
                else if(precision%10 == 2){
                    short int *temp;
                    sciErr = getHypermatOfInteger16(pvApiCtx, piAddressVar, &dims, &ndims, &temp);
                    data = temp;
                }
                else if(precision%10 == 4){
                    int *temp;
                    sciErr = getHypermatOfInteger32(pvApiCtx, piAddressVar, &dims, &ndims, &temp);
                    data = temp;
                }
            }
            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }

            sciprint("int_sci_to_jl: size: (");
            for(int i = 0; i != ndims; i++) {
                sciprint("%d", dims[i]);
                if (i != ndims - 1) 
                    sciprint(", ");
            }
            sciprint(")\n");

            

        }
        else {
            // getting data from Scilab
            int m, n;
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
            ndims = 2;
            dims = xdims;

            dims[0] = m;
            dims[1] = n;
        }

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

        jl_value_t *array_type;
        
        if (precision > 10){
            if(precision%10 == 1){
                array_type = jl_apply_array_type((jl_value_t*)jl_uint8_type, ndims);
            }
            else if(precision%10 == 2){
                array_type = jl_apply_array_type((jl_value_t*)jl_uint16_type, ndims);
            }
            else if(precision%10 == 4){
                array_type = jl_apply_array_type((jl_value_t*)jl_uint32_type, ndims);
            }
        }
        else {
            if(precision%10 == 1){
                array_type = jl_apply_array_type((jl_value_t*)jl_int8_type, ndims);
            }
            else if(precision%10 == 2){
                array_type = jl_apply_array_type((jl_value_t*)jl_int16_type, ndims);
            }
            else if(precision%10 == 4){
                array_type = jl_apply_array_type((jl_value_t*)jl_int32_type, ndims);
            }
        }


        *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
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
    int err = 0;

    if(jl_typeis(input, jl_int8_type)) {
        char data = (char) jl_unbox_int8(input);
        sciprint("integer_conv: output argument #%d: jl_int8_type found\n", position);
        err = createScalarInteger8(pvApiCtx, position, data);
    }
    else if(jl_typeis(input, jl_int16_type)) {
        short int data = (short int) jl_unbox_int16(input);
        sciprint("integer_conv: output argument #%d: jl_int16_type found\n", position);
        err = createScalarInteger16(pvApiCtx, position, data);
    }
    else if(jl_typeis(input, jl_int32_type)) {
        int data = (int) jl_unbox_int32(input);
        sciprint("integer_conv: output argument #%d: jl_int32_type found\n", position);
        err = createScalarInteger32(pvApiCtx, position, data);
    }
    else if(jl_typeis(input, jl_int64_type)) {
        int64_t data = jl_unbox_int64(input);
        if (data > INT_MAX || data < INT_MIN) {
            sciprint("integer_conv: there is an overflow in integer conversion\n");
        }
        sciprint("integer_conv: output argument #%d: jl_int32_type found\n", position);
        err = createScalarInteger32(pvApiCtx, position, (int) data);
    }

    else if(jl_typeis(input, jl_uint8_type)) {
        unsigned char data = (unsigned char) jl_unbox_uint8(input);
        sciprint("integer_conv: output argument #%d: jl_uint8_type found\n", position);
        err = createScalarUnsignedInteger8(pvApiCtx, position, data);
    }
    else if(jl_typeis(input, jl_uint16_type)) {
        short unsigned int data = (short unsigned int) jl_unbox_uint16(input);
        sciprint("integer_conv: output argument #%d: jl_uint16_type found\n", position);
        err = createScalarUnsignedInteger16(pvApiCtx, position, data);
    }
    else if(jl_typeis(input, jl_uint32_type)) {
        unsigned int data = (unsigned int) jl_unbox_uint32(input);
        sciprint("integer_conv: output argument #%d: jl_uint32_type found\n", position);
        err = createScalarUnsignedInteger32(pvApiCtx, position, data);
    }
    else if(jl_typeis(input, jl_uint64_type)) {
        uint64_t data = jl_unbox_uint64(input);
        if (data / 2 > INT_MAX) {
            sciprint("integer_conv: there is an overflow in integer conversion\n");
        }
        sciprint("integer_conv: output argument #%d: jl_int32_type found\n", position);
        err = createScalarUnsignedInteger32(pvApiCtx, position, (unsigned int) data);
    }
    else {
        if (jl_is_array(input)) {
            jl_array_t *matrix = (jl_array_t *) input;
            int ndims = jl_array_ndims(matrix);
            int len = jl_array_len(matrix);
            int dims[ndims];
            
            sciprint("size: (");
            for (int i = 0; i != ndims; i++){
                dims[i] = jl_array_dim(matrix, i);
                sciprint("%d, ", dims[i]);
            }
            sciprint(")\n");

            if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_int8_type, ndims))) {
                int8_t *data = (int8_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

                if(ndims == 2)
                    sciErr = createMatrixOfInteger8(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfInteger8(pvApiCtx, position, dims, ndims, data);
            }
            else if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_int16_type, ndims))) {
                int16_t *data = (int16_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

                if(ndims == 2)
                    sciErr = createMatrixOfInteger16(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfInteger16(pvApiCtx, position, dims, ndims, data);
            }
            else if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_int32_type, ndims))) {
                int32_t *data = (int32_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

                if(ndims == 2)
                    sciErr = createMatrixOfInteger32(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfInteger32(pvApiCtx, position, dims, ndims, data);
            }
            else if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_int64_type, ndims))) {
                int32_t *data = (int32_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

                sciprint("int_jl_to_sci: int64 overflow expected\n");
                if(ndims == 2)
                    sciErr = createMatrixOfInteger32(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfInteger32(pvApiCtx, position, dims, ndims, data);
            }
            else if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_uint8_type, ndims))) {
                uint8_t *data = (uint8_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
                
                if(ndims == 2)
                    sciErr = createMatrixOfUnsignedInteger8(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfUnsignedInteger8(pvApiCtx, position, dims, ndims, data);
            }
            else if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_uint16_type, ndims))) {
                uint16_t *data = (uint16_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
                
                if(ndims == 2)
                    sciErr = createMatrixOfUnsignedInteger16(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfUnsignedInteger16(pvApiCtx, position, dims, ndims, data);
            }
            else if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_uint32_type, ndims))) {
                uint32_t *data = (uint32_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
                
                if(ndims == 2)
                    sciErr = createMatrixOfUnsignedInteger32(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfUnsignedInteger32(pvApiCtx, position, dims, ndims, data);
            }
            else if(jl_typeis(input, jl_apply_array_type((jl_value_t*)jl_uint64_type, ndims))) {
                uint32_t *data = (uint32_t*) jl_array_data(matrix);
                if (jl_exception_occurred())
                    sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));
                
                sciprint("int_jl_to_sci: uint64 overflow expected\n");
                if(ndims == 2)
                    sciErr = createMatrixOfUnsignedInteger32(pvApiCtx, position, dims[0], dims[1], data);
                else 
                    sciErr = createHypermatOfUnsignedInteger32(pvApiCtx, position, dims, ndims, data);
            }
            else {
                Scierror(999, "integer_conv: not a correct integer variable\n");
                return 0;
            }

            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
        }
    }

    if (err) {
        Scierror(999, "interger_conv: error in conversion\n");
        return 0;
    }

    return 1;
}
