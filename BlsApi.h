#pragma once
#ifndef BLS_API

#define BLS_API
#include "napi.h"
#include "ExecPool.h"

class BlsApi {
 public:
  static void Init(napi_env env, napi_value exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

 private:
  explicit BlsApi(double value_ = 0);
  ~BlsApi();

  static napi_value New(napi_env env, napi_callback_info info);
  static napi_value GetValue(napi_env env, napi_callback_info info);
  static napi_value SetValue(napi_env env, napi_callback_info info);
  static napi_value PlusOne(napi_env env, napi_callback_info info);
  static napi_value Multiply(napi_env env, napi_callback_info info);
  static napi_value Delay(napi_env env, napi_callback_info info);
  static napi_ref constructor;
  double value_;
  napi_env env_;
  napi_ref wrapper_;
 // exec::ExecPool m_Executor;
};

#endif  //BLS