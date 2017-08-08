#include "napi.h"
#include "napi-thread-safe-callback.hpp"
#include "BlsReactive.h"
using namespace Napi;

 


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
void async_bls(const CallbackInfo& info)
{
        // Capture callback in main thread
    auto callback = std::make_shared<ThreadSafeCallback>(info[0].As<Function>());
    bool fail = info.Length() > 1;
    ACPool::getThreadPool().submitJob(fibonacci,200,300,callback);
     // Pass callback to other thread
    // std::thread([callback, fail]
    // {
    //     try
    //     {
    //         // Do some work to get a result
    //         if (fail)
    //             throw std::runtime_error("Failure during async work");
    //         std::string result = "foo dwqewqewqeqw ewqeqwqw";

    //         // Call back with result
    //         callback->call([result](Env env, std::vector<napi_value>& args)
    //         {
    //             // This will run in main thread and needs to construct the
    //             // arguments for the call
    //             args = { env.Undefined(), String::New(env, result) };
    //         });
    //     }
    //     catch (std::exception& e)
    //     {
    //         // Call back with error
    //        // callback->callError(e.what());
    //     }
    // }).detach();
}
#define ADD_CALLBACK(name) \
    exports[#name] = Function::New(env, name, #name);


void init(Env env, Object exports, Object module)
{
   
   ADD_CALLBACK(async_bls);

}

NODE_API_MODULE(callexec, init)