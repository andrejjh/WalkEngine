#include <stdlib.h>
#include <iostream>
#include <genNode.h>

void BeginPlugIn(boolean v){
    std::cout << ">>BeginPlugIn" << std::endl;
};
void EndPlugIn(std::ostream& os, void* root){
    std::cout << ">>EndPlugIn" << std::endl;
};
