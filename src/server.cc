#include "server.h"
#include <cassert>
#include "metadata.h"

Server::Server(std::shared_ptr<Object_Store> backend, const std::string &prefix){
    _backend = backend;
    _prefix = prefix;
}

std::shared_ptr<Metadata> Server::_get_metadata(const std::string &key) const{
    std::unique_lock<std::mutex> l(_metadata_lock);
    return _metadata.at(key);
}

std::shared_ptr<Metadata> Server::_get_or_create_metadata(const std::string &key){
    std::unique_lock<std::mutex> l(_metadata_lock);
    return _metadata[key];
}

bool Server::append(const std::string &key, const std::string &data){
    return append(key, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()), data);
}

bool Server::append(const std::string &key, const std::chrono::milliseconds &time,
            const std::string &data){

    std::shared_ptr<Metadata> metadata = _get_or_create_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);

    const std::chrono::high_resolution_clock::time_point t(time);
    if(t <= metadata->last_timestamp){
        return false;
    }

    const std::string backend_key = _prefix + key;

    const uint64_t data_size = data.size();
    const uint64_t data_timestamp = time.count();

    std::string backend_string;
    backend_string.append( (char *)&data_timestamp, sizeof(uint64_t) );
    backend_string.append( (char *)&data_size, sizeof(uint64_t) );
    backend_string.append( data );

    try{
        _backend->append(backend_key, backend_string);
        metadata->last_timestamp = t;
        metadata->last_indexed++;
        metadata->num_elements++;
        return true;
    }
    catch(...){
        return false;
    }

}

/*
bool Server::append_archive(const std::string &key, const Archive &archive){

}

Archive Server::lastn(const std::string &key, const unsigned long long &num_entries){

}

Archive Server::all(const std::string &key){

}

Archive Server::intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end){

}
*/
