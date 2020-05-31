#include <stdlib.h>
#include <stdio.h>
#include "header.h"
#include "btree.h"

/*从磁盘读取结果*/
int ReadAll(Buffer *buf,unsigned int addr,int type) 
{
    unsigned char *blk;
    while(addr!=0) {
        blk = readBlockFromDisk(addr, buf);
        if(type == 0) {
            addr = ReadOneBlock(buf,addr,0);
        } 
        else if (type==1)
        {
            addr = ReadOneBlock(buf,addr,1);
        } else if(type == 2) {

            addr = ReadOneBlock(buf,addr,2);
        }
		freeBlockInBuffer(blk,buf);
	}
    return 0;
}
/*读一个磁盘块*/
unsigned int ReadOneBlock(Buffer *buf,unsigned int addr,int type)
{
    unsigned char *blk;
    blk = readBlockFromDisk(addr, buf);
    printf("read block %d\n",addr);
    if(type == 0) {
        for (int i=0;i<7;i++) {
            int elem1,elem2;
            memcpy(&elem1,blk+i*8,4);
            memcpy(&elem2,blk+i*8+4,4);
            printf("elem1:%d;elem2:%d\n",elem1,elem2);
        }
    } 
    else if (type == 1)
    {
        /* code */
        for(int i=0;i<15;i++) {
            int elem;
            memcpy(&elem,blk+i*4,4);
            printf("elem:%d\n",elem);
        }
    } 
    else if (type == 2)
    {
        /* code */
        for(int i=0;i<5;i++) {
            int elem1,elem2,elem3;
            memcpy(&elem1,blk+i*12,4);
            memcpy(&elem2,blk+i*12+4,4);
            memcpy(&elem3,blk+i*12+8,4);
            printf("A(C):%d;B:%d;D:%d\n",elem1,elem2,elem3);
        }
    }
    memcpy(&addr,blk+60,4);
    freeBlockInBuffer(blk,buf);  
    return addr;  
}
/*二分查找*/
int Binary(int num, tuple *temp, int len)
{
	int left = 0, right = len, mid;
	while (left <= right)
	{
		mid = (left + right) / 2;
		if (temp[mid].elem1 == num)
		{
			return mid;
		}
		if (temp[mid].elem1 < num)
		{
			left = mid + 1;
		}
		else
			right = mid - 1;
	}
	return -1;
}
/*冒泡排序*/
tuple * BubbleSort(tuple *temp, int len)
{
	int i, j;
	tuple tempp;
	for (i = 0; i<len; i++)
	{
		for (j = len - 1; j >= i + 1; j--)
		{
			if (temp[j].elem1 < temp[j - 1].elem1)
			{
				tempp = temp[j];
				temp[j] = temp[j - 1];
				temp[j - 1] = tempp;
			}
		}
	}
	return temp;
}
/*加载磁盘块数据*/
unsigned int LoadData(Buffer *buf,unsigned int blkaddr, tuple * temp,int blknum) 
{
    int count = 0;
    unsigned int addr,naddr;
    addr = blkaddr;
    while(addr!=0 && count < blknum) {
        naddr = LoadOneBlock(buf,addr,temp+count*7);
        count++;addr = naddr;
	}
    return addr;
}
/*加载一个磁盘块数据到元祖*/
unsigned int LoadOneBlock(Buffer *buf,unsigned int blkaddr, tuple * temp)
{
    int index = 0;
    unsigned char *blk;
    unsigned int addr;
    block *read;
    blk = readBlockFromDisk(blkaddr, buf);
    read = (block *)(blk);
    printf("load block %d\n",blkaddr);
    for (int i=0;i<7;i++) {
        if(read->items[i].elem1 != 0){
            temp[index++] = read->items[i];
        }
    }
    addr = read->nblkaddr;
    freeBlockInBuffer(blk,buf);  
    return addr;  
}
/*打印一个元祖*/
void PrintTuple(tuple item)
{
    printf("elem1:%d;elem2:%d\n",item.elem1,item.elem2);
}
/*打印元祖数据*/
void PrintTupleList(tuple *tlist,int len) 
{
    for(int i=0;i<len;i++) {
        printf("TupleList[%d]:elem1:%d;elem2:%d\n",i,tlist[i].elem1,tlist[i].elem2);
    }
}
/*写入磁盘数据
    num:一个元祖的int个数
    len:int 数组temp的长度
*/
int WriteOneBlockOrigin(Buffer *buf,unsigned int blkaddr,int num,int *temp,int len,unsigned int nblkaddr)
{
    unsigned char *outblk;
    outblk = getNewBlockInBuffer(buf);
    memset(outblk,0,64);
    if(len%num !=0) {
        printf("参数错误！\n");
        return -1;
    }
    memcpy(outblk,temp,len*4);
    memcpy(outblk+60,&nblkaddr,4);
    writeBlockToDisk(outblk, blkaddr, buf);
    freeBlockInBuffer(outblk,buf);
    printf("write block at addr:%d\n",blkaddr);
    return 0;
}
/*写入一块磁盘*/
int TupleWriteOneBlock(Buffer *buf,unsigned int blkaddr,int tuplenum,tuple *temp,unsigned int nblkaddr)
{
    int *tmp;
    tmp = (int *)(temp);
    return WriteOneBlockOrigin(buf,blkaddr,2,tmp,tuplenum*2,nblkaddr);
}
/*写入一块磁盘*/
int JtupleWriteOneBlock(Buffer *buf,unsigned int blkaddr,int jtuplenum,jtuple *temp,unsigned int nblkaddr)
{
    int *tmp;
    tmp = (int *)(temp);
    return WriteOneBlockOrigin(buf,blkaddr,3,tmp,3*jtuplenum,nblkaddr);
}
/*将数据写入磁盘
    num:一个元祖的int个数
    len:int 数组temp的长度
*/
int WriteDataOrigin(Buffer *buf,unsigned int blkaddr,int num,int *temp,int len)
{
    if(len % num != 0){
        printf("参数错误！\n");
        return -1;
    }
    int blocktuplenum = 15/num;//一个磁盘块能容纳的元组个数
    int tuplenum = len/num;//数据所有的元祖的个数
    int size = 0;//需要写入的磁盘块数
    if(tuplenum % blocktuplenum == 0){
        size = tuplenum/blocktuplenum;
    } else{
        size = tuplenum/blocktuplenum+1;
    }
    unsigned int addr;
    unsigned int naddr;
    addr = blkaddr;
    printf("size:%d\n",size);
    for(int i=0;i<size;i++){
        if(i<size-1){
            naddr = addr + 1;
            WriteOneBlockOrigin(buf,addr,num,temp+i*blocktuplenum*num,blocktuplenum*num,naddr);
        } else {
            naddr = 0;
            WriteOneBlockOrigin(buf,addr,num,temp+i*blocktuplenum*num,len-blocktuplenum*num*(size-1),naddr);
        }
        addr = naddr;
    }
    return 0;
}
/*将数据写入磁盘*/
int TupleWriteData(Buffer *buf,unsigned int blkaddr,int len,tuple *temp)
{
    int *tmp;
    tmp = (int *)(temp);
    return WriteDataOrigin(buf,blkaddr,2,tmp,2*len);
}
/*将数据写入磁盘*/
int JTupleWriteData(Buffer *buf,unsigned int blkaddr,int len,jtuple *temp)
{
    int *tmp;
    tmp = (int *)(temp);
    return WriteDataOrigin(buf,blkaddr,3,tmp,3*len);
}
/*删除数据*/
int DeleteData(Buffer *buf,unsigned int blkaddr) 
{

    block *read;
    unsigned int addr;
    unsigned int naddr;
    unsigned char *blk;
    addr = blkaddr;
    while(addr!=0) {
        blk = readBlockFromDisk(addr,buf);
        read = (block *)(blk);
        naddr = read->nblkaddr;
        dropBlockOnDisk(addr);
        addr = naddr;
        freeBlockInBuffer(blk,buf);
    }
    return 0;
}
/*hash函数*/
int HashFunc(int x,int bucketsize)
{
    int y;
    y = x % bucketsize;
    return y;
}
/*创建hash桶*/
int CreateHashBucket(Buffer *buf,unsigned int blkaddr,int len,hashbucket *bucket,int bucketsize,unsigned int oblkaddr)
{
    printf("create hash bucket.....\n");
    //初始化桶
    for(int i=0;i<bucketsize;i++)
    {
        bucket[i].key = i;
        bucket[i].num = 0;
        bucket[i].blknum = 0;
        bucket[i].first_addr = oblkaddr + i*100;
        bucket[i].end_addr = oblkaddr +i*100;
    }

    int blknum = 4;
    unsigned int addr;
    addr = blkaddr;
    while(addr!=0)
    {
        tuple temp[42];
        memset(temp,0,blknum*7*8);
        addr = LoadData(buf,addr,temp,blknum);

        for(int i=0;i<blknum*7;i++)
        {
            int tmp = HashFunc(temp[i].elem1,bucketsize);
            if(bucket[tmp].num == 0) //插入桶中第一个数据
            {
                tuple outlist[7];
                memset(outlist,0,56);
                outlist[0] = temp[i];
                TupleWriteOneBlock(buf,bucket[tmp].end_addr,7,outlist,0);
                bucket[tmp].num++;
                bucket[tmp].blknum++;
            } 
            else
            {
                tuple outlist[7];
                int index = bucket[tmp].num - (bucket[tmp].blknum-1) * 7; //计算插入地址
                if(index == 7) //扩充磁盘块
                {
                    memset(outlist,0,56);
                    LoadData(buf,bucket[tmp].end_addr,outlist,1);
                    TupleWriteOneBlock(buf,bucket[tmp].end_addr,7,outlist,bucket[tmp].end_addr+1);
                    bucket[tmp].end_addr ++; //添加新磁盘块
                    memset(outlist,0,56);
                    outlist[0] = temp[i];
                    TupleWriteOneBlock(buf,bucket[tmp].end_addr,7,outlist,0);
                    bucket[tmp].blknum ++;
                    bucket[tmp].num ++;
                } else { 
                    memset(outlist,0,56);
                    LoadData(buf,bucket[tmp].end_addr,outlist,1);       
                    outlist[index] = temp[i];
                    TupleWriteOneBlock(buf,bucket[tmp].end_addr,7,outlist,0);
                    bucket[tmp].num++;
                }
            }
        }
    }
    return 0;
}
/*读取hash桶*/
void ReadBucket(Buffer *buf,hashbucket *bucket,int bucketsize) 
{
    printf("read bucket......\n");

    for(int i=0;i<bucketsize;i++) {
        printf("bucket:%d\n",i);
        if(bucket[i].num != 0)
        {
            ReadAll(buf,bucket[i].first_addr,0);
        }
    }
}
/*删除hash桶*/
void DeleteBucket(Buffer *buf,hashbucket *bucket,int bucketsize)
{
    for(int i=0;i<bucketsize;i++)
    {
        if(bucket[i].num != 0)
        {
            DeleteData(buf,bucket[i].first_addr);
        }
    }
}
/*合并相邻子序列*/
unsigned int Merge(Buffer *buf,unsigned int laddr,unsigned int raddr)
{
    unsigned int l_addr;
    unsigned int nl_addr;
    unsigned int r_addr;
    unsigned int nr_addr;
    int blknum = 2;//最大为2
    tuple l_list[7*2];
    tuple r_list[7*2]; 
    tuple temp[7*3];
    unsigned int tempblkaddr [32];
    int blkcount = 0;
    for(int i=0;i<32;i++) {  //临时地址空间
        tempblkaddr[i] = 200+i*blknum;
    }
    l_addr = laddr;r_addr = raddr;
    memset(l_list,0,7*blknum);
    nl_addr = LoadData(buf,l_addr,l_list,blknum);
    memset(r_list,0,7*blknum);
    nr_addr = LoadData(buf,r_addr,r_list,blknum);
    printf("begin....\n");
    int i=0;
    int j=0;
    int index = 0;
    while(l_addr !=0 && r_addr !=0) {
        printf("index:%d\n",index);
        if(l_list[i].elem1 <= r_list[j].elem1) {
            temp[index++] = l_list[i++];
        } else {
            temp[index++] = r_list[j++];
        }
        if(index == 7*blknum) { //满则写入到磁盘
            TupleWriteData(buf,tempblkaddr[blkcount],blknum*7,temp);
            blkcount++;index = 0;
            memset(temp,0,blknum*7);
        }
        if(i == 7*blknum) { //到数组末尾则读取新的磁盘块
            if(nl_addr != 0) { //若还有新的磁盘块，读取
                memset(l_list,0,7*blknum);
                l_addr = nl_addr;
                nl_addr = LoadData(buf,l_addr,l_list,blknum);
                i = 0;
            } else {
                l_addr = nl_addr;
            }
        }
        if(j == 7*blknum) { //到数组末尾则读取新的磁盘块
            if(nr_addr != 0) { //若还有新的磁盘块，读取
                memset(r_list,0,7*blknum);
                r_addr = nr_addr;
                nr_addr = LoadData(buf,r_addr,r_list,blknum);
                j = 0;
            } else {
                r_addr = nr_addr;
            }
        }
    }
    printf("Fill in the remaining elements\n");
    /*填充剩余元素*/
    if(i < blknum*7) {
        while(l_addr!=0) {
            temp[index++] = l_list[i++];
            if(index == 7*blknum) {//满则写入磁盘
                TupleWriteData(buf,tempblkaddr[blkcount],blknum*7,temp);
                blkcount++;index = 0;
                memset(temp,0,blknum*7);
            }
            if(i == 7*blknum){ //到数组末尾则读取新的磁盘块
                if(nl_addr != 0) {//若还有新的磁盘块，读取
                    memset(l_list,0,7*blknum);
                    l_addr = nl_addr;
                    nl_addr = LoadData(buf,l_addr,l_list,blknum);
                    i = 0;
                } else {
                    l_addr = nl_addr;
                }
            }
        }
    }
    /*填充剩余元素*/
    if(j < blknum*7) {
        while(r_addr != 0) {
            temp[index++] = r_list[j++];
            if(index == 7*blknum) {//满则写入磁盘
                TupleWriteData(buf,tempblkaddr[blkcount],blknum*7,temp);
                blkcount++;index = 0;
                memset(temp,0,blknum*7);
            }
            if(j == 7*blknum) {//到数组末尾则读取新的磁盘块
                if(nr_addr != 0) {//若还有新的磁盘块，读取
                    memset(r_list,0,7*blknum);
                    r_addr = nr_addr;
                    nr_addr = LoadData(buf,r_addr,r_list,blknum);
                    j = 0;
                } else {
                    r_addr = nr_addr;
                }
            }
        }
    }
    // printf("打印中间排序结果\n");
    // for(int i=0;i<blkcount;i++){
    //     ReadAll(buf,tempblkaddr[i],0);
    // }

    /*连接两个磁盘块*/
    l_addr = laddr;
    while(l_addr!=0) {
        nl_addr = LoadData(buf,l_addr,l_list,1);
        if(nl_addr == 0) {
            TupleWriteOneBlock(buf,l_addr,7,l_list,raddr);
            break;
        }
        l_addr = nl_addr;
    }
    /*将临时结果从磁盘中覆盖原来的*/
    printf("Overwrite temporary results from disk\n");
    int k=0;
    l_addr = laddr;
    while(l_addr != 0 && k<blkcount) {
        memset(temp,0,7);
        LoadData(buf,tempblkaddr[k],temp,blknum);
        for(int i=0;i<blknum;i++){
            nl_addr = LoadData(buf,l_addr,l_list,1);
            TupleWriteOneBlock(buf,l_addr,7,temp+i*7,nl_addr);
            l_addr = nl_addr;
        }
        k++;
    }

     /*从磁盘删除临时结果*/
    for(int i=0;i<blkcount;i++) {
        DeleteData(buf,tempblkaddr[i]);
    }

    return laddr;
}
/*外部排序*/
unsigned int ExternSort(Buffer *buf,unsigned int *tempblkaddr,int left,int right) {

    int mid = (left+right)/2;
    if(left < right) {
        unsigned int l_addr = ExternSort(buf,tempblkaddr,left,mid); //递归求解
        unsigned int r_addr = ExternSort(buf,tempblkaddr,mid+1,right);
        printf("Merge l_addr:%d r_addr:%d\n",l_addr,r_addr);
        return Merge(buf,l_addr,r_addr);
    } else {
        return tempblkaddr[left];
    }
}
/*讲磁盘数据排序*/
unsigned int SortBlockData(Buffer *buf,unsigned int blkaddr,unsigned int oblkaddr,int len,int blknum){

	unsigned int addr;
	unsigned int naddr;
	// int blknum = 6;//一次读取的磁盘块数 最大为7
	addr = blkaddr;
	int blocksize =0;//磁盘块数
	if(len % 7==0) {
		blocksize = len/7;
	} else {
		blocksize = len/7 +1;
	}
	int blkcount = 0;//需要读取的磁盘次数
	if(blocksize % blknum ==0) {
		blkcount = blocksize /blknum;
	} else {
		blkcount = blocksize /blknum + 1;
	}
    unsigned int tempblkaddr [32]; //临时结果地址空间
    for(int i=0;i<blkcount;i++) {
        tempblkaddr[i] = oblkaddr + i*blknum;
	}
	for(int i=0;i<blkcount;i++) { //内排，将结果写入磁盘块
		tuple tuplelist[49];
		memset(tuplelist,0,blknum*7*8);
		if(i!=blkcount-1) {
			memset(tuplelist,0,blknum*7*8);
			naddr = LoadData(buf,addr,tuplelist,blknum);
			BubbleSort(tuplelist,blknum*7);
			TupleWriteData(buf,tempblkaddr[i],blknum*7,tuplelist);
		} else {
			int m = blocksize-blknum*(blkcount-1);
			memset(tuplelist,0,blknum*7*8);
			naddr = LoadData(buf,addr,tuplelist,m);
			BubbleSort(tuplelist,m*7);
			TupleWriteData(buf,tempblkaddr[i],m*7,tuplelist);
		}
		addr = naddr;
	}
	addr = ExternSort(buf,tempblkaddr,0,blkcount-1); //外排，将有序磁盘块归并
    return addr;
}
/*创建聚集索引*/
unsigned int CreateIndex(Buffer *buf,unsigned int blkaddr,unsigned int oblkaddr, int len)
{
    unsigned int addr,naddr,outaddr;
    outaddr = oblkaddr;
    tuple outlist[7];
    int index = 0;
    memset(outlist,0,56);
    int max = 0; //记录当前最大元素
    int size = 0;
    while(addr != 0)
    {
        tuple temp[7];
        memset(temp,0,56);
        naddr = LoadData(buf,addr,temp,1);
        int num;
        if(naddr == 0)
        {
            num = len - size*7;
        } else {
            num = 7;
        }
        for(int i=0;i<num;i++)
        {
            if(temp[i].elem1 > max) //遇到新值则添加
            {
                max = temp[i].elem1;
                if(index == 7) //满则写入磁盘
                {
                    TupleWriteOneBlock(buf,outaddr,7,outlist,outaddr+1);
                    memset(outlist,0,56);
                    outaddr++;index=0;
                }
                outlist[index].elem1 = temp[i].elem1;
                outlist[index].elem2 = addr;
                index++;
            }
        }
        addr = naddr;
        size ++;
    }
    TupleWriteOneBlock(buf,outaddr,7,outlist,0);
    return oblkaddr;
}
