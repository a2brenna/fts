#include "server.h"
#include <cassert>
#include "metadata.h"
#include "archive.h"
#include "index.h"

const size_t MIN_INDEX = 0;
const size_t MAX_INDEX = std::numeric_limits<size_t>::max();

const auto MIN_TIMESTAMP = std::chrono::milliseconds::min();
const auto MAX_TIMESTAMP = std::chrono::milliseconds::max();

Server::Server(std::shared_ptr<Object_Store> backend, const std::string &prefix){
    _backend = backend;
    _prefix = prefix;
}

Ref Server::_ts_ref(const std::string &key) const{
    return Ref(_prefix + ":client_timeseries:" + key);
}

Ref Server::_index_ref(const std::string &key) const{
    return Ref(_prefix + ":client_index:" + key);
}

std::shared_ptr<Metadata> Server::_unsafe_get_metadata(const std::string &key){
    try{
        return _metadata.at(key);
    }
    catch(std::out_of_range metadata_dne){
        try{
            //TODO: return pointer to Object if we have to fetch it since we might need it to save ourselves a fetch
            std::shared_ptr<Metadata> m(new Metadata(_ts_ref(key), _backend, 0, 0));
            _metadata.insert(std::pair<std::string, std::shared_ptr<Metadata>>(key, m));
            return m;
        }
        catch(E_OBJECT_DNE o){
            throw E_TIMESERIES_DNE();
        }
    }
}

std::shared_ptr<Metadata> Server::_get_metadata(const std::string &key){
    std::unique_lock<std::mutex> l(_metadata_lock);
    return _unsafe_get_metadata(key);
}

std::shared_ptr<Metadata> Server::_get_or_create_metadata(const std::string &key){
    std::unique_lock<std::mutex> l(_metadata_lock);
    try{
        return _unsafe_get_metadata(key);
    }
    catch(E_TIMESERIES_DNE timeseries_dne){
        //TODO: fix this to insert some metadata AND return pointer
        std::shared_ptr<Metadata> m(new Metadata());
        _metadata[key] = m;
        return m;
    }
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
        throw E_ORDER();
    }

    bool write_index = false;
    if( (metadata->num_elements) % _index_resolution == 0){
        write_index = true;
    }

    Index_Record i;
    i.offset = metadata->size;
    i.index = metadata->num_elements + 1;
    i.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count();

    const uint64_t data_size = data.size();
    const uint64_t data_timestamp = time.count();

    std::string backend_string;
    backend_string.append( (char *)&data_timestamp, sizeof(uint64_t) );
    backend_string.append( (char *)&data_size, sizeof(uint64_t) );
    backend_string.append( data );

    _backend->append(_ts_ref(key), backend_string);

    metadata->last_timestamp = t;
    metadata->size += backend_string.size();
    metadata->num_elements++;

    if(write_index){
        static_assert(sizeof(Index_Record) == 24, "Index_Record is not 24 bytes");
        _backend->append(_index_ref(key), (char *)&i, sizeof(Index_Record));
    }
    return true;
}

/*
bool Server::append_archive(const std::string &key, const Archive &archive){
    std::shared_ptr<Metadata> metadata = _get_or_create_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);

}
*/

std::string Server::lastn(const std::string &key, const unsigned long long &num_entries){
    return query(key, MIN_TIMESTAMP, MAX_TIMESTAMP, MIN_INDEX, MAX_INDEX, num_entries, MAX_TIMESTAMP);
    /*
    std::shared_ptr<Metadata> metadata = _get_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);

    Archive a(_backend->fetch(_ts_ref(key)).data());

    const size_t lower_bound = std::max((ssize_t)0, (ssize_t)metadata->num_elements - (ssize_t)num_entries);

    while(a.current_index() < lower_bound){
        a.next_record();
    }

    return a.remainder();
    */
}

std::string Server::all(const std::string &key){
    std::shared_ptr<Metadata> metadata = _get_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);
    const Object d = _backend->fetch(_ts_ref(key));
    return d.data();
}

std::string Server::intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end){
    return query(key, start, end, MIN_INDEX, MAX_INDEX, MAX_INDEX, MAX_TIMESTAMP);
    /*
    std::shared_ptr<Metadata> metadata = _get_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);

    Archive a(_backend->fetch(_ts_ref(key)).data());

    const std::chrono::high_resolution_clock::time_point s(start);
    const std::chrono::high_resolution_clock::time_point e(end);

    std::string output;

    try{
        while(a.current_time() < s){
            a.next_record();
        }

        while(a.current_time() < e){
            output.append(a.current_record());
            a.next_record();
        }
    }
    catch(E_END_OF_ARCHIVE e){

    }

    return output;
    */

}

std::string Server::query(const std::string &key,
    const std::chrono::milliseconds &youngest, const std::chrono::milliseconds &oldest,
    const size_t &min_index, const size_t &max_index,
    const size_t &tail_size, const std::chrono::milliseconds &tail_age){

    assert(youngest >= oldest);
    assert(max_index >= min_index);

    std::shared_ptr<Metadata> metadata = _get_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);

    const size_t last_index = metadata->num_elements - 1;
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

    const size_t bounded_min_index = std::max(min_index, last_index - tail_size);
    const size_t bounded_max_index = std::min(max_index, last_index);
    assert(bounded_min_index >= bounded_max_index);

    const std::chrono::high_resolution_clock::time_point bounded_min_timestamp(std::max(oldest, now - tail_age));
    const std::chrono::high_resolution_clock::time_point bounded_max_timestamp(std::min(youngest, now));
    assert(bounded_min_timestamp >= bounded_max_timestamp);

    std::string output;

    Archive a(_backend->fetch(_ts_ref(key)).data());

    try{
        size_t i = 0;
        while( (a.current_time() < bounded_min_timestamp) || (i < bounded_min_index) ){
            a.next_record();
            i++;
        }

        while( (a.current_time() <= bounded_max_timestamp) || (i <= bounded_max_index) ){
            output.append(a.current_record());
            a.next_record();
            i++;
        }
    }
    catch(E_END_OF_ARCHIVE e){

    }

    return output;
}

Index Server::index(const std::string &key){
    const Ref index_ref = _index_ref(key);
    const std::string serialized_index = _backend->fetch(index_ref).data();
    return Index(serialized_index);
}
