#include "utils.h"
#include <mutex>

static std::mutex mtx;

using namespace cfsched::Color;

namespace cfsched{

void println(const std::string& what){
    std::lock_guard<std::mutex> lk(mtx);
    std::cout<< (Color::Code)(Color::fg_red+(current_thread_hashed_id()%7)) 
        << what<< (Color::Code)(Color::fg_default) <<std::endl;
}

}