
#include "chroutine.hpp"
#include <unistd.h>
#include <thread>
#include <iostream>

chroutine_manager_t& chroutine_manager_t::instance()
{
    static chroutine_manager_t instance;
    return instance;
}

chroutine_manager_t::chroutine_manager_t()
{
    start();
}

chroutine_manager_t::~chroutine_manager_t()
{}

void chroutine_manager_t::yield()
{
    chroutine_manager_t::instance().yield_current();
}

chroutine_t * chroutine_manager_t::get_chroutine(chroutine_id_t id)
{
    if (id < 0 || id > int(m_schedule.chroutines.size()) - 1)
        return nullptr;

    return m_schedule.chroutines[id].get();
}

void chroutine_manager_t::entry(void *arg)
{
    chroutine_manager_t *p_this = static_cast<chroutine_manager_t *>(arg);
    if (p_this == nullptr)
        return;

    chroutine_t * p_c = p_this->get_chroutine(p_this->m_schedule.running_id);
    if (p_c == nullptr)
        return;

    std::cout << "entry start, " << p_this->m_schedule.running_id << " left:" << p_this->m_schedule.chroutines.size()  << std::endl;
    p_c->state = chroutine_state_running;
    p_c->func(p_c->arg);
    p_this->m_schedule.chroutines.erase(p_this->m_schedule.chroutines.begin() + p_this->m_schedule.running_id);
    p_this->m_schedule.running_id = INVALID_ID;
    std::cout << "entry over, " << p_this->m_schedule.running_id << " left:" << p_this->m_schedule.chroutines.size() << std::endl;
}

chroutine_id_t chroutine_manager_t::create_chroutine(func_t func, void *arg)
{
    if (func == nullptr) 
        return -1;

    std::shared_ptr<chroutine_t> c(new chroutine_t);
    chroutine_t *p_c = c.get();
    if (p_c == nullptr)
        return -1;

    getcontext(&(p_c->ctx));
    p_c->ctx.uc_stack.ss_sp = p_c->stack;
    p_c->ctx.uc_stack.ss_size = STACK_SIZE;
    p_c->ctx.uc_stack.ss_flags = 0;
    p_c->ctx.uc_link = &(m_schedule.main);
    p_c->func = func;
    p_c->arg = arg;
    p_c->state = chroutine_state_ready;
    makecontext(&(p_c->ctx),(void (*)(void))(entry), 1, this);

    chroutine_id_t id = INVALID_ID;
    {
        //std::lock_guard<std::mutex> lck (m_schedule.mutex);
        m_schedule.chroutines.push_back(c);
        id = m_schedule.chroutines.size() - 1;
    }

    std::cout << "create_chroutine over, " << id << std::endl;
    
    // m_schedule.running_id = id;
    // m_schedule.run_start_time = get_time_stamp();
    // swapcontext(&(m_schedule.main), &(p_c->ctx));
    return id;
}

void chroutine_manager_t::yield_current()
{
    if (m_schedule.running_id < 0 || m_schedule.running_id > int(m_schedule.chroutines.size())-1)
        return;
    
    chroutine_t * co = m_schedule.chroutines[m_schedule.running_id].get();
    if (co == nullptr || co->state != chroutine_state_running)
        return;
    
    std::cout << "yield_current ..." << m_schedule.running_id << std::endl;
    co->state = chroutine_state_suspend;
    m_schedule.running_id = INVALID_ID;
    swapcontext(&(co->ctx), &(m_schedule.main));
}

bool chroutine_manager_t::done()
{
    return m_schedule.chroutines.empty();
}

void chroutine_manager_t::resume_to(chroutine_id_t id)
{
    if (id < 0 || id > int(m_schedule.chroutines.size())-1)
        return;

    chroutine_t * co = m_schedule.chroutines[id].get();
    if (co == nullptr || co->state != chroutine_state_suspend)
        return;
    
    std::cout << "resume_to ..." << id << std::endl;
    swapcontext(&(m_schedule.main),&(co->ctx));
    std::cout << "resume_to ..." << id << " over" << std::endl;
}

chroutine_id_t chroutine_manager_t::pick_run_chroutine()
{
    if (m_schedule.running_id != INVALID_ID)
        return m_schedule.running_id;

    if (m_schedule.chroutines.empty())
        return INVALID_ID;

    chroutine_id_t index = INVALID_ID;
    chroutine_t *p_c = nullptr;
    for (auto &node : m_schedule.chroutines) {
        index++;
        p_c = node.get();
        if (p_c != nullptr)
            break;
    }

    if (p_c) {
        std::cout << "pick_run_chroutine ..." << index << std::endl;
        p_c->state = chroutine_state_running;
        m_schedule.running_id = index;

        // if (p_c->state == chroutine_state_ready) {
        //     m_schedule.run_start_time = get_time_stamp();
        // }
        swapcontext(&(m_schedule.main),&(p_c->ctx));

        std::cout << "pick_run_chroutine ..." << index << " over" << std::endl;

    }
    return index;
}

int chroutine_manager_t::schedule()
{
    while (1) {
        // if (m_schedule.running_id == INVALID_ID) {
        //     pick_run_chroutine();

        // } else {
        //     time_t now = get_time_stamp();
        //     if (now - m_schedule.run_start_time > MAX_RUN_MS_EACH &&
        //         m_schedule.chroutines.size() > 1) {
        //         yield_current();
        //         pick_run_chroutine();
        //     }            
        // }

        pick_run_chroutine();
        if (done())
            usleep(10000);
    }
    return 0;
}

void chroutine_manager_t::start()
{
    std::thread thrd( [this] { this->schedule(); } );
    thrd.detach();
}

std::time_t chroutine_manager_t::get_time_stamp()
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}