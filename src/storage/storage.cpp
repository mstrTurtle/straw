#include "storage/storage.h"

struct MetaInfo{

};

struct MetaPage{
        MetaInfo metaInfo;
        char datum[];
};


struct HeapTuplePointer{
    int blkNo;
    int no;
};// (“指”向node节点)

struct node{
    int offset;
};

/*
genTuple :: Tuple -> Location
insertTuple :: Tuple -> Location
getTuple :: Location -> Tuple
deleteTuple :: Location -> Void
getMetaInfo :: MetaInfo
*/

