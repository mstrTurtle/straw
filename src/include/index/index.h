#include <string>
#include <vector>
#include "storage/storage.h"
#include "index/rtree/rtree.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace index{

struct IndexDescriptor{
    Oid oid;
    char name[20];
};


void
getAllIndexDescriptorSecure(
    IndexDescriptor arr[],
    size_t size
)
{
    
}

IndexDescriptor
getIndexDescriptor()
{

}

}