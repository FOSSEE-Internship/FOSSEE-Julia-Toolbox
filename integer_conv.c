#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"
#include <limits.h>


int int_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;
    int err;

    int precision = 0;
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
    else if (isHypermatType(pvApiCtx, piAddressVar)) {
        int *dims;
        int ndims;
        void *data;

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

        jl_value_t *types[] = {(jl_value_t*)jl_long_type, (jl_value_t*)jl_long_type};
        jl_tupletype_t *tt = jl_apply_tuple_type_v(types, ndims);
        typedef struct {
            ssize_t a[ndims];
        } ntupleint;
        
        ntupleint *tuple = (ntupleint*)jl_new_struct_uninit(tt);
        JL_GC_PUSH1(&tuple);

        jl_value_t *array_type;
        
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

        for (int i = 0; i != ndims; i++)
            (tuple->a)[i] = dims[i];

        *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
        JL_GC_POP();

    }
    else {
        // getting data from Scilab
        int m, n;
        void *data;

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
            return 0;
        }

        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
    }

    if (err) {
        Scierror(999, "interger_conv: error in conversion");
        return 0;
    }

    return 1;
}
