/*
 * @Author: Turtle (qq769711153@hotmail.com) 
 * @Date: 2021-12-16 15:51:49 
 * @Last Modified by: Turtle (qq769711153@hotmail.com)
 * @Last Modified time: 2021-12-17 18:55:28
 */
#pragma once

#include "bp_node.hpp"
#include <algorithm>

template <typename _Key, typename _Record>
class bp_tree {
    using bp_node_t = bp_node<_Key, _Record>;
    using bp_inter_t = bp_inter<_Key, _Record>;
    using bp_leaf_t = bp_leaf<_Key, _Record>;

   public:
    bp_tree() = default;

    void insert(const _Key& key, const _Record& record) {
        //LOG("insert\n");
        if(root == nullptr){
            LOG("add root\n");
            root = new bp_leaf_t(&key,&record,1);
        }else{
            LOG("root(",get_nick(root),") call insert__ for help\n");
            bp_node_t* ret = insert__(root, key, record);
            if(ret!=root && ret->is_leaf()){
                bp_leaf_t* ret_casted = ((bp_leaf_t*)ret);
                _Key new_keys[]{ret_casted->keys()[0]};
                bp_node_t* new_ptrs[]{root,ret};
                root = new bp_inter_t(new_keys,new_ptrs,2);
            }else if(ret!=root && !ret->is_leaf()){
                bp_inter_t* ret_casted = ((bp_inter_t*)ret);
                _Key new_keys[]{ret_casted->keys()[INTER_MAXN-1]};
                bp_node_t* new_ptrs[]{root,ret};
                root = new bp_inter_t(new_keys,new_ptrs,2);
            }
        }
        size_++;
        return;
    }

    _Record remove(const _Key& key) {
        _Record ret = find__(root, key);
        if (ret == nullptr) return nullptr;
        LOG("removing ",key,"\n");
        LOG("call remove__ ->");
        if (remove__(root, key) && (root->size() == 1))  // Collapse root
            if (!root->is_leaf()) root = ((bp_inter_t*)root)->get_node_ptr(0);
        size_--;

        return ret;
    }


    _Record find(const _Key& key) const { return find__(root, key); }

    int size() { return size_; }

   private:
    _Record find__(bp_node_t* p_root, const _Key& key) const {
        int pos;
        if (p_root->is_leaf()) {
            pos = find_key_lower(p_root->keys(), p_root->size(), key);
            return ((((bp_leaf_t*)p_root)->keys())[pos] == key)
                       ? ((bp_leaf_t*)p_root)->records(pos)
                       : nullptr;
        }
        pos = find_key_lower(p_root->keys(), p_root->size() - 1, key);
        if(p_root->keys()[pos]==key) pos++;
        return find__(((bp_inter_t*)p_root)->get_node_ptr(pos), key);
    }

    bp_node_t* insert__(bp_node_t* p_root, const _Key& key, const _Record& record) {
        LOG("in insert__(",get_nick(p_root),",",key,/*",",record,*/") -> ");
        if (p_root->is_leaf()){
            PRINT("(",get_nick(p_root),")","is leaf where keys_ = ",pretty_array(((bp_leaf_t*)p_root)->keys_,p_root->size()),/*" and records_ = ",pretty_array(((bp_leaf_t*)p_root)->records_,p_root->size()),*/" -> ");
            bp_node_t* ret = ((bp_leaf_t*)p_root)->add(key, record);
            PRINT(pretty_array(((bp_leaf_t*)p_root)->keys_,p_root->size())," -> ");
            PRINT("return (",get_nick(ret),") where keys_ = ",pretty_array(ret->keys(),ret->size()),"\n");
            return ret;
        }
        int pos = find_key_lower(p_root->keys(),p_root->size() - 1,key);
        if ((p_root->keys())[pos] == key) throw "key duplicated";
        bp_node_t* p_sub_root = (((bp_inter_t*)p_root)->get_node_ptr(pos));
        PRINT("(",get_nick(p_root),")","is not leaf, pos = ",pos,"\n");
        bp_node_t* ret = insert__(p_sub_root, key, record);
        if (ret != p_sub_root) {
            if(ret->is_leaf()){
                return ((bp_inter_t*)p_root)->add(ret->keys()[0],ret);
            }else{
                return ((bp_inter_t*)p_root)->add(ret->keys()[INTER_MAXN-1],ret);
            }
        } else {
            return p_root;
        }
    }

    /**
     * @brief Delete record of given value
     * 
     * @param p_root the tree
     * @param key the key
     * @return return true if root underflow
     */
    bp_node_t* remove__(bp_node_t* p_root, const _Key& key){
        PRINT("remove__(",get_nick(p_root),",",key,") -> ");
        using bp_node_t = bp_node<_Key, _Record>;
        using bp_inter_t = bp_inter<_Key, _Record>;
        using bp_leaf_t = bp_leaf<_Key, _Record>;
        int pos = find_key_lower(p_root->keys(), p_root->size() - 1, key);
        if (p_root->is_leaf()){
            PRINT("is leaf\n");
            return (((bp_leaf_t*)p_root)->keys()[pos] == key)
                ? (((bp_leaf_t*)p_root)->remove(pos))
                : nullptr;
        }
        bp_node_t* p_sub_root = ((bp_inter_t*)p_root)->get_node_ptr(pos);
        bp_node_t* ret = remove__(p_sub_root, key);
        if(!ret) return nullptr;
        if(!((bp_inter_t*)p_sub_root)->underflow()) return nullptr;
        // handle merge
        /*
         * ???????????????
         * ??????????????????
         * ??????????????????
         * ????????????????????????
         */
        bp_inter_t* p_root_casted = ((bp_inter_t*)p_root);
        _Key* cur_keys = ((bp_inter_t*)p_root)->keys();
        bp_node_t** cur_ptrs = ((bp_inter_t*)p_root)->ptrs();
        // 1. ??????
        if(p_sub_root->is_leaf()){
            bp_leaf_t* p_sub_root_casted = (bp_leaf_t*)p_sub_root;
            bp_leaf_t* p_good_friend = (bp_leaf_t*)(cur_ptrs[((pos==0)?1:(pos-1))]);
            // 1.1 ?????????????????????
            if((p_good_friend->size()) - 1 > p_sub_root->min_size()){
                // 1.1.1 ??????????????????
                if(pos==0){
                    // ??????????????????key??????
                    cur_keys[pos] = p_good_friend->keys()[1];
                    // ?????????
                    p_sub_root_casted->add(p_good_friend->keys()[0],p_good_friend->p_records()[0]);
                    // ????????????
                    p_good_friend->remove(0);
                // 1.1.2 ?????????????????????
                }else{
                    bp_leaf_t* p_good_friend = (bp_leaf_t*)cur_ptrs[pos-1];
                    // ??????????????????key??????
                    cur_keys[pos] = p_good_friend->keys()[p_good_friend->size()-1];
                    // ?????????
                    p_sub_root_casted->add(p_good_friend->keys()[p_good_friend->size()-1],p_good_friend->p_records()[p_good_friend->size()-1]);
                    // ????????????
                    p_good_friend->remove(p_good_friend->size()-1);
                }
            // 1.2 ???????????????
            }else{
                // 1.2.1 ??????????????????
                if(pos==0){
                    // ??????????????????key????????????????????????
                    p_root_casted->remove_r(pos);
                    // ???????????????????????????
                    std::copy(p_good_friend->keys(),p_good_friend->keys()+p_good_friend->size(),p_sub_root_casted->keys()+p_sub_root_casted->size());
                    std::copy(p_good_friend->p_records(),p_good_friend->p_records()+p_good_friend->size(),p_sub_root_casted->p_records()+p_sub_root_casted->size());
                // 1.2.2 ?????????????????????
                }else{
                    // ??????????????????key??????????????????(?????????sub_root??????)??????
                    p_root_casted->remove_r(pos);
                    // ????????????????????????????????????
                    std::copy(p_sub_root_casted->keys(),p_sub_root_casted->keys()+p_sub_root_casted->size(),p_good_friend->keys()+p_good_friend->size());
                    std::copy(p_sub_root_casted->p_records(),p_sub_root_casted->p_records()+p_sub_root_casted->size(),p_good_friend->p_records()+p_good_friend->size());
                }
            }
        // 2. ?????????
        }else{
            bp_inter_t* p_sub_root_casted = (bp_inter_t*)p_sub_root;
            bp_inter_t* p_good_friend = (bp_inter_t*)cur_ptrs[((pos==0)?1:(pos-1))];
            // 2.1 ????????????????????????
            if((p_good_friend->size()) - 1 > p_sub_root->min_size()){
                // 2.1.1 ???????????????????????????
                if(pos==0){
                    // ???key??????
                    p_sub_root_casted->fast_append_r(cur_keys[pos],(bp_inter_t*)p_good_friend->ptrs()[0]);
                    // ????????????????????????????????????????????????
                    cur_keys[pos] = p_good_friend->keys()[0];
                    // ????????????
                    p_good_friend->remove_l(0);
                // 2.1.2 ??????????????????????????????
                }else{
                    // ???key??????
                    p_sub_root_casted->fast_append_l(cur_keys[pos],(bp_inter_t*)p_good_friend->ptrs()[size_-1]);
                    // ????????????????????????????????????????????????
                    cur_keys[pos] = p_good_friend->keys()[size_-2];
                    // ????????????
                    p_good_friend->remove_r(size_-2);
                }

            // 2.2 ????????????????????????????????????
            }else{
                // 2.2.1 ??????????????????
                if(pos==0){
                    // ????????????????????????????????????
                    p_sub_root_casted->fast_append_r(cur_keys[pos],(bp_inter_t*)p_good_friend->ptrs()[0]);
                    // ?????????key????????????????????????
                    std::copy(p_good_friend->keys(),p_good_friend->keys()+p_good_friend->size()-1,p_sub_root_casted->keys()+size_-1);
                    std::copy(p_good_friend->ptrs()+1,p_good_friend->ptrs()+p_good_friend->size(),p_sub_root_casted->ptrs()+size_);
                // 2.2.2 ?????????????????????
                }else{
                    // ????????????????????????????????????
                    p_good_friend->fast_append_r(cur_keys[pos],(bp_inter_t*)p_sub_root_casted->ptrs()[0]);
                    // ?????????key????????????????????????
                    std::copy(p_sub_root_casted->keys(),p_sub_root_casted->keys()+p_sub_root_casted->size()-1,p_good_friend->keys()+size_-1);
                    std::copy(p_sub_root_casted->ptrs()+1,p_sub_root_casted->ptrs()+p_sub_root_casted->size(),p_good_friend->ptrs()+size_);
                }
            }
        }

        return p_root;
    }

    /**
     * @brief find key position using binary search
     * 
     * @param keys begin of key_arr
     * @param cnt length of key_arr
     * @param key the key to search
     * @return int the first position where key greater equal than it
     */
    int find_key_lower(_Key keys[], int cnt, const _Key key) const {
        return std::lower_bound(keys,keys+cnt,key) - keys;
    }

    bp_node_t* root = nullptr;
    int size_ = 0;
};