#include <stdio.h>
#include <julia.h>

int main() {

    ////////// Julia Init Code //////////
    /* required: setup the Julia context */
    jl_init();

    char sparse_construct[100];
    sprintf(sparse_construct, "sparse([0 4325; 23 0; 0 0; 245 2352])");
    jl_value_t *empty = jl_eval_string(sparse_construct);
    printf("%s variable returned\n", jl_typeof_str(empty));
    printf("%s \ncolptr,  rowval,  nzval \n",sparse_construct);
    typedef struct {
        int64_t m;
        int64_t n;
        jl_array_t *colptr;
        jl_array_t *rowval;
        jl_array_t *nzval;
    } jl_sparse_matrix_csc_t;

    jl_sparse_matrix_csc_t *mat = (jl_sparse_matrix_csc_t*) empty;

    jl_value_t **inpArgs;
    JL_GC_PUSHARGS(inpArgs, 3); 

    // get function using the function name provided
    jl_function_t *func = jl_get_function(jl_base_module, "string");
    if (jl_exception_occurred()) {
        printf("%s \n", jl_typeof_str(jl_exception_occurred()));
        return 0;
    }

    printf("%s: %s %s %s\n", jl_typeof_str(empty), jl_typeof_str(mat->colptr), jl_typeof_str(mat->rowval), jl_typeof_str(mat->nzval));

    // if (jl_typeis(mat->colptr, jl_apply_array_type(jl_int32_type, 1))) {
    //     printf("Yes int32\n");
    // }
    // else if (jl_typeis(mat->colptr, jl_apply_array_type(jl_int64_type, 1))) {
    //     printf("Yes int64\n");
    // }

    inpArgs[0] = mat->colptr;
    inpArgs[1] = mat->rowval;
    inpArgs[2] = mat->nzval;

    jl_value_t *jl_sparse_t = jl_get_global(jl_base_module, jl_symbol("SparseMatrixCSC")); 
    // jl_value_t *sparse_double_type = jl_apply_type(jl_sparse_t, jl_svec2(jl_float64_type, jl_int64_type));
    if (jl_isa(empty, jl_sparse_t)) 
        printf("Yes!\n");
    else 
        printf("No!\n");

    jl_value_t *ret = jl_call(func, inpArgs, 3);
    JL_GC_POP();

    JL_GC_PUSH1(&ret);
    printf("%s\n", jl_string_data(ret));
    JL_GC_POP();

    printf("%s\n", jl_string_data(jl_call1(func, empty)));
    /* strongly recommended: notify Julia that the
         program is about to terminate. this allows
         Julia time to cleanup pending write requests
         and run all finalizers
    */
    jl_atexit_hook(0);
    // *ret = mat->rowval;
    // *ret = mat->nzval;


}