#include <cassert>
#include <string>
#include <fstream>
#include <memory>
#include <map>

#include "utils/violent/violent.h"

namespace parser {

    /*****************************************************************
     *
     * Forward Declaration
     *
     *****************************************************************/
    void brand_new_xml();
    void match_header();
    void match_tag();
    void match_content();

    void TAG();
    void ATTR();
    char ESCAPE();

    void next();
    void yield(bool);


    /*****************************************************************
     *
     * Global Variables
     *
     *****************************************************************/
    std::shared_ptr<std::ifstream>  p_ifs;
    char                            cur_char    = EOF;
    callback_t                      callback    = nullptr;
    InfoSaver&                      Info = InfoSaver::getInstance();


    /*****************************************************************
     *
     * \name            init
     * \params          path
     * \description     初始化
     * 
     * You know what，它这个重载等于号，是接收右边的常引用（因为是个亡右值）后，
     * 拷贝构造一个新sharedptr,然后调用swap方法跟那个新sharedptr交换。
     * 所以左边那个sharedptr相当于被释放了一次。
     *****************************************************************/
    void init(const char* path, callback_t callback) {
        static bool inited = false ;
        assert(!inited);

        p_ifs = std::shared_ptr<std::ifstream>(new std::ifstream(path));
        parser::callback = callback;

        next();

        inited = true;
    }

    /*****************************************************************
     *
     * \name            execute
     * 
     * 真正执行。传入一个回调函数。
     *
     *****************************************************************/
    void execute() {
        brand_new_xml();
    }

    char cur() {
        return cur_char;
    }

    /*****************************************************************
     *
     * 读取下一个非换行符，并处理行号和offset
     *
     *****************************************************************/
    void next() {
        auto step = [&]() {
            cur_char = p_ifs->get();
        };

        step();
        Info.offset += 1;

        while ('\r' == cur_char || '\n' == cur_char) {
            if ('\n' == cur_char) {
                Info.lineno += 1;
                Info.offset = 0;
            }
            step();
        }
    }

    /*****************************************************************
     *
     * 断言并转至下一个字符
     *
     *****************************************************************/
    void match(char c) {
        if (c == cur()) {
            next();
            return;
        }
        else {
            assert(false);
        }
    }

    /*****************************************************************
     *
     * 一个单元已识别完成，调用callback
     *
     *****************************************************************/
    void yield(bool term = false) {

        if (!term) {
            callback(Info);
            Info.tag.clear();
            Info.attrs.clear();
        }
        else {
            // do nothing since we adapt callback rather than setjump/longjmp
        }
    }

    /*********************************************************************
     *
     * There are only five escape characters:
     *
     * "   &quot;
     * '   &apos;
     * <   &lt;
     * >   &gt;
     * &   &amp;
     * 
     * Escaping characters depends on where the special character is used.
     *
     * The examples can be validated at the W3C Markup Validation Service.
     * (https://validator.w3.org/#validate_by_input)
     *
     *********************************************************************/
    char ESCAPE() {
        match('&');
        if ('a' == cur()) {
            match('a');
            if ('m' == cur()) {
                match('m');
                match('p');
                match(';');
                return '&';
            }
            else if ('p' == cur()) {
                match('p');
                match('o');
                match('s');
                match(';');
                return '\'';
            }
            else {
                assert(false);
                return EOF;
            }
        }
        else if ('g' == cur()) {
            match('g');
            match('t');
            match(';');
            return '>';
        }
        else if ('l' == cur()) {
            match('l');
            match('t');
            match(';');
            return '<';
        }
        else if ('q' == cur()) {
            match('q');
            match('u');
            match('o');
            match('t');
            match(';');
            return '"';
        }
        else {
            assert(false);
            return EOF;
        }
    }

    void white_space() {
        while (cur() && isblank(cur())) next();
    }

    void ATTR() {
        white_space();
        std::string name;
        std::string value;
        while (isalnum(cur())) {
            name += cur();
            next();
        }
        white_space();
        match('=');
        white_space();
        match('"');
        while ('"' != cur()) { // 反正，xml总不会有双引号
            if ('&' == cur()) {
                value += ESCAPE();
            }
            else {
                value += cur();
                next();
            }
        }
        match('"');
        Info.attrs[name] = value;
    }

    void TAG() {
        white_space();
        std::string tag;
        while (isalnum(cur())) {
            tag += cur();
            next();
        }
        Info.tag = tag;
    }


    void match_header() {
        match('<');
        match('?');
        TAG();
        white_space();
        while ('?' != cur()) {
            ATTR();
        }
        match('?');
        match('>');
        Info.type = type::header;
    }

    void match_tag() {
        match('<');
        white_space();
        if (isalpha(cur())) { // 这是个平凡的TAG，而且暂时不清楚它是闭合的还是开放的
            TAG();
            white_space();
            while (isalpha(cur())) {
                ATTR();
                white_space();
            }
            if ('/' == cur()) {
                match('/');
                match('>');
                Info.type = type::closed;
                return;
            }
            else if ('>' == cur()) {
                match('>');
                Info.type = type::left;
                return;
            }
            else [[unlikely]] {
                Info.type = type::unkown;
                goto error;
            }
        }
        else if ('/' == cur()) {
            match('/');
            TAG();
            white_space();
            match('>');
            Info.type = type::right;
            return;
        }
        else [[unlikely]] {
            Info.type = type::unkown;
            goto error;
        }

    error:
        return;
    }

    void match_content() {
        while ('<' != cur()) {
            if ('&' == cur()) {
                Info.tag += ESCAPE();
            }
            else {
                Info.tag += cur();
            }
            next();
        }
        Info.type = type::content;
    }

    void brand_new_xml() {
        white_space();
        match_header();
        yield();
        white_space();

        while (EOF != cur()) {
            if ('<' == cur()) {
                match_tag();
                }
            else {
                match_content();
            }
            yield();
            white_space();

        }
        yield(true);
    }

}