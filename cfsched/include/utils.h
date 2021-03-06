#pragma once

#include <thread>
#include <chrono>
#include <cmath>
#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <fstream>
#include <istream>

namespace cfsched{

/*==================================================
 A collection of thread/time/random/numeric utils.
===================================================*/

    // Thread-related
    static inline int nr_cpu(){ // 2*actual cpu number due to intel hyperthreading 
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

    // Colorful & Concurrent Print Line 
    void println(const std::string& what);




}