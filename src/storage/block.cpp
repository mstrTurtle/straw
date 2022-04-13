#include "common/lru.h"
#include "storage/storage.h"
#include <string>
#include <tuple>

#include "storage/block.h"

namespace block{

using BlockIndex = std::pair<std::string,BlockId>;
constexpr size_t LRU_SIZE = 100;

LRUCache<BlockIndex,LRU_SIZE> page_cache;

std::unordered_map<BlockIndex, char*> page_saver;

/*
 * 援引一下：
 * 若another_is_deleted = true：x没有hit，并换出了另一个(换入，换出)
 * 若this_is_new_one = true：x没有hit，并添加了进来（换入，不换出）
 * 其他情况：x hit了，并只是换了换位置。（不换入，不换出）
 */

// 读策略。
char*
readBlock(std::string fname,
    BlockId blkid)
{
    char* retval;
    char* buffer;
    BlockIndex index{fname,blkid};
    auto ret = page_cache.refer(index);
    // 1. “x换入，没东西换出” 和 “x换入，并换出了另一个” 的公共过程（换入）
    if (ret.this_is_new_one||ret.another_is_deleted) {
        // 1.1 分配内存，换入
        buffer = new char[sizeof(char)*BLCKSZ];
        io::read(fname.c_str(),blkid,buffer);
        page_saver[index] = buffer;
        // 1.2 拷贝并读出
        memcpy(retval,page_saver[index],BLCKSZ);
        // 1.3 “x换入，并换出了另一个” 的特有过程（写回）
        if (ret.another_is_deleted) {
            // 写回
            char* buffer = page_saver[ret.who_deleted];
            auto [fname,blkid] = ret.who_deleted;
            io::write(fname.c_str(),blkid,buffer);
            // 回收内存，擦掉引用
            delete[] page_saver[ret.who_deleted];
            page_saver.erase(ret.who_deleted);
        }
    }
    // 2. x hit了，并只是换了换位置。
    else {
        // 拷贝并读出
        memcpy(retval,page_saver[index],BLCKSZ);
    }
    return buffer;
}


// 写策略。我们用的是换出时写回法。
size_t
writeBlock(std::string fname,
    BlockId blkid,
    char* _Buffer)
{
    BlockIndex index{fname,blkid};
    auto ret = page_cache.refer(index);
    // 1. “x换入，没东西换出” 和 “x换入，并换出了另一个” 的公共过程（换入）
    if (ret.this_is_new_one || ret.another_is_deleted) {
        // 1.1 申请内存并注册
        char* buffer;
        buffer = new char[sizeof(char)*BLCKSZ];
        page_saver[index] = buffer;
        memcpy(buffer,_Buffer,BLCKSZ);
        // 1.2 “x换入，并换出了另一个” 的特有过程（写回）
        if (ret.another_is_deleted) {
            // 1.2.1 写回
            char* buffer = page_saver[ret.who_deleted];
            auto [fname,blkid] = ret.who_deleted;
            io::write(fname.c_str(),blkid,buffer);
            // 1.2.2 回收内存，擦掉引用
            delete[] page_saver[ret.who_deleted];
            page_saver.erase(ret.who_deleted);
        }
    }
    // 2. x hit了，并只是换了换位置。
    else {
        // 2.1 直接改page_saver中的东西
        memcpy(page_saver[index],_Buffer,BLCKSZ);
    }

}

} // end namespace block
