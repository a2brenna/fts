#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <string>
#include <chrono>
#include <memory>

class E_TIMESERIES_DNE {};
class E_ORDER {};

class Database {

public:
  // IMPORTANT
  // All of the following transactions occur with respect to the set
  // of lists stored in a particular Database

  /* Append data to the list indicated by key at some time (in milliseconds)
  * between the time this function is called and the time it returns.
  *
  * key: Any std::string such that key.empty() = false
  * data: Any string
  */
  virtual bool append(const std::string &key, const std::string &data);

  /* Append data to the list indicated by key at time
  *
  * key: Any std::string such that key.empty() = false
  * time: Milliseconds since epoch > any previous time
  * data: Any string
  */
  virtual bool append(const std::string &key, const std::chrono::milliseconds &time,
              const std::string &data) = 0;

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
  virtual std::string lastn(const std::string &key, const unsigned long long &num_entries);

  /* Returns a vector contains all the entries in the list for key in
  * chronological order.  If the list is empty or no list exists, a vector
  * of size 0 is returned.
  *
  * key: Any std::string such that key.empty() = false
  */
  virtual std::string all(const std::string &key);

  /* Returns a vector of all the values in the list for key between time
  * index start and end, inclusive. start and end must be in milliseconds since
  * epoch.
  *
  * key: Any std::string such that key.empty() = false
  * start: 0 <= end
  * end: 0 >= start
  */
  virtual std::string intervalt(const std::string &key, const std::chrono::milliseconds &start,
            const std::chrono::milliseconds &end);

  virtual std::string query(const std::string &key,
            const std::chrono::milliseconds &youngest, const std::chrono::milliseconds &oldest,
            const size_t &min_index, const size_t &max_index,
            const size_t &tail_size, const std::chrono::milliseconds &tail_age) = 0;


  /* Returns a human readable string detailing the Database's current metadata
   * and internal state
   */
  virtual std::string str() const;

};

#endif
