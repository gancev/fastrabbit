
#include "BlsApi.h"
#include "napi.h"

void Init(napi_env env, napi_value exports, napi_value module, void* priv) {
  BlsApi::Init(env, exports);
}

NAPI_MODULE(addon, Init)
