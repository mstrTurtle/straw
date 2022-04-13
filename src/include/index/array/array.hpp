#include <cstdio>
#include <exception>
#include <tuple>
#include <cerrno>


// 不可重入、不可删除（只能插入和读取）的单线程数据存储类
template<typename Inner>
class array {
public:
    struct file_header {
        uint32_t magic_number;
        size_t total_cnt;
        size_t inner_sz;
    };
    struct record_header {
        Inner inner;
    };
    array(const char* path) {
        // 打开文件
        m_fp = nullptr; // 以防万一
        fopen_s(&m_fp, path, "rb+"); // 注意，r是只读打开，w是只写打开。r+是读写打开，w+是不管如何创建个新的空文件
        if (m_fp == nullptr) {
            auto i = errno;
            char errMsg[100];
            strerror_s(errMsg, sizeof(errMsg), errno);
            exit(1);
        }
        // 装载文件头
        fseek(m_fp, 0, SEEK_SET);
        fread(&fheader, sizeof(file_header), 1, m_fp);
        if (fheader.magic_number != 0xdeadbeef) { //检验这个是不是真的文件
            throw std::exception{ "Invalid file" };
        }
    }
    ~array() {
        fclose(m_fp);
    }
    // 初始化一个新的
    static void create(const char* path) {
        FILE* t_fp = nullptr; // 注意，这个t_fp，开完后一定要关掉
        fopen_s(&t_fp, path, "wb+");
        if (t_fp == nullptr)
            throw std::exception{ "Cannot open file" };
        /* 初始化file_header */
        file_header t_fheader{
            .magic_number = 0xdeadbeef,
            .total_cnt = 0,
            .inner_sz = sizeof(Inner)
        };
        fseek(t_fp, 0, SEEK_SET);
        fwrite(&t_fheader, sizeof(file_header), 1, t_fp);
        fclose(t_fp);
    }
    Inner& visit_inner(int cnt) {
        if(!(0<=cnt && cnt <= fheader.total_cnt)){
            throw std::exception{"Invalid Index"};
        }
        cur_pos = sizeof(file_header) + cnt * sizeof(record_header);
        fseek(m_fp, cur_pos, SEEK_SET);
        fread(&rheader, sizeof(rheader), 1, m_fp);
        return rheader.inner;
    }
    int update_inner(int cnt,Inner const& inner) {
        if(!(0<=cnt && cnt <= fheader.total_cnt)){
            throw std::exception{"Invalid Index"};
        }
        cur_pos = sizeof(file_header) + cnt * sizeof(record_header);
        fseek(m_fp, cur_pos, SEEK_SET);
        fread(&rheader, sizeof(rheader), 1, m_fp);
        rheader.inner = inner;
        fseek(m_fp, cur_pos, SEEK_SET);
        fwrite(&rheader, sizeof(rheader), 1, m_fp);
        return 0;
    }
    record_header get_cur_rheader() {
        return rheader;
    }
    void append(Inner const& inner) {
        /* 先append最后一个 */
        fseek(m_fp, 0, SEEK_END);
        int new_pos = ftell(m_fp);
        record_header t_rheader{
            .inner = inner
        };
        fwrite(&t_rheader, sizeof(record_header), 1, m_fp);
        // 更新fheader状态并写回
        fheader.total_cnt += 1;
        fseek(m_fp, 0, SEEK_SET);
        fwrite(&fheader, sizeof(fheader), 1, m_fp);
    }
private:
    FILE* m_fp;
    int cur_pos = 0;
    file_header fheader;
    record_header rheader;
};