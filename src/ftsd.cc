#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <cassert>
#include <memory>
#include <thread>

#include <smpl.h>
#include <smplsocket.h>
#include <rtos/remote_store.h>

#include "local_database.h"
#include "fts_wire_protocol.pb.h"
#include "archive.h"

namespace po = boost::program_options;

std::string RTOS_UDS = "/tmp/rtos.sock";
std::string PREFIX;
std::string UNIX_DOMAIN_SOCKET = "./fts.sock";
std::shared_ptr<Local_Database> server;

void handle_channel(std::shared_ptr<smpl::Channel> client){
    for(;;){
        try{
            const std::string serialized_request = client->recv();
            sls2::Request request;
            request.ParseFromString(serialized_request);

            sls2::Response response;
            std::string query_result;

            if(request.has_append()){
                const bool append_success = server->append(request.key(),
                    std::chrono::milliseconds(request.append().millitime()),
                    request.append().data());

                if(append_success){
                    response.set_result(sls2::Response::SUCCESS);
                }
                else{
                    response.set_result(sls2::Response::FAIL);
                }
            }
            else if(request.has_query()){
                try{
                    query_result = server->query(request.key(),
                        std::chrono::milliseconds(request.query().youngest()),
                        std::chrono::milliseconds(request.query().oldest()),
                        request.query().min_index(),
                        request.query().max_index(),
                        request.query().tail_size(),
                        std::chrono::milliseconds(request.query().tail_age()));

                    if(query_result.size() > 0){
                        response.set_result(sls2::Response::BYTES_TO_FOLLOW);
                    }
                }
                catch(E_TIMESERIES_DNE e){
                    response.set_result(sls2::Response::FAIL);
                }
            }
            else{
                //unhandled message type
                break;
            }

            std::string serialized_response;
            response.SerializeToString(&serialized_response);
            client->send(serialized_response);
            if(response.result() == sls2::Response::BYTES_TO_FOLLOW){
                assert(query_result.size() > 0);
                client->send(query_result);
            }
        }
        catch(smpl::Transport_Failed t){
            break;
        }
    }
}

void handle_local_address(std::shared_ptr<smpl::Local_Address> incoming){
  for(;;){
    std::shared_ptr<smpl::Channel> new_client(incoming->listen());
    auto t = std::thread(std::bind(handle_channel, new_client));
    t.detach();
  }
}

int main(int argc, char* argv[]){
	po::options_description desc("Options");
	desc.add_options()
        ("rtos-uds", po::value<std::string>(&RTOS_UDS), "Unix Domain Socket of RTOS backend")
        ("uds", po::value<std::string>(&UNIX_DOMAIN_SOCKET), "Unix Domain Socket to listen on")
        ("prefix", po::value<std::string>(&PREFIX), "Unique database name")
    ;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    assert(PREFIX.size() > 0);

    std::shared_ptr<Object_Store> backend(new Remote_Store(std::shared_ptr<smpl::Remote_Address>(new smpl::Remote_UDS(RTOS_UDS))));

    server = std::shared_ptr<Local_Database>(new Local_Database(backend, PREFIX));

    std::shared_ptr<smpl::Local_Address> unix_domain_socket(new smpl::Local_UDS(UNIX_DOMAIN_SOCKET));
    auto unix_domain_handler = std::thread(std::bind(handle_local_address, unix_domain_socket));
    unix_domain_handler.detach();

	while(true){
		std::this_thread::sleep_for(std::chrono::seconds(3600));
	}

    return 0;
}
