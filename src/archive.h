#ifndef __SOME_TYPES_H__
#define __SOME_TYPES_H__

#include <chrono>
#include <string>

class E_BAD_ARCHIVE {};
class E_END_OF_ARCHIVE {};

class Archive {

    public:
        Archive(const std::string &raw);
        Archive();
        std::chrono::milliseconds current_time() const;
        size_t current_index() const;
        size_t size() const;
        std::string current_data() const;
        std::string current_record() const;
        std::string remainder() const;
        std::string str() const;

        void next_record();

    private:
        std::string _archive;
        size_t _cursor = 0;
        size_t _current_index = 0;

};

#endif
