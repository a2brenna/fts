#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <rtos/fs_store.h>
#include <cassert>
#include <memory>
#include <thread>

#include "server.h"
#include "archive.h"

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

    for(int i = 0; i < 100; i++){
        assert(server.append("test_key", std::to_string(i)));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Archive a(server.all("test_key"));
    for(;;){
        try{
            std::cout << a.current_index() << " ";
            std::cout << a.current_time().time_since_epoch().count() << " ";
            std::cout << a.current_data() << std::endl;;

            a.next_record();
        }
        catch(E_END_OF_ARCHIVE e){
            break;
        }
    }

    return 0;
}
