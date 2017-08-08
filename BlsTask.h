#pragma once

#include <utility>
#include <vector>
#include "stopwatch.h"
#include <sstream>
#include "napi-thread-safe-callback.hpp"
using namespace Napi;
  struct Task
    {
        Task(std::function<void(int)>  fun,int i,int v, std::shared_ptr<ThreadSafeCallback> p)
                :m_f{std::move(fun)}
            {
                id=i;
                value=v;
                callback=p;
            }
        int id=0;
        int value=0;
       std::shared_ptr<ThreadSafeCallback> callback;
        std::function<void(int)> m_f;
        void execute()
        {
            Stopwatch<> sw;
            sw.start();
            m_f(value);
            sw.stop();
       
            std::ostringstream ss;
           ss << "task id "<<id<<" thread =  "<<std::this_thread::get_id()<<"  "<<sw.elapsed()<<std::endl;
          // std::cout<<ss.str();

           std::string result = ss.str();
           // Call back with result
            callback->call([result](Env env, std::vector<napi_value>& args)
            {
                // This will run in main thread and needs to construct the
                // arguments for the call
                args = { env.Undefined(), String::New(env, result) };
            });
           
        }
    };