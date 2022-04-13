#include "storage/disk.h"
#include <cstdio>
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
{
    size_t ret;
    FILE* fd = fopen(fname,"rb");
    fseek(fd,blkid*BLCKSZ,SEEK_SET);
    ret =  fread(_Buffer,1,BLCKSZ,fd);
    fclose(fd);
    return ret;
}

size_t
write(const char* fname,
    BlockId blkid,
    void* _Buffer)
{
    size_t ret;
    FILE* fd = fopen(fname,"wb");
    fseek(fd,blkid*BLCKSZ,SEEK_SET);
    ret =  fwrite(_Buffer,1,BLCKSZ,fd);
    fclose(fd);
    return ret;
}

}
