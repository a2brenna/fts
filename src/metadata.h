#ifndef __METADATA_H__
#define __METADATA_H__

#include <chrono>
#include <mutex>

class Metadata {

    public:
        std::mutex lock;
        size_t num_elements = 0;
        size_t last_indexed = 0;
        std::chrono::high_resolution_clock::time_point last_timestamp;

        Metadata();

};

#endif
