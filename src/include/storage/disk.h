#include "straw.h"
namespace io{

/*
 * getBlock
 * Get the blkid_th block of fd,
 * copy it to _Buffer.
 * return the number of bytes read.
*/
size_t
read(const char* fname,
    BlockId blkid,
    void* _Buffer)
;

size_t
write(const char* fname,
    BlockId blkid,
    void* _Buffer)
;

}
