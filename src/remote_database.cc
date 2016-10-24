#include "remote_database.h"
#include <cassert>

Remote_Database::Remote_Database(std::shared_ptr<smpl::Remote_Address> server_address){
    _server_address = server_address;
    //TODO: Open Channel
}

bool Remote_Database::append(const std::string &key, const std::chrono::milliseconds &time,
            const std::string &data){
    //TODO: Send to server, get response
}

std::string Remote_Database::query(const std::string &key,
    const std::chrono::milliseconds &youngest, const std::chrono::milliseconds &oldest,
    const size_t &min_index, const size_t &max_index,
    const size_t &tail_size, const std::chrono::milliseconds &tail_age){
    //TODO: Send to server, get response

}

std::string Remote_Database::str() const{
    return "";
}
