#include "arena.h"
#include <sstream>
namespace cfsched{

std::string Arena::stats(){
    std::stringstream ss;

    return ss.str();
}

Arena::Arena(){
    for(auto& t:tasks) freeList.push(&t);
}


}