#include "api_scilab.h"
#include <julia.h>
#include "Scierror.h"


int sparse_sci_to_jl(int *piAddressVar, jl_value_t **ret) {
    // Error management variable
    SciErr sciErr;
    int err;

    char *fname = "sparse_sci_to_jl";

    int m, n;
    int num;
    int *numRow;
    int *colPos;
    double *data;
    sciErr = getSparseMatrix(pvApiCtx, piAddressVar, &m, &n, &num, &numRow, &colPos, &data);
    if (sciErr.iErr)
    {
        JL_GC_POP();
        printError(&sciErr, 0);
        return 0;
    }

    sciprint("%s: %dx%d: num: %d\n", fname, m, n, num);
    for (int i = 0; i != num; i++){
        sciprint("%s: data: %f, col: %d\n", fname, data[i], colPos[i]);
    }

    for (int i = 0; i != m; i++){
        sciprint("%s: %d: %d\n", fname, i, numRow[i]);
    }

    char sparse_construct[100];
    // sprintf(sparse_construct, "sparse([0 4325; 23 0; 0 0; 245 2352])");
    sprintf(sparse_construct, "spzeros(%d, %d)", n, m);
    *ret = jl_eval_string(sparse_construct);


    sciprint("%s: %s variable returned\n", fname, jl_typeof_str(*ret));

    
    typedef struct {
        int64_t m;
        int64_t n;
        jl_array_t *colptr;
        jl_array_t *rowval;
        jl_array_t *nzval;
    } jl_sparse_matrix_csc_t;

    jl_sparse_matrix_csc_t *mat = (jl_sparse_matrix_csc_t*) *ret;

    jl_value_t *array_type = jl_apply_array_type(jl_int64_type, 1);
    jl_value_t *float_array_type = jl_apply_array_type(jl_float64_type, 1);


    jl_value_t *colptr = (jl_value_t *)jl_alloc_array_1d(array_type, m + 1);
    jl_value_t *rowval = (jl_value_t *) jl_alloc_array_1d(array_type, num);
    jl_value_t *nzval = (jl_value_t *) jl_ptr_to_array_1d(float_array_type, data, num, 0);

    int64_t *zData = (int64_t*)jl_array_data(colptr);
    int curr = 1;
    for (int i = 0; i != m + 1; i++) {
        zData[i] = curr;
        if (i < m)
            curr += numRow[i];
    }
    zData = (int64_t*)jl_array_data(rowval);
    for (int i = 0; i != num; i++)
        zData[i] = colPos[i];

    mat->colptr = colptr;
    mat->rowval = rowval;
    mat->nzval = nzval;
    // *ret = (jl_value_t*)rowval;
    return 1;
}
