/*******************************************************************************
 * src/thread_pool.hpp
 *
 * A ThreadPool of std::threads to work on jobs
 *
 * Copyright (C) 2015-2016 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef BISPANNING_THREAD_POOL_HEADER
#define BISPANNING_THREAD_POOL_HEADER

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

/*!
 * ThreadPool starts a fixed number p of std::threads which process Jobs that
 * are \ref Enqueue "enqueued" into a concurrent job queue. The jobs
 * themselves can enqueue more jobs that will be processed when a thread is
 * ready.
 *
 * The ThreadPool can either run until
 *
 * 1. all jobs are done AND all threads are idle, when called with
 * LoopUntilEmpty(), or
 *
 * 2. until Terminate() is called when run with LoopUntilTerminate().
 *
 * Jobs are plain std::function<void()> objects (actually: common::delegate),
 * hence the pool user must pass in ALL CONTEXT himself. The best method to pass
 * parameters to Jobs is to use lambda captures. Alternatively, old-school
 * objects implementing operator(), or std::binds can be used.
 *
 * The ThreadPool uses a condition variable to wait for new jobs and does not
 * remain busy waiting.
 *
 * Note that the threads in the pool start **before** the two loop functions are
 * called. In case of LoopUntilEmpty() the threads continue to be idle
 * afterwards, and can be reused, until the ThreadPool is destroyed.

\code
ThreadPool pool(4); // pool with 4 threads

int value = 0;
pool.Enqueue([&value]() {
  // increment value in another thread.
  ++value;
});

pool.LoopUntilEmpty();
\endcode

 * ## Synchronization Primitives
 *
 * Beyond threads from the ThreadPool, the framework contains two fast
 * synchronized queue containers:
 *
 * - \ref ConcurrentQueue
 * - \ref ConcurrentBoundedQueue.
 *
 * If the Intel Thread Building Blocks are available, then these use their
 * lock-free implementations, which are very fast, but do busy-waiting for
 * items. Otherwise, compatible replacements are used.
 *
 * The \ref ConcurrentQueue has no busy-waiting pop(), only a try_pop()
 * method. This should be preferred! The \ref ConcurrentBoundedQueue<T> has a
 * blocking pop(), but it probably does busy-waiting.
 */
class ThreadPool
{
public:
    using Job = std::function<void()>;

private:
    //! Deque of scheduled jobs.
    std::deque<Job> jobs_;

    //! Mutex used to access the queue of scheduled jobs.
    std::mutex mutex_;

    //! threads in pool
    std::vector<std::thread> threads_;

    //! Condition variable used to notify that a new job has been inserted in
    //! the queue.
    std::condition_variable cv_jobs_;
    //! Condition variable to signal when a jobs finishes.
    std::condition_variable cv_finished_;

    //! Counter for number of threads busy.
    std::atomic<size_t> busy_ = { 0 };
    //! Counter for total number of jobs executed
    std::atomic<size_t> done_ = { 0 };

    //! Flag whether to terminate
    std::atomic<bool> terminate_ = { false };

    //! size limit on the job queue
    size_t queue_size_;

public:
    //! Construct running thread pool of num_threads
    explicit ThreadPool(
        size_t num_threads = std::thread::hardware_concurrency(),
        size_t queue_size = 2* std::thread::hardware_concurrency())
        : threads_(num_threads), queue_size_(queue_size)
    {
        // immediately construct worker threads
        for (size_t i = 0; i < num_threads; ++i)
            threads_[i] = std::thread(&ThreadPool::Worker, this);
    }

    //! non-copyable: delete copy-constructor
    ThreadPool(const ThreadPool&) = delete;
    //! non-copyable: delete assignment operator
    ThreadPool& operator = (const ThreadPool&) = delete;

    //! Stop processing jobs, terminate threads.
    ~ThreadPool()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // set stop-condition
        terminate_ = true;
        cv_jobs_.notify_all();
        lock.unlock();

        // all threads terminate, then we're done
        for (size_t i = 0; i < threads_.size(); ++i)
            threads_[i].join();
    }

    //! Enqueue a Job, the caller must pass in all context using captures.
    void Enqueue(Job&& job)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_finished_.wait(
            lock, [this]() { return jobs_.size() < queue_size_; });
        jobs_.emplace_back(std::move(job));
        cv_jobs_.notify_all();
    }

    //! Loop until no more jobs are in the queue AND all threads are idle. When
    //! this occurs, this method exits, however, the threads remain active.
    void LoopUntilEmpty()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_finished_.wait(
            lock, [this]() { return jobs_.empty() && (busy_ == 0); });
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }

    //! Loop until terminate flag was set.
    void LoopUntilTerminate()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_finished_.wait(
            lock, [this]() { return terminate_ && (busy_ == 0); });
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }

    //! Terminate thread pool gracefully, wait until currently running jobs
    //! finish and then exit. This should be called from within one of the
    //! enqueue jobs or from an outside thread.
    void Terminate()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // flag termination
        terminate_ = true;
        // wake up all worker threads and let them terminate.
        cv_jobs_.notify_all();
        // notify LoopUntilTerminate in case all threads are idle.
        cv_finished_.notify_one();
    }

    //! Return number of jobs currently completed.
    size_t done() const
    {
        return done_;
    }

    //! Return number of threads in pool
    size_t size() const
    {
        return threads_.size();
    }

    //! Return thread handle to thread i
    std::thread & thread(size_t i)
    {
        assert(i < threads_.size());
        return threads_[i];
    }

private:
    //! Worker function, one per thread is started.
    void Worker()
    {
        // lock mutex, it is released during condition waits
        std::unique_lock<std::mutex> lock(mutex_);

        while (true) {
            // wait on condition variable until job arrives, frees lock
            cv_jobs_.wait(
                lock, [this]() { return terminate_ || !jobs_.empty(); });

            if (terminate_) break;

            if (!jobs_.empty()) {
                // got work. set busy.
                ++busy_;

                {
                    // pull job.
                    Job job = std::move(jobs_.front());
                    jobs_.pop_front();

                    // release lock.
                    lock.unlock();

                    // execute job.
                    try {
                        job();
                    }
                    catch (std::exception& e) {
                        std::cerr << "EXCEPTION: " << e.what() << std::endl;
                    }
                    // destroy job by closing scope
                }

                // release memory the Job changed
                std::atomic_thread_fence(std::memory_order_seq_cst);

                ++done_;
                --busy_;

                // relock mutex before signaling condition.
                lock.lock();
                cv_finished_.notify_one();
            }
        }
    }
};

#endif // !BISPANNING_THREAD_POOL_HEADER

/******************************************************************************/
