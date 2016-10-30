#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <cassert>
#include <memory>
#include <thread>
#include <smplsocket.h>

#include "remote_database.h"
#include "archive.h"

namespace po = boost::program_options;

std::string UNIX_DOMAIN_SOCKET;

int main(int argc, char* argv[]){

	po::options_description desc("Options");
	desc.add_options()
        ("uds", po::value<std::string>(&UNIX_DOMAIN_SOCKET), "Server Unix Domain Socket")
    ;

	po::variables_map vm;
    try{
        po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch(...){
        std::cout << "Error parsing arguments" << std::endl;
        std::cout << desc << std::endl;
        return -1;
    }
    po::notify(vm);

    assert(UNIX_DOMAIN_SOCKET.size() > 0);

    std::shared_ptr<Database> server(new Remote_Database(std::shared_ptr<smpl::Remote_Address>(new smpl::Remote_UDS(UNIX_DOMAIN_SOCKET))));

    /*
    result = server->query(timeseries,
            youngest,
            oldest,
            min_index,
            max_index,
            tail_size,
            tail_age
            );
    */

    return 0;
}
