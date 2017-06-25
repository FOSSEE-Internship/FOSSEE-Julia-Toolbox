#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"

#include <complex.h>


int double_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;
    int err;

    if(isScalar(pvApiCtx, piAddressVar)){
        double data, img_data;
        if (isVarComplex(pvApiCtx, piAddressVar)) { 
            sciprint("double_sci_to_jl: Scalar Complex Double\n");
            err = getScalarComplexDouble(pvApiCtx, piAddressVar, &data, &img_data);
            if (err)
            {
                return 0;
            }

            jl_value_t *scalar_type = jl_apply_type((jl_value_t*)jl_complex_type, jl_svec1(jl_float64_type));
            *ret = (jl_value_t*)jl_new_struct_uninit((jl_datatype_t *)scalar_type);
    
            double complex *temp = (double complex*) *ret;
            *temp = data + (img_data * I);
        }
        else {
            sciprint("double_sci_to_jl: Scalar Double\n");
            err = getScalarDouble(pvApiCtx, piAddressVar, &data);
            if (err)
            {
                return 0;
            }   
            *ret = jl_box_float64(data);
        }

    }
    else { 
        int ndims = 0;
        
        int xdims[2];
        int *dims;

        if (isVarComplex(pvApiCtx, piAddressVar)) {
            double *real, *imag;
            if (isHypermatComplex(pvApiCtx, piAddressVar)) {    
                sciprint("double_sci_to_jl: Hypermat Complex Double\n");

                sciErr = getComplexHypermatOfDouble(pvApiCtx, piAddressVar, &dims, &ndims, &real, &imag);
                if (sciErr.iErr)
                {
                    printError(&sciErr, 0);
                    return 0;
                }
            }
            else {
                sciprint("double_sci_to_jl: Matrix Complex Double\n");
                
                int m, n;
                // getting data from Scilab
                sciErr = getComplexMatrixOfDouble(pvApiCtx, piAddressVar, &m, &n, &real, &imag);
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

            jl_value_t *array_element_type = jl_apply_type((jl_value_t*)jl_complex_type, jl_svec1(jl_float64_type));
            jl_value_t *array_type = jl_apply_array_type((jl_datatype_t *)array_element_type, ndims);
            
            jl_value_t *types[ndims];
            for (int i = 0; i != ndims; i++)
                types[i] = (jl_value_t*)jl_long_type;

            jl_tupletype_t *tt = jl_apply_tuple_type_v(types, ndims);

            ssize_t *tuple = (ssize_t*)jl_new_struct_uninit(tt);
            JL_GC_PUSH1(&tuple);

            
            for (int i = 0; i != ndims; i++)
                (tuple)[i] = dims[i];

            *ret = (jl_value_t*) jl_new_array(array_type, (jl_value_t*)tuple);
            JL_GC_POP();
            
            double complex *data = (double complex*) jl_array_data(*ret);
            for (int i = 0; i != len; i++) {
                sciprint("double_sci_to_jl: %d\n", data);
                // data[i] = (jl_value_t*)jl_new_struct_uninit((jl_datatype_t *)array_element_type);
                data[i] = real[i] + imag[i] * I;
            }

            // *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);

            complex double *xData = (complex double*) jl_array_data(*ret);
            for (int i = 0; i != len; i++)
                sciprint("%f + %fi\n", creal(xData[i]), cimag(xData[i]));
        }
        else {
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
            // typedef struct {
            //     ssize_t a[ndims];
            // } ntupleint;
            
            ssize_t *tuple = (ssize_t*)jl_new_struct_uninit(tt);
            JL_GC_PUSH1(&tuple);

            
            for (int i = 0; i != ndims; i++)
                (tuple)[i] = dims[i];

            

            *ret = (jl_value_t*) jl_ptr_to_array(array_type, data, (jl_value_t*)tuple, 0);
            JL_GC_POP();

            double *xData = (double*) jl_array_data(*ret);

            for (int i = 0; i != len; i++)
                sciprint("%f\n", xData[i]);
        }
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
        jl_array_t *matrix = (jl_array_t *) input;
        
        // Get number of dimensions
        int ndims = jl_array_ndims(matrix);
        // Get the size of the matrix
        int dims[ndims];
        int len = jl_array_len(matrix);

        sciprint("size: (");
        for (int i = 0; i != ndims; i++){
            dims[i] = jl_array_dim(matrix, i);
            sciprint("%d, ", dims[i]);
        }
        sciprint(")\n");

        if(jl_typeis(matrix, jl_apply_array_type(jl_float64_type, ndims))) {
            double *data;

            sciprint("double_jl_to_sci: argument #%d: Matrix Double\n", position);

            data = (double*) jl_array_data(matrix);
            
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
        else if (jl_typeis(matrix, jl_apply_array_type((jl_datatype_t*)jl_apply_type((jl_value_t*)jl_complex_type, jl_svec1(jl_float64_type)), ndims))) {
            sciprint("double_jl_to_sci: argument #%d: Matrix Complex Double\n", position);
            double complex *jl_data = (double complex *) jl_array_data(matrix);
            // double complex ** data = (double complex **) jl_array_data(matrix);

            // sciprint("double_jl_to_sci: %d: %s\n", position, jl_typeof_str());

            double *real, *imag;
            real = (double*) malloc(len * sizeof(double));
            imag = (double*) malloc(len * sizeof(double));

            // sciprint("%f\n", creal(data[0]));

            for (int i = 0; i != len; i++) {
                double complex temp = jl_data[i];
                // sciprint("double_jl_to_sci: %d: %s\n", i, jl_typeof_str(jl_data[i]));
                real[i] = creal(temp);
                imag[i] = cimag(temp);
                // sciprint("%f ", creal(*data[i]));
                // sciprint("%f\n", cimag(*data[i]));
                // real[i] = creal(*data[i]);
                // imag[i] = cimag(*data[i]);
            }

            if (ndims == 2){
                sciErr = createComplexMatrixOfDouble(pvApiCtx, position, dims[0], dims[1], real, imag);
            }
            else {
                sciErr = createComplexHypermatOfDouble(pvApiCtx, position, dims, ndims, real, imag);
            }
            
            free(real);
            free(imag);

            if (sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }
        }

        if (jl_exception_occurred())
            sciprint("%s \n", jl_typeof_str(jl_exception_occurred()));

    }
    else if(jl_typeis(input, jl_float64_type)){
        sciprint("double_jl_to_sci: argument #%d: Scalar Double\n", position);
        double data = jl_unbox_float64(input);
        sciprint("data: %f\n", data);
        err = createScalarDouble(pvApiCtx, position, data);
        if (err) {
            return 0;
        }
    }
    else if (jl_typeis(input, jl_apply_type((jl_value_t*)jl_complex_type, jl_svec1(jl_float64_type)))) {
        complex double *data = (complex double*) input;
        double real = creal(*data);
        double imag = cimag(*data);
        err = createScalarComplexDouble(pvApiCtx, position, real, imag);
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
