#include "index.h"
#include <cassert>
#include <cstring>

Index::Index(const std::string &serialized_index){
    const size_t index_bytes = serialized_index.size();

    if(index_bytes % sizeof(Index_Record) != 0){
        throw E_CORRUPT_INDEX();
    }

    _index.reserve(index_bytes / sizeof(Index_Record));
    std::memcpy(_index.data(), &serialized_index[0], index_bytes);

    _consistency_check();
}

void Index::_consistency_check() const{
    const Index_Record* current = &_index[0];
    for(size_t c = 1; c < _index.size(); c++){
        if (
                (_index[c].offset < current->offset) ||
                (_index[c].index < current->index) ||
                (_index[c].timestamp < current->timestamp)
        ){
            throw E_CORRUPT_INDEX();
        }
        else{
            current = &_index[c];
        }
    }
}

std::pair<size_t, size_t> Index::lookup(const size_t &s, const size_t &e){
    std::pair<size_t, size_t> r(0, std::numeric_limits<size_t>::max());

    //TODO: Replace with binary search
    size_t c = 0;
    for( ; c < _index.size(); c++){
        if ( _index[c].index > s ){
            break;
        }
        else{
            r.first = _index[c].offset;
        }
    }

    for( ; c < _index.size(); c++){
        if ( _index[c].index > e ){
            break;
        }
        else{
            r.second = _index[c].offset;
        }
    }

    return r;
}

std::pair<size_t, size_t> Index::lookup(const std::chrono::high_resolution_clock::time_point &start, const std::chrono::high_resolution_clock::time_point &end){
    std::pair<size_t, size_t> r(0, std::numeric_limits<size_t>::max());

    size_t s = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
    size_t e = std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()).count();

    //TODO: Replace with binary search
    size_t c = 0;
    for( ; c < _index.size(); c++){
        if ( _index[c].timestamp > s ){
            break;
        }
        else{
            r.first = _index[c].offset;
        }
    }

    for( ; c < _index.size(); c++){
        if ( _index[c].timestamp > e ){
            break;
        }
        else{
            r.second = _index[c].offset;
        }
    }

    return r;
}
