#include "utils.h"
#include <mutex>

static std::mutex mtx;

namespace cfsched{
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
    std::ostream& operator<<(std::ostream& os, Code code) {
        return os << "\033[" << static_cast<int>(code) << "m";
    }
}
void println(const std::string& what){
    std::lock_guard<std::mutex> lk(mtx);
    std::cout<< (Color::Code)(Color::fg_red+(current_thread_hashed_id()%7)) 
        << what<< (Color::Code)(Color::fg_default) <<std::endl;
}

}