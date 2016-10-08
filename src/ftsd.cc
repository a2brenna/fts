#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <rtos/fs_store.h>
#include <cassert>
#include <memory>

#include "server.h"

namespace po = boost::program_options;

std::string DIRECTORY = "./fts.db";
std::string PREFIX;

int main(int argc, char* argv[]){
	po::options_description desc("Options");
	desc.add_options()
        ("directory", po::value<std::string>(&DIRECTORY), "Backend storage directory")
        ("prefix", po::value<std::string>(&PREFIX), "Unique database name")
    ;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    assert(PREFIX.size() > 0);

    std::shared_ptr<Object_Store> backend(new FS_Store(DIRECTORY, 5, 1));

    Server server(backend, PREFIX);

    server.append("test_key2", "test_data2");

    //std::cout << server.fetch("test_key2") << std::endl;

    return 0;
}
