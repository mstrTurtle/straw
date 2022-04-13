#include <cstdint>
uint32_t HASH_SEED = 0xdeadbeef;
uint32_t HASH_SEED2 = 0xbaadf00d;
uint32_t HASH_SEED3 = 0xdeadc0de;
uint32_t HASH_SEED4 = 0xdead10cc;

constexpr int inv_sqroot(int i){
    return 0x5f3759df - (i>>1);
}

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint8 bits8;			/* >= 8 bits */
typedef int32_t int32;
typedef uint32 CommandId;
typedef unsigned int Oid;
typedef uint32 TransactionId;
typedef int BlockId;

#ifndef BLCKSZ
#define BLCKSZ 8192
#endif