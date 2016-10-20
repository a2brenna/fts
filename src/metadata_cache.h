#ifndef __METADATA_CACHE_H__
#define __METADATA_CACHE_H__

#include <mutex>
#include <memory>
#include <rtos/object_store.h>

class Metadata_Cache {

    private:
        mutable std::mutex _lock;
        bool _dirty = false;

        std::shared_ptr<Object_Store> _backend;

};

#endif
