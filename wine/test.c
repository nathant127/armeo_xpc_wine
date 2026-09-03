#include "xpc/include/xpcapi.h"

#include <stdio.h>

int main(int argc, char** argv) {
    if (xPCInitAPI("../dll/xpcapi.dll") == -1) {
        printf("Failed to load .dll\n");
        return 1;
    }


    
}
