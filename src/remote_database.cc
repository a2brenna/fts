#include "remote_database.h"
#include "wire_protocol.pb.h"
#include <cassert>

#include <iostream>

Remote_Database::Remote_Database(std::shared_ptr<smpl::Remote_Address> server_address){
    _server_address = server_address;
    _server = std::shared_ptr<smpl::Channel>(_server_address->connect());
}

sls2::Response Remote_Database::_perform(const sls2::Request &request){
    std::cerr << "Request: " << request.DebugString() << std::endl;
    std::string serialized_request;
    request.SerializeToString(&serialized_request);

    _server->send(serialized_request);

    const std::string serialized_response = _server->recv();

    sls2::Response response;
    response.ParseFromString(serialized_response);
    return response;
}

bool Remote_Database::append(const std::string &key, const std::chrono::milliseconds &time,
            const std::string &data){

    sls2::Request request;
    request.set_key(key);

    sls2::Append *append = request.mutable_append();
    append->set_data(data);
    append->set_millitime(time.count());

    const sls2::Response response = _perform(request);
    return(response.result() == sls2::Response::SUCCESS);
}

std::string Remote_Database::query(const std::string &key,
    const std::chrono::milliseconds &youngest, const std::chrono::milliseconds &oldest,
    const size_t &min_index, const size_t &max_index,
    const size_t &tail_size, const std::chrono::milliseconds &tail_age){

    sls2::Request request;
    request.set_key(key);

    sls2::Query *query = request.mutable_query();
    query->set_youngest(youngest.count());
    query->set_oldest(oldest.count());
    query->set_min_index(min_index);
    query->set_max_index(max_index);
    query->set_tail_size(tail_size);
    query->set_tail_age(tail_age.count());

    sls2::Response response = _perform(request);

    if(response.result() == sls2::Response::BYTES_TO_FOLLOW){
        const std::string query_result = _server->recv();
        return query_result;
    }
    else{
        return "";
    }
}

std::string Remote_Database::str() const{
    return "";
}
