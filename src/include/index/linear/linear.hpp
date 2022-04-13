#include <cstdio>
#include <exception>
#include <tuple>

// 不可重入、不可删除（只能插入和读取）的单线程数据存储类
template<typename Inner>
class linear{
public:
    struct file_header{
        size_t magic_number;
        size_t first_pos;
        size_t last_pos;
        size_t total_cnt;
        size_t inner_sz;
    };
    struct record_header{
        size_t prev_pos;
        size_t next_pos;
        size_t data_len;
        Inner inner;
    };
    linear(char* path){
        // 打开文件
        m_fp = nullptr; // 以防万一
        m_fp = fopen(path,"wb");
        if(m_fp == nullptr)
            throw std::exception{"Cannot open file"};
        // 装载文件头
        fseek(m_fp,0,SEEK_SET);
        fread(&fheader,sizeof(file_header),1,m_fp);
        if(fheader.magic_number != 0xdeadbeef){ //检验这个是不是真的文件
            throw std::exception{"Invalid file"};
        }
        // 我们保证，这里面至少有一个节点，所以直接装进来
        fseek(m_fp,fheader.first_pos,SEEK_SET);
        fread(&rheader,sizeof(record_header),1,m_fp);
    }
    ~linear(){
        fclose(m_fp);
        delete current_data;
    }
    // 初始化一个新的
    static void create(char* path){
        FILE* t_fp = fopen(path,"wb");
        file_header t_fheader{
            .magic_number = 0xdeadbeef,
            .first_pos = 0,
            .last_pos = sizeof(file_header),
            .total_cnt = 1,
            .inner_sz = sizeof(Inner);
        };
        fseek(t_fp,0,SEEK_SET);
        fwrite(&t_fheader,sizeof(file_header),1,t_fp);
        record_header t_rheader {
            .prev_pos = 0,
            .next_pos = 0,
            .data_len = 0,
            .inner = {}
        };
        fseek(t_fp,sizeof(file_header),SEEK_SET); // 保险起见，免得被其他线程搞了(?)
        fwrite(&t_rheader,sizeof(record_header),1,t_fp);
    }
    int next(){
        // 已到达最后端
        if(rheader.next_pos == 0){
            return 0;
        } else {
            cur_pos = rheader.next_pos;
            fseek(m_fp,cur_pos,SEEK_SET);
            fread(&rheader,sizeof(record_header),1,m_fp);
            return 1;
        }
    }
    int prev(){
        // 已到达最前端
        if(rheader.prev_pos == 0){
            return 0;
        } else {
            cur_pos = rheader.prev_pos;
            fseek(m_fp,cur_pos,SEEK_SET);
            fread(&rheader,sizeof(record_header),1,m_fp);
            return 1;
        }
    }
    record_header get_record_header(){
        return rheader;
    }
    std::pair<char*,size_t> get_cur_data(){
        size_t len = rheader.data_len;
        fseek(m_fp,cur_pos+sizeof(record_header),SEEK_SET);
        fread(current_data,len,1,m_fp);
        return {current_data, len};
    }
    void append(Inner inner, char* data,size_t len){
        size_t new_pos = fseek(m_fp,0,SEEK_END);
        record_header t_rheader {
            .prev_pos = fheader.last_pos,
            .next_pos = 0,
            .data_len = len,
            .inner = inner
        };
        fwrite(&t_rheader,sizeof(record_header),1,m_fp);
        fwrite(data,sizeof(char),len,m_fp);
        // 更新fheader状态
        fheader.last_pos = new_pos;
        fheader.total_cnt += 1;
        // 注意把fheader写回。
        fseek(m_fp,0,SEEK_SET);
        fwrite(&fheader,sizeof(fheader),1,m_fp);
    }
private:
    FILE* m_fp = nullptr;
    size_t cur_pos = 0;
    file_header fheader;
    record_header rheader;
    char current_data[8196]; // 不够再加
};