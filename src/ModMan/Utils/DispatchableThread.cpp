#include "DispatchableThread.h"

DispatchableThread::DispatchableThread()
    : m_terminate(false)
{
}

DispatchableThread::~DispatchableThread()
{
    Terminate();
}

void DispatchableThread::Start()
{
    m_terminate = false;
    m_thread = std::thread(
        [&]
        {
            ThreadLoop();
        });
}

void DispatchableThread::Terminate()
{
    std::unique_lock lock(m_cb_mutex);

    if (!m_terminate)
    {
        m_terminate = true;
        m_cv.notify_all();
        lock.unlock();
        m_thread.join();
    }
    else
    {
        lock.unlock();
    }
}

void DispatchableThread::Dispatch(cb_t cb)
{
    std::lock_guard lock(m_cb_mutex);

    m_cb_list.emplace_back(std::move(cb));
    m_cv.notify_one();
}

std::optional<DispatchableThread::cb_t> DispatchableThread::NextCallback()
{
    if (m_terminate || m_cb_list.empty())
        return std::nullopt;

    auto cb = std::move(m_cb_list.front());
    m_cb_list.pop_front();

    return cb;
}

void DispatchableThread::ThreadLoop()
{
    while (!m_terminate)
    {
        std::unique_lock lock(m_cb_mutex);
        m_cv.wait(lock,
                  [&]
                  {
                      return !m_cb_list.empty() || m_terminate;
                  });

        auto maybeCb = NextCallback();

        lock.unlock();

        if (maybeCb)
            (*maybeCb)();
    }
}
