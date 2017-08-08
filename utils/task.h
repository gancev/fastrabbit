#pragma once

#include <utility>
#include <vector>
#include "stopwatch.h"
#include <sstream>
#include "napi-thread-safe-callback.hpp"
using namespace Napi;

class ITask
{
  public:
    ITask(void) = default;
    virtual ~ITask(void) = default;
    ITask(const ITask &rhs) = delete;
    ITask &operator=(const ITask &rhs) = delete;
    ITask(ITask &&other) = default;
    ITask &operator=(ITask &&other) = default;

    /**
             * Run the task.
             */
    virtual void execute() = 0;
};

/**
    * Run the task.
    */
template <typename Func>
class Task : public ITask
{
  public:
    Task(Func &&func)
        : m_func{std::move(func)}
    {
    }

    ~Task(void) override = default;
    Task(const Task &rhs) = delete;
    Task &operator=(const Task &rhs) = delete;
    Task(Task &&other) = default;
    Task &operator=(Task &&other) = default;

    /**
             * Run the task.
             */
    void execute() override
    {
        m_func();
    }

  private:
    Func m_func;
};