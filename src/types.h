#ifndef __SOME_TYPES_H__
#define __SOME_TYPES_H__

#include <chrono>
#include <string>

class E_BAD_ARCHIVE {};
class E_END_OF_ARCHIVE {};

struct Index_Record{
    uint64_t offset = 0;
    uint64_t index = 0;
    uint64_t timestamp = 0;
};

class Archive {

    public:
        Archive(const std::string &raw);
        Archive();
        std::chrono::high_resolution_clock::time_point current_time() const;
        size_t current_index() const;
        std::string current_record() const;
        std::string remainder() const;

        void next_record();

    private:
        std::string _archive;
        size_t _cursor = 0;
        size_t _current_index = 0;

};

#endif
