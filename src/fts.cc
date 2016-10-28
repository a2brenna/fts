#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <cassert>
#include <memory>
#include <thread>
#include <smplsocket.h>

#include "remote_database.h"

namespace po = boost::program_options;

std::string UNIX_DOMAIN_SOCKET;

int main(int argc, char* argv[]){
    std::string timeseries;

    bool append = false;
    std::string data;

    std::chrono::milliseconds timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    std::chrono::milliseconds youngest = std::chrono::milliseconds::min();
    std::chrono::milliseconds oldest = std::chrono::milliseconds::max();
    uint64_t min_index = 0;
    uint64_t max_index = std::numeric_limits<size_t>::max();
    uint64_t tail_size = 0;
    std::chrono::milliseconds tail_age = std::chrono::milliseconds::max();

    int64_t arg_timestamp = timestamp.count();
    int64_t arg_youngest = youngest.count();
    int64_t arg_oldest = oldest.count();
    int64_t arg_tail_age = tail_age.count();

	po::options_description desc("Options");
	desc.add_options()
        ("uds", po::value<std::string>(&UNIX_DOMAIN_SOCKET), "Server Unix Domain Socket")
        ("timeseries", po::value<std::string>(&timeseries), "Timeseries to operate on/query")
        ("append", po::bool_switch(&append), "Append data to timeseries")
        ("data", po::value<std::string>(&data), "Object to append")
        ("timestamp", po::value<int64_t>(&arg_timestamp), "Timestamp to append data at")
        ("youngest", po::value<int64_t>(&arg_youngest), "Start time of query window")
        ("oldest", po::value<int64_t>(&arg_oldest), "End time of query window")
        ("min_index", po::value<uint64_t>(&min_index), "Start index of query window")
        ("max_index", po::value<uint64_t>(&max_index), "End index of query window")
        ("tail_size", po::value<uint64_t>(&tail_size), "Number of elements to fetch from end of timeseries")
        ("tail_age", po::value<int64_t>(&arg_tail_age), "Age of elements to fetch from end of timeseries")
    ;

    timestamp = std::chrono::milliseconds(arg_timestamp);
    youngest = std::chrono::milliseconds(arg_youngest);
    oldest = std::chrono::milliseconds(arg_oldest);
    tail_age = std::chrono::milliseconds(arg_tail_age);

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    assert(UNIX_DOMAIN_SOCKET.size() > 0);
    if (timeseries.size() == 0){
        std::cout << desc << std::endl;
        return 0;
    }

    std::shared_ptr<Database> server(new Remote_Database(std::shared_ptr<smpl::Remote_Address>(new smpl::Remote_UDS(UNIX_DOMAIN_SOCKET))));

    if(append && (data.size() != 0)){
        server->append(timeseries, std::chrono::milliseconds(timestamp), data);
        return 0;
    }

    std::string result;

    result = server->query(timeseries,
            std::chrono::milliseconds(youngest),
            std::chrono::milliseconds(oldest),
            min_index,
            max_index,
            tail_size,
            std::chrono::milliseconds(tail_age)
            );

    std::cout << result << std::endl;

    return 0;
}
