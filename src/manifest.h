#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include <mutex>
#include <memory>
#include <rtos/object_store.h>

class Manifest {

    private:
        mutable std::mutex _lock;
        std::shared_ptr<Object_Store> _backend;

};

#endif
