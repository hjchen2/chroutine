/// \file engin.hpp
/// 
/// engin is the manager for all chroutine_thread_t
///
/// \author ingangi
/// \version 0.1.0
/// \date 2018-12-12

#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <thread>
#include "chroutine.hpp"
#include "selectable_obj.hpp"

typedef std::map<std::thread::id, std::shared_ptr<chroutine_thread_t> > thread_pool_t;
typedef std::vector<std::shared_ptr<chroutine_thread_t> > creating_threads_t;

#define ENGIN engine_t::instance()
#define ENGINE_INIT(thrds) {ENGIN.init(thrds);}
#define YIELD() {ENGIN.yield();}
#define WAIT(t) {ENGIN.wait(t);}
#define SLEEP(t) {ENGIN.sleep(t);}

std::time_t get_time_stamp();

class engine_t 
{
    friend class chroutine_thread_t;
public:
    static engine_t& instance();
    ~engine_t();

    // start all thread, will block your thread until they are ready !!!
    void init(size_t init_pool_size);
    
    // yield myself by thread loop tick count
    void yield(int tick = 1);

    // yield myself for some time, waiting for son chroutine to be done.
    // will be awaken immediately when son is done, or time is out
    void wait(std::time_t wait_time_ms = 1000);
    
    // yield myself for some time, 
    // we should always use this instead of system sleep() !!!
    void sleep(std::time_t wait_time_ms);
    
    // create and run a chroutine in the lightest thread
    chroutine_id_t create_chroutine(func_t func, void *arg);

    // create and run a son chroutine for the current chroutine.
    // returns the son's result so the father can get what he want.
    // @timeout_ms controls the max time for the son to run.
    reporter_base_t * create_son_chroutine(func_t func, reporter_sptr_t reporter, std::time_t timeout_ms);

    // register a select object to current thread
    int register_select_obj(selectable_object_sptr_t select_obj);

    // get my chroutine id
    chroutine_id_t get_current_chroutine_id();

    // awake waiting chroutine
    int awake_chroutine(chroutine_id_t id);

private:    
    engine_t();
    void on_thread_ready(size_t creating_index, std::thread::id thread_id);
    chroutine_thread_t *get_current_thread();
    chroutine_thread_t *get_lightest_thread();
    
    // get current chroutine's reporter
    // (maybe no longer needed)
    reporter_base_t *get_my_reporter();

private:
    std::mutex          m_pool_lock;            // only used during m_init_over is false
    thread_pool_t       m_pool;                 // is readonly after m_init_over become true
    creating_threads_t  m_creating;             // is readonly after m_init_over become true
    bool                m_init_over = false;    // if all threads ready
};

#endif