void Insert(){
    // Invoke ChooseSubtree, with the level as a parameter
    // to find an appropriate node N, in which to place the
    // new entry E

    // If N has less than M entries, accommodate E in N
    // If N has M entries, invoke OverflowTreatment with the
    // level of N as a parameter [for reinsertion or split]

    // If OverflowTreatment was called and a split was
    // performed, propagate OverflowTreatment upwards
    // if necessary
    // If OverflowTreatment caused a split of the root, create a
    // new root

    // Adjust all covering rectangles in the insertion path
    // such that they are minumum bounding boxes
    // enclosing their children rectangles
}