#include "common/lru.h"

// 九敏
template <typename _Ty, size_t _Size>
LRUCache<_Ty,_Size>::Ret LRUCache<_Ty,_Size>::refer(_Ty x) {
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
template <typename _Ty, size_t _Size>
std::string LRUCache<_Ty,_Size>::show() {
    // Iterate in the deque and print
    // all the elements in it
    std::stringstream ss;
    for (auto element: mList) ss << element << " ";
    return ss.str();
}