#include "types.h"

#include <cassert>

Archive::Archive(const std::string &raw){
    _archive = raw;
}

Archive::Archive(){

}

std::chrono::high_resolution_clock::time_point Archive::current_time() const{
    std::chrono::milliseconds since_epoch(*(uint64_t *)(&_archive[_cursor]));
    return std::chrono::high_resolution_clock::time_point(since_epoch);
}

size_t Archive::current_index() const{
    return _current_index;
}

std::string Archive::current_record() const{
    const size_t record_size = *(uint64_t *)(&_archive[_cursor] + sizeof(uint64_t));
    if( (_cursor + record_size + sizeof(uint64_t) * 2) > _archive.size() ){
        throw E_BAD_ARCHIVE();
    }
    else{
        return std::string(_archive, _cursor + (sizeof(uint64_t) * 2), record_size);
    }
    assert(false);
}

std::string Archive::remainder() const{
    return std::string(_archive, _cursor, _archive.size() - _cursor);
}

void Archive::next_record(){
    const size_t record_size = *(uint64_t *)(&_archive[_cursor] + sizeof(uint64_t));
    const size_t new_cursor = sizeof(uint64_t) * 2 + record_size;

    if(new_cursor > _archive.size()){
        throw E_END_OF_ARCHIVE();
    }
    else{
        _current_index++;
        _cursor = new_cursor;
        return;
    }
    assert(false);
}
