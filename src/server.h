#ifndef __FTS_SERVER_H__
#define __FTS_SERVER_H__

#include <string>
#include <chrono>
#include <memory>
#include <map>
#include <mutex>
#include <rtos/object_store.h>

#include "metadata.h"

class E_TIMESERIES_DNE {};
class E_ORDER {};

class Server {

public:
  // IMPORTANT
  // All of the following transactions occur with respect to the set
  // of lists stored on the Server as indicated by server_connection

  /* Create a client connected to the server accepting connections
   * smpl::Remote_Address server .
   */
  Server(std::shared_ptr<Object_Store> backend, const std::string &prefix);

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
  //TODO:
  //bool append_archive(const std::string &key, const Archive &archive);

  /* Returns a vector of at most the N most recent entries in the list for key
  * in chronological order.  If the list for key contains less than
  * num_entries, the entire list is returned. If the list for key is empty,
  * or num_entries is 0, a vector of size 0 is returned.
  *
  * key: Any std::string such that key.empty() = false
  * num_entries: >= 0
  */
  std::string lastn(const std::string &key, const unsigned long long &num_entries);

  /* Returns a vector contains all the entries in the list for key in
  * chronological order.  If the list is empty or no list exists, a vector
  * of size 0 is returned.
  *
  * key: Any std::string such that key.empty() = false
  */
  std::string all(const std::string &key);

  /* Returns a vector of all the values in the list for key between time
  * index start and end, inclusive. start and end must be in milliseconds since
  * epoch.
  *
  * key: Any std::string such that key.empty() = false
  * start: 0 <= end
  * end: 0 >= start
  */
  //TODO:
  std::string intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end);

  /* Returns a human readable string detailing the Server's current metadata
   * and internal state
   */
  std::string str() const;

private:
  std::shared_ptr<Object_Store> _backend;
  std::string _prefix;

  size_t _index_resolution = 10;

  std::map<std::string, std::shared_ptr<Metadata>> _metadata;
  mutable std::mutex _metadata_lock;

  std::string _calc_address(const std::string &key) const;
  std::shared_ptr<Metadata> _unsafe_get_metadata(const std::string &key);
  std::shared_ptr<Metadata> _get_metadata(const std::string &key);
  std::shared_ptr<Metadata> _get_or_create_metadata(const std::string &key);

};

#endif
