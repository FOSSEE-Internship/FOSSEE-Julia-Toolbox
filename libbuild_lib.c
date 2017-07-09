#ifdef __cplusplus
extern "C" {
#endif
#include <mex.h> 
#include <sci_gateway.h>
#include <api_scilab.h>
#include <MALLOC.h>
static int direct_gateway(char *fname,void F(void)) { F();return 0;};
extern Gatefunc sci_call_julia;
extern Gatefunc sci_init_julia;
extern Gatefunc sci_eval_julia;
extern Gatefunc sci_get_global_julia;
extern Gatefunc sci_exit_julia;
extern Gatefunc sci_import_package;
static GenericTable Tab[]={
  {(Myinterfun)sci_gateway,sci_call_julia,"callJulia"},
  {(Myinterfun)sci_gateway,sci_init_julia,"initJulia"},
  {(Myinterfun)sci_gateway,sci_eval_julia,"evalJulia"},
  {(Myinterfun)sci_gateway,sci_get_global_julia,"getGlobalJulia"},
  {(Myinterfun)sci_gateway,sci_exit_julia,"exitJulia"},
  {(Myinterfun)sci_gateway,sci_import_package,"importJuliaPackage"},
};
 
int C2F(libbuild_lib)()
{
  Rhs = Max(0, Rhs);
  if (*(Tab[Fin-1].f) != NULL) 
  {
     if(pvApiCtx == NULL)
     {
       pvApiCtx = (StrCtx*)MALLOC(sizeof(StrCtx));
     }
     pvApiCtx->pstName = (char*)Tab[Fin-1].name;
    (*(Tab[Fin-1].f))(Tab[Fin-1].name,Tab[Fin-1].F);
  }
  return 0;
}
#ifdef __cplusplus
}
#endif
