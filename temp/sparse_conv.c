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

    jl_value_t *array_type = jl_apply_array_type((jl_value_t*)jl_int64_type, 1);
    jl_value_t *float_array_type = jl_apply_array_type((jl_value_t*)jl_float64_type, 1);


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

    mat->colptr = (jl_array_t*)colptr;
    mat->rowval = (jl_array_t*)rowval;
    mat->nzval = (jl_array_t*)nzval;
    return 1;
}

int sparse_jl_to_sci(jl_value_t *input, int position) {
    // Error management variable
    SciErr sciErr;
    int err;

    // get function using the function name provided
    jl_function_t *func = jl_get_function(jl_base_module, "transpose");
    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }

    jl_value_t *ret = jl_call1(func, input);

    typedef struct {
        int64_t m;
        int64_t n;
        jl_array_t *colptr;
        jl_array_t *rowval;
        jl_array_t *nzval;
    } jl_sparse_matrix_csc_t;

    jl_sparse_matrix_csc_t *mat = (jl_sparse_matrix_csc_t *) ret;

    int m = mat->n;
    int n = mat->m;
    int num = jl_array_len(mat->nzval);
    double *data = (double*)jl_array_data(mat->nzval);

    int *numRow;
    int *colPos;

    sciprint("%d x %d \n", m, n);

    numRow = (int*) malloc(sizeof(int) * m);
    int64_t *colptr = (int64_t*) jl_array_data(mat->colptr);
    for(int i = 0; i != m; i++) {
        numRow[i] = colptr[i + 1] - colptr[i];
    }


    colPos = (int*) malloc(sizeof(int) * num);
    int64_t *rowval = (int64_t*) jl_array_data(mat->rowval);
    for (int i = 0; i != num; i++) {
        colPos[i] = rowval[i];
    }

    for (int i = 0; i != jl_array_len(mat->colptr); i++) 
        sciprint("%d, ", colptr[i]);
    sciprint("\n");

    for (int i = 0; i != num; i++) 
        sciprint("%d, ", rowval[i]);
    sciprint("\n");

    sciErr = createSparseMatrix(pvApiCtx, position, m, n, num, numRow, colPos, data);
    free(numRow);
    free(colPos);

    if (sciErr.iErr)
    {
        JL_GC_POP();
        printError(&sciErr, 0);
        return 0;
    }

    return 1;
}

