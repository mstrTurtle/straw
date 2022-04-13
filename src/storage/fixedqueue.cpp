
#include <list>
#include <unordered_map>
#include <iostream>
#include <array>

using namespace std;

template <typename _Ty, typename _Size>
class FixedQueue {

public:
    struct iterator{
        public:
            iterator(array<_Ty, _Size>& array, size_t offset)
            : _array(array)
              _offset(offset)
            {}

            const iterator& operator++() {_offset = (_offset + 1) % _array.size();return *this;}

            bool operator!=(const iterator& other) { return (_offset != other._offset); }

            _Ty& operator*() {
                return _array[_offset];
            }

        private:
            array<_Ty, _Size>& _array;
            size_t _offset;
    };

    struct const_iterator{
        public:
            iterator(const array<_Ty, _Size>& array, size_t offset)
            : _array(array)
              _offset(offset)
            {}

            const iterator& operator++() {_offset = (_offset + 1) % _array.size();return *this;}

            bool operator!=(const iterator& other) { return (_offset != other._offset); }

            const _Ty& operator*() {
                return _array[_offset];
            }

        private:
            const array<_Ty, _Size>& _array;
            size_t _offset;
    };

    [[nodiscard]] constexpr iterator begin() noexcept {
        return iterator(_Elems, mHead);
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept {
        return const_iterator(_Elems, mHead);
    }

    [[nodiscard]] constexpr iterator end() noexcept {
        return iterator(_Elems, mTail);
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept {
        return const_iterator(_Elems, mTail);
    }

    bool full(){
        return ((mTail+1)%_Size = mHead) ? true : false;
    }

    bool empty(){
        return (mTail == mHead) ? true : false;
    }

    void insert(_Ty _Val){
        if(!full()){
            _Elems[]
        }
    }

private:
    size_t mHead;
    size_t mTail;
    array<_Ty,_Size> _Elems;

};