#include "straw.h"
#include <string>

namespace block{

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
;


// 写策略。我们用的是换出时写回法。
size_t
writeBlock(std::string fname,
    BlockId blkid,
    char* _Buffer)
;

} // end namespace block
