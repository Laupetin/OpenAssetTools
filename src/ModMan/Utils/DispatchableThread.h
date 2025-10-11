#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

class DispatchableThread
{
public:
    using cb_t = std::function<void()>;

    DispatchableThread();
    ~DispatchableThread();
    DispatchableThread(const DispatchableThread& other) = delete;
    DispatchableThread(DispatchableThread&& other) noexcept = default;
    DispatchableThread& operator=(const DispatchableThread& other) = delete;
    DispatchableThread& operator=(DispatchableThread&& other) noexcept = default;

    void Start();
    void Terminate();

    void Dispatch(cb_t cb);

private:
    std::optional<cb_t> NextCallback();
    void ThreadLoop();

    std::mutex m_cb_mutex;
    std::deque<cb_t> m_cb_list;

    std::condition_variable m_cv;
    std::thread m_thread;
    bool m_terminate;
};
