#include "index.h"
#include <cassert>

Index::Index(const std::string &serialized_index){
    _buffer = serialized_index;
    _consistency_check();
}

void Index::_consistency_check() const{
    if(_buffer.size() % 24 != 0){
        throw E_CORRUPT_INDEX();
    }

    if(_buffer.size() > 0){
        Index_Record *current = (Index_Record *)(&_buffer[0]);
        for(size_t c = 24; c < (_buffer.size() / 24); c += 24){
            if (
                    (((Index_Record *)(&_buffer[c]))->offset < current->offset) ||
                    (((Index_Record *)(&_buffer[c]))->index < current->index) ||
                    (((Index_Record *)(&_buffer[c]))->timestamp < current->timestamp)
            ){
                throw E_CORRUPT_INDEX();
            }
            else{
                current = (Index_Record *)(&_buffer[c]);
            }
        }
    }
}

std::pair<size_t, size_t> Index::lookup(const size_t &s, const size_t &e){
    std::pair<size_t, size_t> r(0, std::numeric_limits<size_t>::max());

    //TODO: Replace with binary search
    size_t c = 0;
    for( ; c < _buffer.size(); c += 24){
        if ( ((Index_Record *)(&_buffer[c]))->index > s ){
            break;
        }
        else{
            r.first = ((Index_Record *)(&_buffer[c]))->offset;
        }
    }

    for( ; c < _buffer.size(); c += 24){
        if ( ((Index_Record *)(&_buffer[c]))->index > e ){
            break;
        }
        else{
            r.second = ((Index_Record *)(&_buffer[c]))->offset;
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
    for( ; c < _buffer.size(); c += 24){
        if ( ((Index_Record *)(&_buffer[c]))->timestamp > s ){
            break;
        }
        else{
            r.first = ((Index_Record *)(&_buffer[c]))->offset;
        }
    }

    for( ; c < _buffer.size(); c += 24){
        if ( ((Index_Record *)(&_buffer[c]))->timestamp > e ){
            break;
        }
        else{
            r.second = ((Index_Record *)(&_buffer[c]))->offset;
        }
    }

    return r;
}
