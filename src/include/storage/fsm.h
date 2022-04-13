#ifdef STRAW_DISCARD
#pragma once
// #include "storage/storage.h"
#include <cstdio>
#include <cstring>
#include <string>
#include "storage/disk.h"
#ifndef BLCKSZ
#define BLCKSZ 8192
#endif

typedef unsigned int Oid;
typedef int BlockId;

/*
 * PtrGuard<T>
 * We dont hold reference of T* _ptr.
 * only copy its value.
 * The dtor free _ptr and set it
 * null
 */
template<typename T>
class PtrGuard{
    public:
    typedef T* ptr_t;
    PtrGuard(T* (_ptr)):ptr(_ptr){}
    ~PtrGuard(){
        delete[] ptr;
        ptr = nullptr;}
    private:
    T* (ptr);
};

template<typename T>
class New{
    public:
    typedef T* ptr_t;
    New(size_t Size){
        ptr = new T[Size];
    }
    ~New(){
        delete[] ptr;
        ptr = nullptr;
    }
    operator ptr_t (){
        return ptr;
    }
    private:
    T* ptr;
};


struct FsmPageData{
    Oid oid;
    char data[0];
};

typedef FsmPageData* FsmPage;

FILE*
openFsm(Oid oid){
    char oid_string[20];
    sprintf(oid_string,"%d",oid);
    FILE* fd = fopen(strcat(oid_string,"_fsm"),"wb");
    return fd;
}

// void
// creatNewFsm(Oid oid){
//     FILE* fd = openFsm(oid);
//     fseek(fd,0,SEEK_SET);
//     // char deadbeef[20];
//     // to_char(deadbeef,0xdeadbeef);
//     uint32_t deadbeef = 0xdeadbeef;
//     fwrite(&deadbeef,sizeof(uint32_t),1,fd);
//     fseek(fd,1*BLCKSZ,SEEK_SET);
//     fwrite(&deadbeef,sizeof(uint32_t),1,fd);
//     fseek(fd,2*BLCKSZ,SEEK_SET);
//     fwrite(&deadbeef,sizeof(uint32_t),1,fd);
//     fseek(fd,3*BLCKSZ,SEEK_SET); // trunk

//     fclose(fd);
// }

size_t
readFsmBlock(Oid oid,
    BlockId blkid,
    FsmPage page)
{
    FILE* fd = openFsm(oid);


    /* fill both */
    page->oid = oid;
    size_t readed = readRawBlock(fd,blkid,page+offsetof(FsmPageData,data));

    fclose(fd);
    return readed;
}

void
freePage(FsmPage page)
{
    delete[] page->data;
}

/* 引用FSM Page内的第off项 */
#define REF_FSM_ITEM(page,off) (*((uint8_t*)(page->data)+off))
/* 一个FSM Page内项的总量 */
#define FSM_TOTAL_ITEM (BLCKSZ/sizeof(uint8_t))
/* 申请一个FSM Page的空间 */
#define NEW_FSM_PAGE(page) (page = (FsmPage)new char [sizeof(FsmPageData) + BLCKSZ])

BlockId
find_space(Oid oid,
    size_t space)
{
    FsmPage page = nullptr;
    PtrGuard guard(page);
    size_t readed;

    /* alloc space for both */
    NEW_FSM_PAGE(page);
    PtrGuard guard(page);

    for(int blkno = 0;;blkno++)
    {
        readed = readFsmBlock(oid,blkno,page);
        if(!readed)
            break;

        for(size_t i = 0;i < FSM_TOTAL_ITEM;i++){
            if(REF_FSM_ITEM(page,i)>=space){
                return blkno;
            }
        }
    }

}



void
update_space(Oid oid,
    BlockId ds_blkid,
    size_t new_space)
{
    FsmPage page = nullptr;

    /* alloc space for both */
    NEW_FSM_PAGE(page);
    PtrGuard guard(page);

    BlockId fsm_blkid = ds_blkid / (BLCKSZ/sizeof(uint8_t));
    size_t fsm_offset = ds_blkid % (BLCKSZ/sizeof(uint8_t));

    readFsmBlock(oid,fsm_blkid,page);

    REF_FSM_ITEM(page,fsm_offset) = new_space;

}

#endif