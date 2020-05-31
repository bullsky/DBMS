#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extmem.h"
#include "extmem.c"
#include "header.h"
#include "myfunc.c"

#define oblkaddr 50000000


/*连接算法*/
int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */
    
    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    if(argc != 2) {
        printf("usage: join <algo>[-n/-s/-h]\n");
        return -1;
    } else {
        if(strcmp(argv[1],"-n") == 0) {
            NestLoopJoin(&buf,rblkaddr,sblkaddr,oblkaddr);
        } 
        else if (strcmp(argv[1],"-s")==0)
        {
            /* code */
            SortMergeJoin(&buf,rblkaddr,sblkaddr,oblkaddr);
        } 
        else if (strcmp(argv[1],"-h")==0)
        {
            /* code */
            HashJoin(&buf,rblkaddr,sblkaddr,oblkaddr);
        }
    }
	printf("\n# of IO's is %ld\n", buf.numIO);
    return 0;
}
/*Nest-Loop- Join算法*/
int NestLoopJoin(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr) {

    unsigned int r_addr;
    unsigned int s_addr;
	unsigned int outaddr;
 
    jtuple outlist[5];
    int index = 0;
    int r_blknum = 5;
    int s_blknum = 1;

    r_addr = r_blkaddr;
    s_addr = s_blkaddr;
    outaddr = outblkaddr;
    memset(outlist,0,60);
    while(r_addr!=0)
    {
        tuple r_list[35];
        memset(r_list,0,r_blknum*7*8);
        r_addr = LoadData(buf,r_addr,r_list,r_blknum);
        for(int i=0;i<r_blknum*7;i++)
        {
            printf("i:%d\n",i);
            while(s_addr != 0)
            {
                tuple s_list[7];
                memset(s_list,0,s_blknum*7*8);
                s_addr = LoadData(buf,s_addr,s_list,s_blknum);
                for(int j=0;j<s_blknum*7;j++)
                {
                    if(r_list[i].elem1 == s_list[j].elem1)
                    {
                        if(index == 5)
                        {
                            JtupleWriteOneBlock(buf,outaddr,5,outlist,outaddr+1);
                            memset(outlist,0,60);
                            outaddr++;index=0;
                        }
                        jtuple jtmp;
                        jtmp.elem1 = r_list[i].elem1;
                        jtmp.elem2 = r_list[i].elem2;
                        jtmp.elem3 = s_list[j].elem2;
                        outlist[index++] = jtmp;
                    }
                }
            }
            s_addr = sblkaddr;
        }
    }
    JtupleWriteOneBlock(buf,outaddr,5,outlist,0);
    return 0;
}
/*Sort-Merge-Join算法*/
int SortMergeJoin(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr){

    unsigned int r_addr,nr_addr;
    unsigned int s_addr,ns_addr;
	unsigned int outaddr;
    int index = 0;
    int r_blknum = 3;
    int s_blknum = 3;

    outaddr = outblkaddr;

    unsigned int r_sortaddr = 300;
    unsigned int s_sortaddr = 400;
    r_addr = SortBlockData(buf,r_blkaddr,r_sortaddr,112,6);
    s_addr = SortBlockData(buf,s_blkaddr,s_sortaddr,224,6);

    tuple r_list[21];
    tuple s_list[21];
    jtuple outlist[5];
    memset(r_list,0,r_blknum*7*8);
    memset(s_list,0,s_blknum*7*8);
    memset(outlist,0,60);
    nr_addr = LoadData(buf,r_addr,r_list,r_blknum);
    ns_addr = LoadData(buf,s_addr,s_list,s_blknum);
    int i=0;int j=0;
    while(r_addr!=0 && s_addr!=0)
    {
        if(r_list[i].elem1 == s_list[j].elem1)
        {
            int k = j;
            unsigned int temp_addr = ns_addr;
            while(s_list[k].elem1 == r_list[i].elem1)
            {
                if(index == 5)
                {
                    JtupleWriteOneBlock(buf,outaddr,5,outlist,outaddr+1);
                    memset(outlist,0,60);
                    index = 0;outaddr++;
                }
                jtuple jtmp;
                jtmp.elem1 = r_list[i].elem1;
                jtmp.elem2 = r_list[i].elem2;
                jtmp.elem3 = s_list[k].elem2;
                outlist[index++] = jtmp;
                k++;
                if(k == s_blknum*7)
                {
                    if(temp_addr != 0)
                    {
                        memset(s_list,0,s_blknum*7*8);
                        temp_addr = LoadData(buf,temp_addr,s_list,s_blknum);
                        k = 0;
                    } else {
                        break;
                    }
                }
            }
            LoadData(buf,s_addr,s_list,s_blknum);
            i++;
            if(i == r_blknum*7)
            {
                if(nr_addr != 0)
                {
                    memset(r_list,0,r_blknum*7*8);
                    r_addr = nr_addr;
                    nr_addr = LoadData(buf,r_addr,r_list,r_blknum);
                    i = 0;
                } else {
                    r_addr = nr_addr;
                }
            }
        } 
        else if (r_list[i].elem1 < s_list[j].elem1)
        {
            /* code */
            i++;
            if(i == r_blknum*7)
            {
                if(nr_addr != 0)
                {
                    memset(r_list,0,r_blknum*7*8);
                    r_addr = nr_addr;
                    nr_addr = LoadData(buf,r_addr,r_list,r_blknum);
                    i = 0;
                } else {
                    r_addr = nr_addr;
                }
            }
        } 
        else if (r_list[i].elem1 > s_list[j].elem1)
        {
            /* code */
            j++;
            if(j == s_blknum*7)
            {
                if(ns_addr != 0)
                {
                    memset(s_list,0,s_blknum*7*8);
                    s_addr = ns_addr;
                    ns_addr = LoadData(buf,s_addr,s_list,s_blknum);
                    j = 0;
                } else {
                    s_addr = ns_addr;
                }
            }
        }
    }
    JtupleWriteOneBlock(buf,outaddr,5,outlist,0);

    DeleteData(buf,r_sortaddr);
    DeleteData(buf,s_sortaddr);
    return 0;
}
/*Hash-Join算法*/
int HashJoin(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr) {

    unsigned int outaddr;
    int bucketsize = 16;
    hashbucket r_bucket[16];
    hashbucket s_bucket[16];
    outaddr = outblkaddr;

    CreateHashBucket(buf,r_blkaddr,112,r_bucket,bucketsize,10000);
    CreateHashBucket(buf,s_blkaddr,224,s_bucket,bucketsize,20000);
    printf("create hash bucket done!\n");

    // ReadBucket(buf,r_bucket,bucketsize);
    // ReadBucket(buf,s_bucket,bucketsize);

    jtuple outlist[5];
    int index = 0;
    int r_blknum = 3;
    int s_blknum = 3;
    
    for(int i=0;i<bucketsize;i++)
    {
        if(r_bucket[i].num !=0 && s_bucket[i].num!=0)
        {
            unsigned int r_addr = r_bucket[i].first_addr;
            while(r_addr!=0)
            {
                unsigned int s_addr = s_bucket[i].first_addr;
                tuple r_list[21];
                memset(r_list,0,r_blknum*7*8);
                r_addr = LoadData(buf,r_addr,r_list,r_blknum);
                while(s_addr!=0)
                {
                    tuple s_list[21];
                    memset(s_list,0,s_blknum*7*8);
                    s_addr = LoadData(buf,s_addr,s_list,s_blknum);
                    for(int i=0;i<r_blknum*7;i++)
                    {
                        for(int j=0;j<s_blknum*7;j++)
                        {
                            if((r_list[i].elem1 == s_list[j].elem1) && (r_list[i].elem1 != 0))
                            {
                                if(index == 5)
                                {
                                    JtupleWriteOneBlock(buf,outaddr,5,outlist,outaddr+1);
                                    memset(outlist,0,60);
                                    outaddr++;index=0;
                                }
                                jtuple jtmp;
                                jtmp.elem1 = r_list[i].elem1;
                                jtmp.elem2 = r_list[i].elem2;
                                jtmp.elem3 = s_list[j].elem2;
                                outlist[index++] = jtmp;
                            }
                        }
                    }
                }
            }
        }
    }
    JtupleWriteOneBlock(buf,outaddr,5,outlist,0);
    DeleteBucket(buf,r_bucket,bucketsize);
    DeleteBucket(buf,s_bucket,bucketsize);
    return 0;
}
