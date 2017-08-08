#include "napi.h"
#include "napi-thread-safe-callback.hpp"
#include "workerpool.h"
//#include "../threadpool.h"
using namespace Napi;

void async_bls(const CallbackInfo &info)
{
    bool fail = false;
    if (!info[0].IsString())
    {
        std::cout << "First argument in call should be string" << std::endl;
    }
    if (!info[1].IsFunction())
    {
        std::cout << "second argument should be function callback" << std::endl;
    }

    auto callback = std::make_shared<ThreadSafeCallback>(info[1].As<Function>());

    std::string inputJSON = info[0].As<String>();

    //without lambda
    // WorkerPool::getThePool().submitJob(dummycomputation, 200, 300, callback);
    WorkerPool::getThePool().submitJob([inputJSON, callback, fail] {
        try
        {
            Stopwatch<> sw;
            sw.start();

            // Do some work to get a result
            if (fail)
                throw std::runtime_error("Failure during async work");
            std::cout << inputJSON;
            std::string result = inputJSON;
            //DO SOME HEAVY PROCESSING HERE
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            // Call back with result

            sw.stop();
            std::cout << sw.elapsed() << std::endl;
            callback->call([result](Env env, std::vector<napi_value> &args) {
                // This will run in main thread and needs to construct the
                // arguments for the call
                args = {env.Undefined(), String::New(env, result)};
            });
        }
        catch (std::exception &e)
        {
            // Call back with error
            std::cout << "ERROR in thread execution" << e.what() << std::endl;
        }
    });
}
#define ADD_CALLBACK(name) \
    exports[#name] = Function::New(env, name, #name);

void init(Env env, Object exports, Object module)
{

    ADD_CALLBACK(async_bls);
}

NODE_API_MODULE(msgprocessor, init)