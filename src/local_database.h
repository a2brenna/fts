#ifndef __LOCAL_DATABASE_H__
#define __LOCAL_DATABASE_H__

#include <map>
#include <mutex>
#include <rtos/object_store.h>

#include "database.h"
#include "index.h"
#include "metadata.h"

class Local_Database : public Database {

public:
  // IMPORTANT
  // All of the following transactions occur with respect to the set
  // of lists stored on the Local_Database as indicated by server_connection

  /* Create a client connected to the server accepting connections
   * smpl::Remote_Address server .
   */
  Local_Database(std::shared_ptr<Object_Store> backend, const std::string &prefix);

  bool append(const std::string &key, const std::chrono::milliseconds &time,
              const std::string &data);

  std::string query(const std::string &key,
        const std::chrono::milliseconds &youngest, const std::chrono::milliseconds &oldest,
        const size_t &min_index, const size_t &max_index,
        const size_t &tail_size, const std::chrono::milliseconds &tail_age);

  /* Returns the Index associated with a key
   */
  Index index(const std::string &key);

  /* Returns a human readable string detailing the Local_Database's current metadata
   * and internal state
   */
  virtual std::string str() const;

private:
  std::shared_ptr<Object_Store> _backend;
  std::string _prefix;

  size_t _index_resolution = 10;

  std::map<std::string, std::shared_ptr<Metadata>> _metadata;
  mutable std::mutex _metadata_lock;

  std::shared_ptr<Metadata> _unsafe_get_metadata(const std::string &key);
  std::shared_ptr<Metadata> _get_metadata(const std::string &key);
  std::shared_ptr<Metadata> _get_or_create_metadata(const std::string &key);

  Ref _ts_ref(const std::string &key) const;
  Ref _index_ref(const std::string &key) const;

};

#endif
