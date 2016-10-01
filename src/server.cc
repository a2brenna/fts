#include "server.h"
#include <cassert>

Server::Server(std::shared_ptr<Object_Store> backend, const std::string &prefix){
    _backend = backend;
    _prefix = prefix;
}

bool Server::append(const std::string &key, const std::string &data){
    return append(key, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()), data);
}

bool Server::append(const std::string &key, const std::chrono::milliseconds &time,
            const std::string &data){

    //TODO: check that time > last_time

    const std::string backend_key = _prefix + key;
    //hash key to 256 bits and base 16 encode

    const uint64_t data_size = data.size();
    const uint64_t data_timestamp = time.count();

    std::string backend_string;
    backend_string.append( (char *)&data_timestamp, sizeof(uint64_t) );
    backend_string.append( (char *)&data_size, sizeof(uint64_t) );
    backend_string.append( data );

    _backend->append(backend_key, backend_string);
    return true;
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
