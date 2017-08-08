#pragma once
#include "savequeue.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <sstream>
#include "napi.h"
#include "napi-thread-safe-callback.hpp"
#include "task.h"
#include "savequeue.h"
#include "stopwatch.h"

using namespace Napi;

class WorkerPool
{
  public:
    inline static WorkerPool &getThePool(void)
    {
        static WorkerPool defaultPool(1000);
        return defaultPool;
    }
    /**
         * Constructor.
         */
    explicit WorkerPool(const std::uint32_t numThreads)
        : m_done{false},
          m_workQueue{},
          m_threads{}
    {
        try
        {
            for (std::uint32_t i = 0u; i < numThreads; ++i)
            {
                m_threads.emplace_back(&WorkerPool::worker, this);
            }
            for (std::uint32_t i = 0u; i < numThreads; ++i)
            {
                //make the threads independent
                m_threads[i].detach();
            }
            std::cout << "Pool Initialized with " << numThreads << " threads" << std::endl;
        }
        catch (...)
        {
            destroy();
        }
    }
    /**
         * Non-copyable.
         */
    WorkerPool(const WorkerPool &rhs) = delete;

    /**
         * Non-assignable.
         */
    WorkerPool &operator=(const WorkerPool &rhs) = delete;

    /**
         * Destructor.
         */
    ~WorkerPool(void)
    {
        destroy();
    }
    /**
         * Submit a job to the default thread pool.
         */

    // void submitJob(std::function<void(int)> fun, int id, int value, std::shared_ptr<ThreadSafeCallback> pCall)
    // {
    //     std::unique_ptr<Task> p = std::make_unique<Task>(fun, id, value, pCall);
    //     m_workQueue.push(std::move(p));
    // }
    /**
         * Submit a job to the default thread pool.
         */
    template <typename Func, typename... Args>
    inline auto submitJob(Func &&func, Args &&... args)
    {
        return getThePool().submit(std::forward<Func>(func), std::forward<Args>(args)...);
    }

    /**
         * Submit a job to be run by the thread pool.
         */
    template <typename Func, typename... Args>
    void submit(Func &&func, Args &&... args)
    {
        auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
        using ResultType = std::result_of_t<decltype(boundTask)()>;
        using PackagedTask = std::packaged_task<ResultType()>;
        using TaskType = Task<PackagedTask>;
        PackagedTask task{std::move(boundTask)};
        m_workQueue.push(std::make_unique<TaskType>(std::move(task)));
    }

  private:
    /**
         * Constantly running function each thread uses to acquire work items from the queue.
         */
    void worker(void)
    {
        int jobs = 0;
        while (!m_done)
        {
            std::unique_ptr<ITask> pTask{nullptr};
            if (m_workQueue.waitPop(pTask))
            {
                jobs++;
                std::ostringstream ss;
                ss << jobs << " jobs thread =  " << std::this_thread::get_id() << std::endl;
                std::cout << ss.str();
                pTask->execute();
            }
            else
            {

                std::this_thread::yield();
            }
        }
    }
    /**
         * Invalidates the queue and joins all running threads.
         * kills the poll gently 
         */
    void destroy(void)
    {
        m_done = true;
        m_workQueue.invalidate();
        for (auto &thread : m_threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

  private:
    std::atomic_bool m_done;
    execlayer::ThreadSafeQueue<std::unique_ptr<ITask>> m_workQueue;
    std::vector<std::thread> m_threads;
};
