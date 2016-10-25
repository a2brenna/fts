#ifndef __REMOTE_DATABASE_H__
#define __REMOTE_DATABASE_H__

#include <smpl.h>
#include "database.h"

class Remote_Database : public Database {

public:
  // IMPORTANT
  // All of the following transactions occur with respect to the set
  // of lists stored on the Local_Database as indicated by server_connection

  /* Create a client connected to the server accepting connections
   * smpl::Remote_Address server .
   */
  Remote_Database(std::shared_ptr<smpl::Remote_Address> server_address);

  bool append(const std::string &key, const std::chrono::milliseconds &time,
              const std::string &data);

  std::string query(const std::string &key,
        const std::chrono::milliseconds &youngest, const std::chrono::milliseconds &oldest,
        const size_t &min_index, const size_t &max_index,
        const size_t &tail_size, const std::chrono::milliseconds &tail_age);

  std::string str() const;

private:
  std::shared_ptr<smpl::Remote_Address> _server_address;
  std::shared_ptr<smpl::Channel> _server;
};

#endif
