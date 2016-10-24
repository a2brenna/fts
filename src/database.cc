#include "database.h"

const size_t MIN_INDEX = 0;
const size_t MAX_INDEX = std::numeric_limits<size_t>::max();
const size_t MAX_ENTRIES = std::numeric_limits<size_t>::max();
const auto MIN_TIMESTAMP = std::chrono::milliseconds::min();
const auto MAX_TIMESTAMP = std::chrono::milliseconds::max();

bool Database::append(const std::string &key, const std::string &data){
    return append(key, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()), data);
}

/*
bool Database::append_archive(const std::string &key, const Archive &archive){
}
*/

std::string Database::lastn(const std::string &key, const unsigned long long &num_entries){
    return query(key, MIN_TIMESTAMP, MAX_TIMESTAMP, MIN_INDEX, MAX_INDEX, num_entries, MAX_TIMESTAMP);
}

std::string Database::all(const std::string &key){
    return query(key, MIN_TIMESTAMP, MAX_TIMESTAMP, MIN_INDEX, MAX_INDEX, MAX_INDEX, MAX_TIMESTAMP);
}

std::string Database::intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end){
    return query(key, start, end, MIN_INDEX, MAX_INDEX, MAX_ENTRIES, MAX_TIMESTAMP);
}

std::string Database::str() const {
    return "";
}
