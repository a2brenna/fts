#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include <mutex>
#include <memory>
#include <string>
#include <rtos/object_store.h>

class Manifest {

    public:
        std::string str() const;

    private:
        mutable std::mutex _lock;
        std::shared_ptr<Object_Store> _backend;

};

#endif
