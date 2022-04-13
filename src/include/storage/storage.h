#include <cstdint>
#include <cassert>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include "storage/fsm.h"
#include "storage/disk.h"
#include "straw.h"


#define FLEXIBLE_ARRAY_MEMBER 0

/*
 * OffsetNumber:
 *
 * this is a 1-based index into the linp (ItemIdData) array in the
 * header of each disk page.
 */
typedef uint16 OffsetNumber;

struct ItemPointer
{
	BlockId ip_blkid;
	OffsetNumber ip_posid;
};

struct PhysicalTupleData
{
	bits8		t_bits[FLEXIBLE_ARRAY_MEMBER];	/* bitmap of NULLs */

	/* MORE DATA FOLLOWS AT END OF STRUCT */
};

typedef PhysicalTupleData *PhysicalTuple;

typedef struct MemoryTupleData
{
	uint32		t_len;			/* length of *t_data */
	ItemPointer t_self;		/* SelfItemPointer */
	Oid			t_tableOid;		/* table the tuple came from */

	PhysicalTuple t_data;		/* -> tuple header and data
                                   物理存储实体             */
} HeapTupleData;

#define HEAPTUPLESIZE (sizeof(HeapTupleData))


typedef HeapTupleData *Tuple;

struct DsPageData{
    Oid oid;
    BlockId blkid;
    char data[FLEXIBLE_ARRAY_MEMBER];
};

typedef DsPageData* DsPage;

/*****************************************
 * formTuple :: Void -> Tuple
 * insertTuple :: Tuple -> ItemPointer
 * getTuple :: ItemPointer -> Tuple
 * deleteTuple :: ItemPointer -> Void
 * getMetaInfo :: MetaInfo
 *****************************************/

#ifdef STRAW_DISCARD
FILE*
openDs(Oid oid){
    char oid_string[20];
    sprintf(oid_string,"%d",oid);
    FILE* fd = fopen(strcat(oid_string,"_ds"),"wb");
    return fd;
}

/* Should allocate enough space for page */
size_t
readDsBlock(Oid oid,
    BlockId blkid,
    DsPage page)
{
    FILE* fd = openFsm(oid);


    /* fill both */
    page->oid = oid;
    size_t readed = readRawBlock(fd,blkid,page+offsetof(DsPageData,data));

    fclose(fd);
    return readed;
}

/* Should allocate enough space for page */
size_t
writeDsBlock(Oid oid,
    BlockId blkid,
    DsPage page)
{
    FILE* fd = openFsm(oid);


    /* fill both */
    page->oid = oid;
    size_t readed = writeRawBlock(fd,blkid,page+offsetof(DsPageData,data));

    fclose(fd);
    return readed;
}
#endif
/*
 * heap_form_tuple
 *		construct a tuple from the given values
 */
Tuple
formTuple(Tuple	tuple,
	char *values,
    size_t data_len)
{
	assert(tuple == nullptr);

    /* determine total space for physicalTuple */
    ptrdiff_t bits_offset = offsetof(PhysicalTupleData, t_bits);
    size_t phy_len = bits_offset + data_len;

    /* allocate space, and let t_data point to the latter segment */
    tuple = (Tuple)new char(phy_len);
    tuple->t_data = (PhysicalTuple)((char*)tuple + HEAPTUPLESIZE);

    /* set header invalid */
    tuple->t_len = phy_len;
    tuple->t_self = {0, 0};
    tuple->t_tableOid = 0;

    /* set data */
    memcpy(tuple->t_data+bits_offset,values,data_len);

	return tuple;
}

void
freeTuple(Tuple	tuple)
{
    delete tuple;
}

enum class ItemStatus{
    NORMAL,
    DIRTY,
    DELETED
};

struct ItemData{
    ItemStatus status;
    size_t tuple_offset;
    size_t tuple_len;
};

typedef ItemData* Item;

struct AppendData{
    size_t head;
    size_t tail;
    size_t item_count;
};

typedef AppendData* Append;

#define REF_ITEM(page,no) ((Item)((char*)page + no * sizeof(ItemData)));
/* 申请一个FSM Page的空间 */
#define NEW_DS_PAGE(page) (page = (DsPage)new char [sizeof(DsPageData) + BLCKSZ])

#ifdef STRAW_DISCARD
namespace heapmanip{

void
storage_insert(Oid oid,
    Tuple tuple
)
{
    BlockId blkid = find_space(oid,tuple->t_len);
    FILE* fd = openDs(oid);
    DsPage page;
    NEW_DS_PAGE(page);
    PtrGuard(page);
    /* load free block as page */
    readDsBlock(oid,blkid,page);
    /* read append from page */
    Append append = (Append)((char*)page + (BLCKSZ - sizeof(AppendData)));
    /* alter correspond item */
    Item item = (Item)((char*)page+append->head);
    item->status = ItemStatus::NORMAL;
    item->tuple_len = tuple->t_len;
    item->tuple_offset = append->tail - tuple->t_len;
    /* alter append */
    append->item_count++;
    append->head += sizeof(ItemData);
    append->tail -= tuple->t_len;
    /* alter the page */
    memcpy((char*)page+append->tail-tuple->t_len,tuple->t_data,tuple->t_len);
    /* flush page into disk */
    writeRawBlock(fd,blkid,page->data);
    /* update freespace of block */
    update_space(oid,blkid,append->tail - append->tail);
}

void
storage_delete(Oid oid,
    ItemPointer ptr)
{
    FILE* fd = openDs(oid);
    DsPage page;
    NEW_DS_PAGE(page);
    PtrGuard(page);
    readDsBlock(oid,ptr.ip_blkid,page);
    Item item;
    item = REF_ITEM(page,ptr.ip_posid);
    item->status = ItemStatus::DELETED;
    writeRawBlock(fd,ptr.ip_blkid,page->data);
    fclose(fd);
    return;
}

[[nodiscard]]
Tuple
storage_get(Oid oid,
    ItemPointer ptr)
{
    FILE* fd = openDs(oid);
    DsPage page;
    NEW_DS_PAGE(page);
    readDsBlock(oid,ptr.ip_blkid,page);
    Item item;
    item = REF_ITEM(page,ptr.ip_posid);
    Tuple tuple = (Tuple)new char [sizeof(MemoryTupleData) + item->tuple_len];
    memcpy(tuple+offsetof(MemoryTupleData,t_data),(char*)page + item->tuple_offset,item->tuple_len);
    fclose(fd);
    return tuple;
}

/*
 * storage_alter
 *      only a shortcut as combination of
 *      storage_delete and storage_insert
 */
void
storage_alter(Oid oid,
    ItemPointer ptr,
    Tuple tuple)
{
    storage_delete(oid,ptr);
    storage_insert(oid,tuple);
}

}
#endif

