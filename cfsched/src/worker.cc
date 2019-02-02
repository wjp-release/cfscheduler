#include "worker.h"
#include <sstream>
namespace cfsched{

std::string Worker::stats(){
    std::stringstream ss;
    ss<<"worker "<<(int)id;
    ss<<", arena stats: "<<arena.stats()<<", buffer stats: "<<buffer.stats();
    return ss.str();
}


}
