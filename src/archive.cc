#include "archive.h"
#include <sstream>
#include <cassert>

Archive::Archive(const std::string &raw){
    _archive = raw;
}

Archive::Archive(){

}

std::chrono::milliseconds Archive::current_time() const{
    std::chrono::milliseconds since_epoch(*(uint64_t *)(&_archive[_cursor]));
    return since_epoch;
}

size_t Archive::current_index() const{
    return _current_index;
}

size_t Archive::size() const{
    return _archive.size();
}

std::string Archive::current_data() const{
    const size_t data_size = *(uint64_t *)(&_archive[_cursor] + sizeof(uint64_t));
    if( (_cursor + data_size + sizeof(uint64_t) * 2) > _archive.size() ){
        if(_cursor == size()){
            throw E_END_OF_ARCHIVE();
        }
        else{
            throw E_BAD_ARCHIVE();
        }
    }
    else{
        return std::string(_archive, _cursor + (sizeof(uint64_t) * 2), data_size);
    }
    assert(false);
}

std::string Archive::remainder() const{
    return std::string(_archive, _cursor, _archive.size() - _cursor);
}

std::string Archive::current_record() const{
    const size_t record_size = *(uint64_t *)(&_archive[_cursor] + sizeof(uint64_t)) + (sizeof(uint64_t) * 2);
    if( (_cursor + record_size) > _archive.size() ){
        if(_cursor == size()){
            throw E_END_OF_ARCHIVE();
        }
        else{
            throw E_BAD_ARCHIVE();
        }
    }
    else{
        return std::string(_archive, _cursor, record_size);
    }
    assert(false);
}

void Archive::next_record(){
    const size_t record_size = *(uint64_t *)(&_archive[_cursor] + sizeof(uint64_t));
    const size_t new_cursor = _cursor + sizeof(uint64_t) * 2 + record_size;

    if(new_cursor == _archive.size()){
        throw E_END_OF_ARCHIVE();
    }
    else if(new_cursor > _archive.size()){
        throw E_BAD_ARCHIVE();
    }
    else{
        _current_index++;
        _cursor = new_cursor;
        return;
    }
    assert(false);
}

std::string Archive::str(){
    std::stringstream output;
    for(;;){
        try{
            output << current_index() << " "
                << current_time().count() << " "
                << current_data()
                << std::endl;
            next_record();
        }
        catch(E_END_OF_ARCHIVE){
            break;
        }
    }

    return output.str();
}
