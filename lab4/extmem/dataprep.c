#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "extmem.h"
#include "extmem.c"
#include "header.h"
#include "myfunc.c"

#define rblkaddr 10000000
#define sblkaddr 20000000

int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */

    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    DataPrep(&buf,rblkaddr,16);
    DataPrep(&buf,sblkaddr,32);
    return 0;
}


/*准备测试数据*/
int DataPrep(Buffer *buf,unsigned int blkaddr,int len){

    int size;
    srand((unsigned int)time(NULL));
    tuple items[224];
    if(len % 7 ==0) {
        size = len /7;
    } else {
        size = len/7 +1;
    }

    for(int i =0;i<len;i++) {
        int ra;
        if(size == 16) {
            ra = rand()%40 + 1;
        } else {
            ra = rand()%41 + 20;
        }
        int rb = rand()%1000 +1 ;
        items[i].elem1 = ra;
        items[i].elem2 = rb;
    }
    TupleWriteData(buf,blkaddr,len,items);
    return 0;
}

