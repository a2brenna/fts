#ifndef __INDEX_H__
#define __INDEX_H__

#include <chrono>
#include <string>

struct Index_Record{
    uint64_t offset = 0;
    uint64_t index = 0;
    uint64_t timestamp = 0;
};

class Index{

    public:
        Index(const std::string &serialized_index);
        std::pair<size_t, size_t> lookup(const std::chrono::high_resolution_clock::time_point &s, const std::chrono::high_resolution_clock::time_point &e);
        std::pair<size_t, size_t> lookup(const size_t &s, const size_t &e);

    private:
        std::string _buffer;
        size_t _cursor;

};

#endif
