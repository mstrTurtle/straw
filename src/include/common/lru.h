/**********************************************
 *
 * This snippet is adopt from:
 * https://www.geeksforgeeks.org/lru-cache-implementation/
 * contributed by Satish Srinivas
 *
 ***********************************************/

// We can use stl container list as a double
// ended queue to store the cache keys, with
// the descending time of reference from front
// to back and a set container to check presence
// of a key. But to fetch the address of the key
// in the list using find(), it takes O(N) time.
// This can be optimized by storing a reference
//	 (iterator) to each key in a hash map.

#include <list>
#include <unordered_map>
#include <iostream>
#include <array>
#include <sstream>
#include <tuple>
#include <optional>

/*
 * This code is contributed by Satish Srinivas
 */
template <typename _Ty, size_t _Size>
class LRUCache {
    // store keys of cache
    std::list<_Ty> mList;

    // store references of key in cache
    std::unordered_map<_Ty, list<_Ty>::iterator> mHashmap;

   public:

    // 若未hit且已满，就需要删一个元组. 删了哪个?
    //using Ret = std::optional<_Ty>;

    // 若another_is_deleted = true：x没有hit，并换出了另一个
    // 若this_is_new_one = true：x没有hit，并添加了进来
    // 其他情况：x hit了，并只是换了换位置。
    struct Ret{
        bool another_is_deleted = false;
        _Ty who_deleted;
        bool this_is_new_one = false;
    };

    LRUCache() {}

    // 引用一次_Ty，如磁盘中的表。
    Ret refer(_Ty x) {
        Ret ret;
        // 先删掉一部分东西
        // 不在哈希里
        if ( mHashmap.find(x) == mHashmap.end() ) {
            // 若链表满了，则链表最后一个丢掉，同时哈希里也擦掉
            if (mList.size() == _Size) {
                _Ty last = mList.back(); // 哈希擦除时要用到这个元素，所以先暂存它
                mList.pop_back();
                mHashmap.erase(last);
                ret->another_is_deleted = true;
                ret->who_deleted = last;
            }
        }
        // 在哈希里找到了，则从链表里擦掉
        else {
            ret->this_is_new_one = true;
            mList.erase(mHashmap[x]);
        }

        // 再把引用加到哈希和链表头部。
        mList.push_front(x);
        mHashmap[x] = mList.begin();
        return ret;
    }

    // Function to show contents of cache
    std::string show() {
        // Iterate in the deque and print
        // all the elements in it
        std::stringstream ss;
        for (auto element: mList) ss << element << " ";
        return ss.str();
    }
};

