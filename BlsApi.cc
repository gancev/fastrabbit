#include "BlsApi.h"
#include <assert.h>
#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "savequeue.h"
#include "threadpool.h"

napi_ref BlsApi::constructor;
using namespace execlayer;

using namespace exec;
BlsApi::BlsApi(double value)
    : value_(value), env_(nullptr), wrapper_(nullptr) {
      ExecPool::getThreadPool();
    }

BlsApi::~BlsApi() { napi_delete_reference(env_, wrapper_); }

void BlsApi::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<BlsApi*>(nativeObject)->~BlsApi();
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

void BlsApi::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
      { "value", 0, 0, GetValue, SetValue, 0, napi_default, 0 },
      DECLARE_NAPI_METHOD("plusOne", PlusOne),
      DECLARE_NAPI_METHOD("multiply", Multiply),
      DECLARE_NAPI_METHOD("delay", Delay),
  };

  napi_value cons;
  status =
      napi_define_class(env, "BlsApi", New, nullptr, 4, properties, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "BlsApi", cons);
  assert(status == napi_ok);
}

napi_value BlsApi::New(napi_env env, napi_callback_info info) {
  napi_status status;

  bool is_constructor;
  status = napi_is_construct_call(env, info, &is_constructor);
  assert(status == napi_ok);

  if (is_constructor) {
    // Invoked as constructor: `new BlsApi(...)`
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    double value = 0;

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    assert(status == napi_ok);

    if (valuetype != napi_undefined) {
      status = napi_get_value_double(env, args[0], &value);
      assert(status == napi_ok);
    }

    BlsApi* obj = new BlsApi(value);

    obj->env_ = env;
    status = napi_wrap(env,
                       jsthis,
                       reinterpret_cast<void*>(obj),
                       BlsApi::Destructor,
                       nullptr,  // finalize_hint
                       &obj->wrapper_);
    assert(status == napi_ok);

    return jsthis;
  } else {
    // Invoked as plain function `BlsApi(...)`, turn into construct call.
    size_t argc_ = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
    assert(status == napi_ok);

    const size_t argc = 1;
    napi_value argv[argc] = {args[0]};

    napi_value cons;
    status = napi_get_reference_value(env, constructor, &cons);
    assert(status == napi_ok);

    napi_value instance;
    status = napi_new_instance(env, cons, argc, argv, &instance);
    assert(status == napi_ok);

    return instance;
  }
}

napi_value BlsApi::GetValue(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value jsthis;
  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  BlsApi* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  napi_value num;
  status = napi_create_number(env, obj->value_, &num);
  assert(status == napi_ok);

  return num;
}

napi_value BlsApi::SetValue(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value value;
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, &value, &jsthis, nullptr);
  assert(status == napi_ok);


  BlsApi* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  status = napi_get_value_double(env, value, &obj->value_);
  assert(status == napi_ok);

  return nullptr;
}

napi_value BlsApi::PlusOne(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value jsthis;
  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  BlsApi* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  obj->value_ += 1;

  napi_value num;
  status = napi_create_number(env, obj->value_, &num);
  assert(status == napi_ok);

  return num;
}

napi_value BlsApi::Multiply(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value args[1];
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
  assert(status == napi_ok);

  napi_valuetype valuetype;
  status = napi_typeof(env, args[0], &valuetype);
  assert(status == napi_ok);

  double multiple = 1;
  if (valuetype != napi_undefined) {
    status = napi_get_value_double(env, args[0], &multiple);
    assert(status == napi_ok);
  }

  BlsApi* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  napi_value cons;
  status = napi_get_reference_value(env, constructor, &cons);
  assert(status == napi_ok);

  const int kArgCount = 1;
  napi_value argv[kArgCount];
  status = napi_create_number(env, obj->value_ * multiple, argv);
  assert(status == napi_ok);

  napi_value instance;
  status = napi_new_instance(env, cons, kArgCount, argv, &instance);
  assert(status == napi_ok);

  return instance;
}
void fibonacci(int n)
{
   int first = 0, second = 1, next, c; 
 
   for ( c = 0 ; c < n ; c++ )
   {
      if ( c <= 1 )
         next = c;
      else
      {
         next = first + second;
         first = second;
         second = next;
      }
   }
  
   std::this_thread::sleep_for (std::chrono::milliseconds(100));

 
} 
napi_value BlsApi::Delay(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value args[1];
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
  assert(status == napi_ok);

  napi_valuetype valuetype;
  status = napi_typeof(env, args[0], &valuetype);
  assert(status == napi_ok);

  double sleepFor = 1;
  if (valuetype != napi_undefined) {
    status = napi_get_value_double(env, args[0], &sleepFor);
    assert(status == napi_ok);
  }

  BlsApi* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  napi_value cons;
  status = napi_get_reference_value(env, constructor, &cons);
  assert(status == napi_ok);

  const int kArgCount = 1;
  napi_value argv[kArgCount];
  int timeout = obj->value_ * sleepFor;
  //std::cout<<"recieved "<<timeout<<std::endl;
  // std::thread tr( [timeout]()
  //   {
    ExecPool::getThreadPool().submitJob(fibonacci,(int)sleepFor,(int)sleepFor);
    //  std::this_thread::sleep_for (std::chrono::seconds(1));
     // std::cout<<"sleeped"<<timeout<<std::endl;
    // } );
    // tr.detach();
  //  DefaultThreadPool::submitJob([timeout]()
  //  {
  //     std::this_thread::sleep_for (std::chrono::seconds(1));
  //     std::cout<<"sleeped"<<timeout<<std::endl;
  //    });

  status = napi_create_number(env, timeout, argv);
  assert(status == napi_ok);

  napi_value instance;
  status = napi_new_instance(env, cons, kArgCount, argv, &instance);
  assert(status == napi_ok);

  return instance;
}