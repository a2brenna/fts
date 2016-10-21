#ifndef __METADATA_H__
#define __METADATA_H__

#include <chrono>
#include <mutex>
#include <memory>
#include <rtos/types.h>
#include <rtos/object_store.h>

class Metadata {

    public:
        mutable std::mutex lock;
        size_t num_elements = 0;
        size_t size = 0;
        std::chrono::high_resolution_clock::time_point last_timestamp;

        Metadata();
        Metadata(const Ref &data_ref, std::shared_ptr<Object_Store> backend, const size_t &index, const size_t &offset);

};

#endif
