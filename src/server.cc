#include "server.h"
#include <cassert>
#include "metadata.h"
#include "types.h"

Server::Server(std::shared_ptr<Object_Store> backend, const std::string &prefix){
    _backend = backend;
    _prefix = prefix;
}

uint64_t record_extract_timestamp(const char *record){
    return *(uint64_t *)(record);
}

uint64_t record_extract_size(const char *record){
    return *(uint64_t *)(record + sizeof(uint64_t));
}

std::string record_extract_data(const char* record){
    return std::string(record + (sizeof(uint64_t) * 2), record_extract_size(record));
}

std::shared_ptr<Metadata> Server::_unsafe_get_metadata(const std::string &key){
    try{
        return _metadata.at(key);
    }
    catch(std::out_of_range metadata_dne){
        const std::string backend_key = _prefix + key;

        try{
            //TODO: return pointer to Data if we have to fetch it since we might need it to save ourselves a fetch
            const Data data = _backend->fetch(backend_key);

            const char *record = data.data().c_str();
            const char *archive_end = record + data.data().size();
            size_t num_elements = 0;
            uint64_t latest_timestamp = 0;
            while(record < archive_end){
                num_elements++;
                latest_timestamp = record_extract_timestamp(record);
                record += record_extract_size(record) + (sizeof(uint64_t) * 2);
            }

            std::shared_ptr<Metadata> m(new Metadata());
            m->num_elements = num_elements;
            m->last_timestamp = std::chrono::high_resolution_clock::time_point(std::chrono::milliseconds(latest_timestamp));
            m->size = data.data().size();
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
    if( (metadata->num_elements + 1) % _index_resolution == 0){
        write_index = true;
    }

    Index_Record i;
    i.offset = metadata->size;
    i.index = metadata->num_elements;
    i.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(metadata->last_timestamp.time_since_epoch()).count();


    const std::string backend_key = _prefix + key;

    const uint64_t data_size = data.size();
    const uint64_t data_timestamp = time.count();

    std::string backend_string;
    backend_string.append( (char *)&data_timestamp, sizeof(uint64_t) );
    backend_string.append( (char *)&data_size, sizeof(uint64_t) );
    backend_string.append( data );

    _backend->append(backend_key, backend_string);

    metadata->last_timestamp = t;
    metadata->size += backend_string.size();
    metadata->num_elements++;

    if(write_index){
        static_assert(sizeof(Index_Record) == 24, "Index_Record is not 24 bytes");
        std::string index_string;
        index_string.append( (char *)&i, sizeof(Index_Record));

        const std::string index_key = backend_key + ".index";
        _backend->append(index_key, index_string);
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
    std::shared_ptr<Metadata> metadata = _get_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);

    const std::string backend_key = _prefix + key;
    Archive a(_backend->fetch(backend_key).data());

    const size_t lower_bound = std::max((ssize_t)0, (ssize_t)metadata->num_elements - (ssize_t)num_entries);

    while(a.current_index() < lower_bound){
        a.next_record();
    }

    return a.remainder();
}

std::string Server::all(const std::string &key){
    //Does not require metadata/lock, atomic r/w property of backend ensures we only get complete archives
    const std::string backend_key = _prefix + key;
    try{
        const Data d = _backend->fetch(backend_key);
        return d.data();
    }
    catch(E_OBJECT_DNE){
        throw E_TIMESERIES_DNE();
    }
}

std::string Server::intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end){
    std::shared_ptr<Metadata> metadata = _get_metadata(key);
    std::unique_lock<std::mutex> m(metadata->lock);

    const std::string backend_key = _prefix + key;
    Archive a(_backend->fetch(backend_key).data());

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

}
