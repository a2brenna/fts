#ifndef __SLS_CLIENT_H__
#define __SLS_CLIENT_H__

#include <smpl.h>
#include <vector>
#include <memory>

#include "archive.h"
#include "sls.h"
#include "sls.pb.h"

namespace sls {

extern std::shared_ptr<smpl::Remote_Address> global_server;

class Client {

public:
  // IMPORTANT
  // All of the following transactions occur with respect to the set
  // of lists stored on the Server as indicated by server_connection

  /* Create a client connected to the server accepting connections
   * smpl::Remote_Address server .
   */
  Client(std::shared_ptr<smpl::Remote_Address> server);

  /* Append data to the list indicated by key at some time (in milliseconds)
  * between the time this function is called and the time it returns.
  *
  * key: Any std::string such that key.empty() = false
  * data: Any string
  */
  bool append(const std::string &key, const std::string &data);

  /* Append data to the list indicated by key at time
  *
  * key: Any std::string such that key.empty() = false
  * time: Milliseconds since epoch > any previous time
  * data: Any string
  */
  bool append(const std::string &key, const std::chrono::milliseconds &time,
              const std::string &data);

  /* Append entire archive to the list indicated by key
  *
  * key: Any std::string such that key.empty() = false
  * archive: An Archive file, such that archive.head_time() >= the timestamp
  * of the current last element of the timeseries associated with key.
  */
  bool append_archive(const std::string &key, const Archive &archive);

  /* Returns a vector of at most the N most recent entries in the list for key
  * in chronological order.  If the list for key contains less than
  * num_entries, the entire list is returned. If the list for key is empty,
  * or num_entries is 0, a vector of size 0 is returned.
  *
  * key: Any std::string such that key.empty() = false
  * num_entries: >= 0
  */
  Archive lastn(const std::string &key, const unsigned long long &num_entries);

  /* Returns a vector contains all the entries in the list for key in
  * chronological order.  If the list is empty or no list exists, a vector
  * of size 0 is returned.
  *
  * key: Any std::string such that key.empty() = false
  */
  Archive all(const std::string &key);

  /* Returns a vector of all the values in the list for key between time
  * index start and end, inclusive. start and end must be in milliseconds since
  * epoch.
  *
  * key: Any std::string such that key.empty() = false
  * start: 0 <= end
  * end: 0 >= start
  */
  Archive intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end);
};

class Cached_Client{

private:

    Client _client;
    std::map<std::string, Archive> _cache;
    size_t _max_cache_size = 4000000000;
    size_t _current_cache_size = 0;

    bool _check();
    bool _flush();

    bool _check_key(const std::string &key);
    bool _flush_key(const std::string &key);

public:
  // IMPORTANT
  // All of the following transactions occur with respect to the set
  // of lists stored on the Server as indicated by server_connection.
  // This Cached_Client caches writes as the name suggests, for much
  // improved throughput, particularly for large import jobs.
  // The method signatures are the same as Client, and their behaviours
  // should be identical, with relaxed consistency constraints due to
  // cacheing.

  Cached_Client(std::shared_ptr<smpl::Remote_Address> server, const size_t max_cache_size);
  ~Cached_Client();

  /* Flush the cache.  If this returns true, then all data was successfully
   * written to the database. If false, then some/all of the data may have
   * failed to be written.
   */
  bool flush();

  bool append(const std::string &key, const std::string &data);

  bool append(const std::string &key, const std::chrono::milliseconds &time,
              const std::string &data);

  bool append_archive(const std::string &key, const Archive &archive);

  Archive lastn(const std::string &key, const unsigned long long &num_entries);

  Archive all(const std::string &key);

  Archive intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end);

};

}

#endif
