#include "metadata.h"
#include "archive.h"

Metadata::Metadata(){

}

Metadata::Metadata(const Ref &data_ref, std::shared_ptr<Object_Store> backend, const size_t &index, const size_t &offset){
    Archive partial(backend->fetch_from(data_ref, offset).data());
    last_timestamp = partial.current_time();

    for(;;){
        try{
            partial.next_record();
        }
        catch(E_END_OF_ARCHIVE e){
            break;
        }
    }

    last_timestamp = partial.current_time();
    num_elements = index + partial.current_index() + 1;
    size = offset + partial.size();
}
