/*
 * @Author: Turtle (qq769711153@hotmail.com)
 * @Date: 2021-12-16 15:51:45
 * @Last Modified by: Turtle (qq769711153@hotmail.com)
 * @Last Modified time: 2021-12-17 18:34:39
 */
#pragma once

#include <algorithm>
#include "bp.hpp"

// Implementation for leaf node
template <typename _Key, typename _Record>
class bp_inter
: public bp_node<_Key, _Record> {

    using Node = bp_node<_Key, _Record>;
    using Internal = bp_inter<_Key, _Record>;

   public:
    bp_inter() {}  // constructor
    bp_inter(_Key* key_arr,Node* ptr_arr[],int size):m_size(size){
        LOG("create inter(",get_nick(this),") by arr -> <",key_arr[0],
            "> -> <",get_nick(ptr_arr[0]),",",get_nick(ptr_arr[1]),">\n");
		std::copy(key_arr,key_arr+size-1,m_keys);
		std::copy(ptr_arr,ptr_arr+size,m_ptrs);
        m_keys[INTER_MAXN-1] = key_arr[INTER_MAXN-1];
    }
    bool is_leaf() const { return false; }
    bool is_full() const { return (m_size == INTER_MAXN); }
    int size() const { return m_size; }
    int max_size() const {return INTER_MAXN;}
    int min_size() const {return INTER_MINN;}
    _Key* keys() { return m_keys; }
    Node* get_node_ptr(int pos) const { return m_ptrs[pos]; }
    Node** ptrs() { return m_ptrs; }

    Node* remove(int pos) {
        std::rotate(m_keys+pos,m_keys+pos+1,m_keys+m_size);
        std::rotate(m_ptrs+pos,m_ptrs+pos+1,m_ptrs+m_size);
        m_size--;
        return this;
    }
    Node* remove_l(int pos) {
        //this->remove(pos);
        std::rotate(m_keys+pos,m_keys+pos+1,m_keys+m_size);
        std::rotate(m_ptrs+pos,m_ptrs+pos+1,m_ptrs+m_size);
        return this;
    }
    Node* remove_r(int pos) {
        std::rotate(m_keys+pos,m_keys+pos+1,m_keys+m_size);
        std::rotate(m_ptrs+pos+1,m_ptrs+pos+2,m_ptrs+m_size);
        m_size--;
        return this;
    }

    /**
     * @brief add a key-ptr pair in this bp_inter node
     * 
     * @param key the key
     * @param ptr the pointer
     * @return Internal* if need to split, return the p_new_inter_node; otherwise return this
     */
    Internal* add(_Key key, Node* ptr) {
        LOG("bp_inter(",get_nick(this),") where m_size = ",m_size,", m_keys = ",
            pretty_array(m_keys)," and m_ptrs = ",pretty_array(m_ptrs),
            " add <",key,",",get_nick(ptr),">\n");
        auto pos = std::lower_bound(m_keys, m_keys + m_size - 1, key) - m_keys;
        auto new_ptr = &ptr;
        if(!is_full()){
            for (int i = m_size - 1; i > pos; i--) {
                m_keys[i] = m_keys[i-1];
                m_ptrs[i+1] = m_ptrs[i];
            }
            m_keys[pos] = key;
            m_ptrs[pos+1] = ptr;
            m_size++;
            return this;
        }else{
            _Key take;
            m_size = INTER_MID_POS + 1;
            Internal* new_inter_node;
            _Key new_key_arr[INTER_MAXN];
            Node* new_ptr_arr[INTER_MAXN];
            if(pos == INTER_MID_POS){
                PRINT("[in inter.add(",key,",",get_nick(ptr),"): pos(",pos,") = mid]\n");
                take = key;
                // 放好这个
                *new_ptr_arr = ptr;
                // 对于key，拷走[mid_pos:]；对于ptr，拷走[mid_pos+1:]
				std::copy(m_keys+INTER_MID_POS,m_keys+INTER_MAXN-1,new_key_arr);
				std::copy(m_ptrs+INTER_MID_POS+1,m_ptrs+INTER_MAXN,new_ptr_arr+1);
            }else if(pos < INTER_MID_POS){
                PRINT("[in inter.add(",key,",",get_nick(ptr),"): pos(",pos,") < mid]\n");
                take = m_keys[INTER_MID_POS-1];
                // 对于key，拷走[mid_pos:]；对于ptr，拷走[mid_pos:]
				std::copy(m_keys+INTER_MID_POS,m_keys+INTER_MAXN-1,new_key_arr);
				std::copy(m_ptrs+INTER_MID_POS,m_ptrs+INTER_MAXN,new_ptr_arr);
                // 往后搬
                std::rotate(m_keys+pos,m_keys+INTER_MID_POS-1,m_keys+INTER_MID_POS);
                std::rotate(m_ptrs+pos+1,m_ptrs+INTER_MID_POS,m_ptrs+INTER_MID_POS+1);
                // 放好
                m_keys[pos] = key;
                m_ptrs[pos+1] = ptr;
            }else{
                PRINT("[in inter.add(",key,",",get_nick(ptr),"): pos(",pos,") > mid]\n");
                take = m_keys[INTER_MID_POS];
                // m_keys[mid_pos+1:pos] m_ptrs[mid_pos+1:pos+1]
				std::copy(m_keys+INTER_MID_POS+1,m_keys+pos,new_key_arr);
				std::copy(m_ptrs+INTER_MID_POS+1,m_ptrs+pos+1,new_ptr_arr);
                // 放好key和ptr
                new_key_arr[pos-INTER_MID_POS-1] = key;
                new_ptr_arr[pos-INTER_MID_POS] = ptr;
                // m_keys[pos:] m_ptrs[pos+1:]
				std::copy(m_keys+pos,m_keys+INTER_MAXN-1,new_key_arr+pos-INTER_MID_POS);
				std::copy(m_ptrs+pos,m_ptrs+INTER_MAXN-1,new_ptr_arr+pos-INTER_MID_POS+1);
            }
            new_key_arr[INTER_MAXN-1] = take;
            new_inter_node = new Internal(new_key_arr,new_ptr_arr,INTER_MAXN-INTER_MID_POS);
            return new_inter_node;
        }
    }
    void fast_append_r(_Key key,Internal* ptr){
        m_keys[m_size-1] = key;
        m_ptrs[m_size] = ptr;
        m_size++;
    }
    void fast_append_l(_Key key,Internal* ptr){
        m_keys[m_size-1] = key;
        m_ptrs[m_size] = ptr;
        std::rotate(m_keys,m_keys+m_size-1,m_keys+m_size);
        std::rotate(m_ptrs,m_ptrs+m_size,m_ptrs+m_size+1);
    }

    bool underflow() { return (m_size < (INTER_MAXN + 1) / 2); }

   private:
    _Key m_keys[INTER_MAXN];
    Node* m_ptrs[INTER_MAXN];
    int m_size = 0;  // current number of records_
};
