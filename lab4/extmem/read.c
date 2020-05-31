#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extmem.h"
#include "extmem.c"
#include "header.h"
#include "myfunc.c"

int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */
    unsigned char *blk; /* A pointer to a block */

    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    /* Read the block from the hard disk */
    int blknum;
    sscanf(argv[2],"%d",&blknum);
    if ((blk = readBlockFromDisk(blknum, &buf)) == NULL)
    {
        perror("Reading Block Failed!\n");
        return -1;
    }
    
    if(strcmp(argv[1],"-s") == 0 ){
        ReadAll(&buf,blknum,0);
    } 
    else if (strcmp(argv[1],"-p")==0)
    {
        ReadAll(&buf,blknum,1);
    } else if(strcmp(argv[1],"-j")==0) {
        ReadAll(&buf,blknum,2);
    }
    
    printf("\n");
    printf("# of IO's is %d\n", buf.numIO); /* Check the number of IO's */

    return 0;
}

