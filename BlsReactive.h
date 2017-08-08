#pragma once

//
#include "napi.h"
#include "napi-thread-safe-callback.hpp"
#include "BlsTask.h"
#include "savequeue.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>

#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include "stopwatch.h"
#include <sstream>
#include "BlsTask.h"
using namespace Napi;


//void async_bls(const CallbackInfo& info);

 
//namespace asyncpool
//{
    
    class ACPool
    {
        public:
        inline static ACPool& getThreadPool(void)
        {
            static ACPool defaultPool(1500);
            return defaultPool;
        }
        /**
         * Constructor.
         */
        explicit ACPool(const std::uint32_t numThreads)
            :m_done{false},
            m_workQueue{},
            m_threads{}
        {
            try
            {
                for(std::uint32_t i = 0u; i < numThreads; ++i)
                {
                    m_threads.emplace_back(&ACPool::worker, this);
                }
                for(std::uint32_t i = 0u; i < numThreads; ++i)
                {
                    //make the threads independent
                    m_threads[i].detach();
                }
                std::cout<<numThreads <<" threads created"<<std::endl;
            }
            catch(...)
            {
                destroy();
            }
        }

        /**
         * Destructor.
         */
        ~ACPool(void)
        {
            destroy();
        }
        /**
         * Submit a job to the default thread pool.
         */
        
        void submitJob(std::function<void(int)>  fun,int id,int value,std::shared_ptr<ThreadSafeCallback> pCall)
        {
            std::unique_ptr<Task> p= std::make_unique<Task>(fun,id,value,pCall);
            m_workQueue.push(std::move(p));
        }
        private:
        /**
         * Constantly running function each thread uses to acquire work items from the queue.
         */
        void worker(void)
        {
            int jobs=0;
            while(!m_done)
            {
                std::unique_ptr<Task> pTask{nullptr};
                if(m_workQueue.waitPop(pTask))
                {
                    jobs++;
                    std::ostringstream ss;
                    ss << jobs<<" jobs thread =  "<<std::this_thread::get_id()<<std::endl;
                    
                    std::cout<<ss.str();
                    pTask->execute();
                }else
                {
                   
                    std::this_thread::yield();
                }
            }
        }
        /**
         * Invalidates the queue and joins all running threads.
         */
        void destroy(void)
        {
            m_done = true;
            m_workQueue.invalidate();
            for(auto& thread : m_threads)
            {
                if(thread.joinable())
                {
                    thread.join();
                }
            }
        }
        private:
            std::atomic_bool m_done;
            execlayer::ThreadSafeQueue<std::unique_ptr<Task>> m_workQueue;
            std::vector<std::thread> m_threads;
        };
//}