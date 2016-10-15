#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <rtos/fs_store.h>
#include <cassert>
#include <memory>
#include <thread>

#include "server.h"

namespace po = boost::program_options;

std::string DIRECTORY = "./fts.db";
std::string PREFIX;

int main(int argc, char* argv[]){
    std::string timeseries;
    bool append = false;
    bool query_time = false;
    bool query_index = false;
    bool all = false;
    std::string data;
    uint64_t start_time = std::numeric_limits<uint64_t>::min();
    uint64_t end_time = std::numeric_limits<uint64_t>::max();
    uint64_t timestamp = std::numeric_limits<uint64_t>::min();
    size_t start_index = 0;
    size_t end_index = std::numeric_limits<size_t>::max();
    size_t tail = 0;

	po::options_description desc("Options");
	desc.add_options()
        ("directory", po::value<std::string>(&DIRECTORY), "Backend storage directory")
        ("prefix", po::value<std::string>(&PREFIX), "Unique database name")
        ("timeseries", po::value<std::string>(&timeseries), "Timeseries to operate on/query")
        ("append", po::bool_switch(&append), "Append data to timeseries")
        ("query_time", po::bool_switch(&query_time), "Query timeseries by time window")
        ("query_index", po::bool_switch(&query_index), "Query timeseries by index window")
        ("all", po::bool_switch(&query_index), "Fetch entire timeseries")
        ("data", po::value<std::string>(&data), "Data to append")
        ("timestamp", po::value<uint64_t>(&timestamp), "Timestamp to append data at")
        ("start_time", po::value<uint64_t>(&start_time), "Start time of query window")
        ("end_time", po::value<uint64_t>(&end_time), "End time of query window")
        ("start_index", po::value<size_t>(&start_index), "Start index of query window")
        ("end_index", po::value<size_t>(&end_index), "End index of query window")
        ("tail", po::value<size_t>(&tail), "Number of elements to fetch from end of timeseries")
    ;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(PREFIX.size() > 0){
        std::cout << "Empty prefix not supported" << std::endl;
        return 1;
    }

    std::shared_ptr<Object_Store> backend(new FS_Store(DIRECTORY, 5, 1));

    Server server(backend, PREFIX);

    if(append && (data.size() != 0)){
        if(timestamp == std::numeric_limits<uint64_t>::min()){
            //No timestamp provided?
            server.append(timeseries, data);
        }
        else{
            server.append(timeseries, std::chrono::milliseconds(timestamp), data);
        }
        return 0;
    }

    std::string result;

    if(query_time){
        if(start_time < end_time){
            result = server.intervalt(timeseries, std::chrono::milliseconds(start_time), std::chrono::milliseconds(end_time));
        }
        else{
            std::cout << desc << std::endl;
            return 1;
        }
    }
    else if(query_index){
        std::cout << "Index Queries currently unsupported" << std::endl;
        return 1;
        /*
        if(start_index < end_index){

        }
        else{

        }
        */
    }
    else if(tail > 0){
        result = server.lastn(timeseries, tail);
    }
    else if(all){
        result = server.all(timeseries);
    }
    else{
        std::cout << desc << std::endl;
        return 1;
    }

    std::cout << result << std::endl;

    return 0;
}
