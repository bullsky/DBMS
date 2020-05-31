#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extmem.h"
#include "extmem.c"
#include "header.h"
#include "myfunc.c"

#define oblkaddr 40000000

int * BubbleSort1(int *temp, int len);

int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */
    
    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    if(argc != 2){
        printf("usage: projection <attr>[-r,-s]\n");
        return -1;
    }
    if(strcmp(argv[1],"-r")==0) {
        if(projection(&buf,rblkaddr,oblkaddr,112) != 0) {
            perror("projection Failed!\n");
            return -1;
        }
    } 
    else if (strcmp(argv[1],"-s")==0)
    {
        /* code */
        if(projection(&buf,sblkaddr,oblkaddr,224) != 0)
        {
            perror("projection Failed!\n");
            return -1;
        }
    }
	printf("\n# of IO's is %ld\n", buf.numIO);
    return 0;
}

/*投影*/
int projection(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int len) {
	unsigned int addr;
    unsigned int naddr;
    int blknum = 6;
	addr = SortBlockData(buf,blkaddr,sortaddr,len,blknum);
    blknum = 4;
    int outlist[15];
    unsigned int outaddr = oblkaddr;
    unsigned int noutaddr;
    int index = 0;
    printf("关系投影......\n");
    while(addr!=0){
        tuple temp[28];
        memset(temp,0,blknum*7*8);
        naddr = LoadData(buf,addr,temp,blknum);
        int i=0;int j=1;
        while(i<blknum*7){
            if(index == 15) {
                noutaddr = outaddr + 1;
                WriteOneBlockOrigin(buf,outaddr,1,outlist,15,noutaddr);
                memset(outlist,0,60);
                outaddr = noutaddr;
                index = 0;
            }
            outlist[index++] = temp[i].elem1;
            while(temp[i].elem1 == temp[j].elem1){
                j++;
                if(j == blknum*7) {break;}
            }
            i = j;j++;
        }
        addr = naddr;
    }
    WriteOneBlockOrigin(buf,outaddr,1,outlist,15,0);
    return 0;
}