#pragma once

#include <thread>
#include <chrono>
#include <cmath>
#include <vector>
#include <random>
#include <iostream>

namespace cfsched{
 
    // Thread-related
    static inline int nr_cpu(){
        return std::thread::hardware_concurrency();
    }

    static inline void sleep(int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    // Not a unique id. Hasher still has small chance to collide. 
    static inline std::size_t current_thread_hashed_id()
    {
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    }

    // Chrono/Time
    using days = std::chrono::duration<int, std::ratio<3600*24,1>>;
    using time_point = std::chrono::high_resolution_clock::time_point;

    static inline time_point now()
    {
        return std::chrono::high_resolution_clock::now();
    }
    
    template <class Rep, class Period>
    constexpr std::chrono::milliseconds to_ms(const std::chrono::duration<Rep,Period>& d){
        return std::chrono::duration_cast<std::chrono::milliseconds>(d);
    }

    template <class Rep, class Period>
    constexpr std::chrono::seconds to_sec(const std::chrono::duration<Rep,Period>& d){
        return std::chrono::duration_cast<std::chrono::seconds>(d);
    }

    static inline int64_t duration_ms_count(time_point start, time_point end)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    static inline int64_t ms_elapsed_count(time_point start)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(now() - start).count();
    }

    static inline int64_t microsec_elapsed_count(time_point start)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(now() - start).count();
    }

    // Simple Numerics
    template <class T>
    double average(T* samples, int size)
    {
        double sum=0;
        for(int i=0;i<size;i++)sum+=samples[i];
        return sum/(double)size;
    }

    template <class T>
    double variance(T* samples, int size)
    {
        double variance_ = 0;
        T t = samples[0];
        for (int i = 1; i < size; i++)
        {
            t += samples[i];
            T diff = ((i + 1) * samples[i]) - t;
            variance_ += (diff * diff) / ((i + 1.0) *i);
        }
        return variance_ / (size - 1);
    }

    template < class T >
    double standard_deviation(T* samples, int n)
    {
        return std::sqrt(variance(samples, n));
    }


    // Random
    template <int MIN, int MAX>
    int randint(){
        static std::uniform_int_distribution<unsigned> u(MIN,MAX);
        static std::default_random_engine e((unsigned)time(0));
        return u(e);
    }

    static inline int randinteger(int min, int max){
        std::uniform_int_distribution<unsigned> u(min,max);
        static std::default_random_engine e((unsigned)time(0));
        return u(e);
    }

    // Prettier print
    namespace Color{
        enum Code : int {
            fg_black    = 30,
            fg_red      = 31,
            fg_green    = 32,
            fg_yellow   = 33,
            fg_blue     = 34,
            fg_magenta  = 35,
            fg_cyan     = 36,
            fg_white    = 37,
            fg_default  = 39,
            bg_black    = 40,
            bg_red      = 41,
            bg_green    = 42,
            bg_yellow   = 43,
            bg_blue     = 44,
            bg_magenta  = 45,
            bg_cyan     = 46,
            bg_white    = 47,
            bg_default  = 49
        };
        static inline std::ostream& operator<<(std::ostream& os, Code code) {
            return os << "\033[" << static_cast<int>(code) << "m";
        }
    }
    // Colorful & Concurrent Print Line 
    void println(const std::string& what);




}