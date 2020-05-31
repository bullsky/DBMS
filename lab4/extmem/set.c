#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extmem.h"
#include "extmem.c"
#include "header.h"
#include "myfunc.c"

#define oblkaddr 60000000

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
        printf("usage: set <type>[-i/-u/-d]\n");
        return -1;
    } else {
        if(strcmp(argv[1],"-i") == 0) {
            int len = IntersectionSet(&buf,rblkaddr,sblkaddr,oblkaddr);
            printf("total %d tuple\n",len);
        } 
        else if (strcmp(argv[1],"-u")==0)
        {
            /* code */
            int len = UnionSet(&buf,rblkaddr,sblkaddr,oblkaddr);
            printf("total %d tuple\n",len);
        } 
        else if (strcmp(argv[1],"-d")==0)
        {
            /* code */
            int len = DifferenceSet(&buf,rblkaddr,sblkaddr,oblkaddr);
            printf("total %d tuple\n",len);
        }
        
    }
    printf("\n# of IO's is %ld\n", buf.numIO);
    return 0;
}
/*交集*/
int IntersectionSet(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr) {

    unsigned int r_addr,nr_addr,s_addr,ns_addr,outaddr,noutaddr;
    int blknum = 3;
    int index = 0;
    int count = 0;
    r_addr = r_blkaddr;
    s_addr = s_blkaddr;
    outaddr = outblkaddr;
    tuple outlist[7];
    memset(outlist,0,56);
    while(r_addr!=0)
    {
        tuple r_list[21];
        nr_addr = LoadData(buf,r_addr,r_list,blknum);
        while(s_addr!=0)
        {
            tuple s_list[21];
            ns_addr = LoadData(buf,s_addr,s_list,blknum);
            for(int i=0;i<blknum*7;i++)
            {
                for(int j=0;j<blknum*7;j++)
                {
                    if((r_list[i].elem1 == s_list[j].elem1) && (r_list[i].elem2 == s_list[j].elem2))
                    {
                        if(index == 7)
                        {
                            noutaddr = outaddr + 1;
                            TupleWriteOneBlock(buf,outaddr,7,outlist,noutaddr);
                            memset(outlist,0,56);
                            outaddr = noutaddr;
                            index = 0;
                        }
                        outlist[index++] = r_list[i];
                        count++;
                    }
                }
            }
            s_addr = ns_addr;
        }
        s_addr = sblkaddr;
        r_addr = nr_addr;
    }
    TupleWriteOneBlock(buf,outaddr,7,outlist,0);
    return count;
}
/*差集*/
int DifferenceSet(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr) {

    unsigned int r_addr,nr_addr,s_addr,ns_addr,outaddr,noutaddr;
    tuple outlist[7];
    int index = 0 ;
    int r_blknum = 4;
    int s_blknum = 2;
    int count = 0;
    r_addr = r_blkaddr;
    s_addr = s_blkaddr;
    outaddr = outblkaddr;
    memset(outlist,0,56);
    while(r_addr!=0)
    {
        tuple r_list[28];
        memset(r_list,0,r_blknum*7*8);
        nr_addr = LoadData(buf,r_addr,r_list,r_blknum);
        for(int i=0;i<r_blknum*7;i++)
        {        
            int flag = 0;
            while(s_addr!=0)
            {
                tuple s_list[14];
                memset(s_list,0,s_blknum*7*8);
                ns_addr = LoadData(buf,s_addr,s_list,s_blknum);
                for(int j=0;j<s_blknum*7;j++)
                {
                    if((r_list[i].elem1 == s_list[j].elem1) && (r_list[i].elem2 == s_list[j].elem2))
                    {
                        flag = 1;
                        break;
                    }
                } 
                if(flag == 1) {break;}
                s_addr = ns_addr;
            }
            if(flag == 0)
            {
                if(index == 7)
                {
                    noutaddr = outaddr + 1;
                    TupleWriteOneBlock(buf,outaddr,7,outlist,noutaddr);
                    memset(outlist,0,56);
                    outaddr = noutaddr;
                    index = 0;
                }
                printf("diff elem1:%d;elem2:%d\n",r_list[i].elem1,r_list[i].elem2);
                outlist[index++] = r_list[i];
                count++;
            }
            s_addr = sblkaddr;
        }
        r_addr = nr_addr;
    }
    TupleWriteOneBlock(buf,outaddr,7,outlist,0);
    return count;
}
/*并集*/
int UnionSet(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr) {

    unsigned int inter_addr = 200;
    unsigned int diff_addr = 300;
    //求R和S的交集
    IntersectionSet(buf,r_blkaddr,s_blkaddr,inter_addr);
    //求R和R交S的查集
    int len = DifferenceSet(buf,r_blkaddr,inter_addr,diff_addr);
    //讲s_blkaddr的数据和diff_addr结果合并
    unsigned int addr,outaddr;
    addr = s_blkaddr;
    outaddr = oblkaddr;
    while(addr !=0 )
    {
        tuple temp[7];
        memset(temp,0,56);
        addr = LoadData(buf,addr,temp,1);
        TupleWriteOneBlock(buf,outaddr,7,temp,outaddr+1);
        outaddr++;
    }
    addr = diff_addr;
    while(addr!=0){
        tuple temp[7];
        memset(temp,0,56);
        addr = LoadData(buf,addr,temp,1);
        if(addr != 0)
        {
            TupleWriteOneBlock(buf,outaddr,7,temp,outaddr+1);
            outaddr++;
        } else {
            TupleWriteOneBlock(buf,outaddr,7,temp,0);
        }
    }
    //删除中间结果
    DeleteData(buf,inter_addr);
    DeleteData(buf,diff_addr);
    return len+224;
}
