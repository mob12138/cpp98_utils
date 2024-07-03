#ifndef CALU_RUNTIME_H
#define CALU_RUNTIME_H

#include <ctime>

class calc_runtime
{
public:
    calc_runtime() { start(); }
    virtual ~calc_runtime() {}

public:
    void start() { reset(); }
    void end()
    {
        m_tEnd = ::clock();
        m_tRunTime = m_tEnd - m_tStart;
    }
    void reset()
    {
        m_tStart = ::clock();
        m_tEnd = 0;
        m_tRunTime = 0;
    }

    clock_t start_time() const { return m_tStart; }
    clock_t end_time() const { return m_tEnd; }
    clock_t run_time() const { return m_tRunTime; }
    double run_time_in_second() const { return (double)m_tRunTime / CLOCKS_PER_SEC; }

private:
    clock_t m_tStart;
    clock_t m_tEnd;
    clock_t m_tRunTime;
};

#endif // CALU_RUNTIME_H