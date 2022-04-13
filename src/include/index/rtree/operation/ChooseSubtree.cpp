#include "algorithm/algorithm.h"
#include "storage/block.h"

struct node{
    int level;
    CoordInterval2 box;
};

node& get_root(){
    char* page1 = block::readBlock("rtree",0);
}

void ChooseSubtree(){
    // 1. Set N to be the root
    node& N = get_root();

    // 2. Dive into leaf
CS2:
    if(N.level == 0){
        return N;
    }
    else{
        // Determine the minimum overlap cost
        if(N.level == 1){
            // Resolve ties by choosing the entry
            // whose rectangle needs least area enlargement
        }
        else{ // Determine the minumum area cost

        }
    }

    // 3. Set N to be the childnode pointed to by the
    // childpointer of the chosen entry and repeat from CS2
    N = chosen;
    goto CS2;
}