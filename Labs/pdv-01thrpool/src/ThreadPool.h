#pragma once

#include <cstddef>
#include <list>
#include <vector>
#include <thread>
#include <condition_variable>

template<typename JobT, typename WorkerT>
class ThreadPool {
private:
    /** Fronta uloh. */
    std::list<JobT> job_queue{};
    /** Vlakna konzumentu zpracovavajicich ulohy. */
    std::vector<std::thread> worker_threads{};
    /** Funkce, kterou maji konzumenti vykonavat. */
    WorkerT worker_fn;

public:
    ThreadPool(size_t thread_count, WorkerT worker) : worker_fn(worker) {
        // Zde vytvorte "thread_count" vlaken konzumentu:
        for (size_t i = 0; i < thread_count; i++)
        {
            worker_threads.push_back(std::thread([this]() {
                worker_loop();
            }));
        }
    }

    /** Adds a new job to the queue.*/
    void process(const JobT job) {
        std::unique_lock<std::mutex> queueLock(queueMut);
        job_queue.push_back(job);
        cv.notify_all();
        queueLock.unlock();
    }

    /** Tato metoda nam umozni volajici funkci v main.cpp pockat na vsechna spustena vlakna konzumentu. */
    void join() {
        for (auto& worker_thread : worker_threads) {
            worker_thread.join();
        }
    }

private:
    std::condition_variable cv;
    std::mutex queueMut;

    void worker_loop() {
        while (true) {
//            Conditional Lock
            std::unique_lock<std::mutex> queueLock(queueMut);
            cv.wait(queueLock, [this] { return !job_queue.empty(); });

            queueLock.lock();
            auto job = job_queue.front();
            job_queue.pop_front();
            queueLock.unlock();

            if (!job)
                break;

            worker_fn(job);
        }
    }
};
