#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extmem.h"
#include "extmem.c"
#include "header.h"
#include "myfunc.c"
#include "btree.h"
#include "BTree.c"

#define oblkaddr 30000000
#define indexblkaddr 600


/*选择算法*/
int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */
    
    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    int value;
	if(argc != 4) {
		printf("usge:search <type> <attr> <value>\n");
		return -1;
	} else {
		if((strcmp(argv[1],"-l")==0)&&(strcmp(argv[2],"r") == 0 )) {
			sscanf(argv[3],"%d",&value);
			if(LinearSearch(&buf,rblkaddr,oblkaddr,value) !=0) {
				printf("search data failed!\n");
				return -1;
			}
		} 
		else if ((strcmp(argv[1],"-l")==0)&&(strcmp(argv[2],"s") == 0)) {
			sscanf(argv[3],"%d",&value);
			if(LinearSearch(&buf,sblkaddr,oblkaddr,value) !=0) {
				printf("search data failed!\n");
				return -1;
			}
		} 
		else if ((strcmp(argv[1],"-b")==0)&&(strcmp(argv[2],"r")==0))
		{
			/* code */
			sscanf(argv[3],"%d",&value);
			if(BinarySearch(&buf,rblkaddr,oblkaddr,value,112) !=0) {
				printf("search data failed!\n");
				return -1;
			}
		} 
		else if ((strcmp(argv[1],"-b")==0)&&(strcmp(argv[2],"s")==0))
		{
			/* code */
			sscanf(argv[3],"%d",&value);
			if(BinarySearch(&buf,sblkaddr,oblkaddr,value,224) !=0) {
				printf("search data failed!\n");
				return -1;
			}
		} 
		else if ((strcmp(argv[1],"-i")==0)&&(strcmp(argv[2],"r")==0))
		{
			/* code */
			sscanf(argv[3],"%d",&value);
			IndexSearch(&buf,rblkaddr,indexblkaddr,oblkaddr,value,112);
		}
		
		else if ((strcmp(argv[1],"-i")==0)&&(strcmp(argv[2],"s")==0))
		{
			/* code */
			sscanf(argv[3],"%d",&value);
			IndexSearch(&buf,sblkaddr,indexblkaddr,oblkaddr,value,224);
		}
		else if ((strcmp(argv[1],"-bt")==0)&&(strcmp(argv[2],"r")==0)) {
			sscanf(argv[3],"%d",&value);
			BTreeSearch(&buf,rblkaddr,oblkaddr,value);
		}
		else if((strcmp(argv[1],"-bt")==0)&&(strcmp(argv[2],"s")==0))
		{
			sscanf(argv[3],"%d",&value);
			BTreeSearch(&buf,sblkaddr,oblkaddr,value);			
		}
		else 
		{
			printf("usge:search <type> <attr> <value>\n");
			return -1;
		}
	}
	printf("\n# of IO's is %ld\n", buf.numIO);
    return 0;
}
/*线性搜索*/
int LinearSearch(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int value) {

	unsigned int addr;
	unsigned int outaddr;
	int blknum = 6;
	tuple outlist[7];
	int index = 0;
	outaddr = outblkaddr;
	addr = blkaddr;
	memset(outlist,0,56);
	printf("线性查找......\n");
	while(addr!=0)
	{
		tuple temp[42];
		memset(temp,0,blknum*7*8);
		addr = LoadData(buf,addr,temp,blknum);
		for(int i=0;i<blknum*7;i++)
		{
			if(temp[i].elem1 == value)
			{
				if(index == 7)
				{
					TupleWriteOneBlock(buf,outaddr,7,outlist,outaddr+1);
					memset(outlist,0,56);
					index = 0;outaddr++;
				}
				outlist[index++] = temp[i];
			}
		}
	}
	TupleWriteOneBlock(buf,outaddr,7,outlist,0);
	return 0;
}
/*二分搜索*/
int BinarySearch(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int value,int len) {

	unsigned int addr;
	unsigned int naddr;
	unsigned int outaddr;
	unsigned int noutaddr;
	int blknum = 6;//一次读取的磁盘块数 最大为7

	addr = blkaddr;
	printf("BinarySearch......\n");

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
	//排序
	addr = SortBlockData(buf,blkaddr,sortaddr,len,blknum);
	// addr = sortaddr;
	int find = -1;
	int flag = 0;
	tuple tuplelist[49];
	//分块二分查找
	while(addr!=0) {
		memset(tuplelist,0,blknum*7*8);
		naddr = LoadData(buf,addr,tuplelist,blknum);
		if(naddr == 0) {
			flag = 1;
			find = Binary(value,tuplelist,len-blknum*7*(blkcount-1));
		} else{
			find = Binary(value,tuplelist,blknum*7);
		}
		if(find != -1) {
			break;
		} else {
			addr = naddr;
		}
	}
	
	printf("find:%d\n",find);
	if (find == -1) {
		printf("no match!\n");
		return -1;
	} else { //找到了
		tuple outlist[7];
		memset(outlist,0,64);
		int index = 0;
		outaddr = oblkaddr;
		//搜索前面的元祖
		for(int i=find -1;i>=0;i--) {
			if(tuplelist[i].elem1 == value) {
				if(index == 7){
					noutaddr = outaddr + 1;
					TupleWriteOneBlock(buf,outaddr,7,outlist,noutaddr);
					memset(outlist,0,56);
					index = 0;outaddr = noutaddr;
				}
				outlist[index++] = tuplelist[i];
				printf("ra:%d;rb:%d\n",tuplelist[i].elem1,tuplelist[i].elem2);
			} else {
				break;
			}
		}
		int i = find;
		//搜索后面的元祖
		while((i<blknum*7 && flag == 0)|(i<len-blknum*7*(blkcount-1)&&flag ==1)) {
			if(tuplelist[i].elem1 == value) {
				if(index == 7){
					noutaddr = outaddr + 1;
					TupleWriteOneBlock(buf,outaddr,7,outlist,noutaddr);
					memset(outlist,0,56);
					index = 0;outaddr = noutaddr;
				}
				outlist[index++] = tuplelist[i];
				printf("ra:%d;rb:%d\n",tuplelist[i].elem1,tuplelist[i].elem2);
				if(i == blknum*7-1) { //到数组末尾则读取新数据
					addr = naddr;
					memset(tuplelist,0,blknum*7*8);
					naddr = LoadData(buf,addr,tuplelist,blknum);
					i = 0;
				}
			} else {
				break;
			}
			i++;
		}
		TupleWriteOneBlock(buf,outaddr,7,outlist,0);
		// DeleteData(buf,sortaddr);

	}
	return 0;
}
/*索引搜索*/
int IndexSearch(Buffer *buf,unsigned int blkaddr,unsigned int indexaddr,unsigned int outblkaddr,int value,int len)
{
	unsigned int addr,outaddr;
	int blknum = 6;
	int flag = 0;
	outaddr = outblkaddr;
	SortBlockData(buf,blkaddr,sortaddr,len,blknum); //排序
	addr = CreateIndex(buf,sortaddr,indexaddr,len); //创建索引
	//根据聚集索引查找最先出现的地址
	while(addr!=0)
	{
		tuple temp[42];
		memset(temp,0,blknum*7*8);
		addr = LoadData(buf,addr,temp,blknum);
		for(int i=0;i<blknum*7;i++)
		{
			if(temp[i].elem1 == value)
			{
				addr = temp[i].elem2;
				flag = 1;
				break;
			}
		}
		if(flag == 1)
		{
			break;
		}
	}
	tuple outlist[7];
	memset(outlist,0,56);
	int index = 0;
	flag = 0;
	while(addr!=0)
	{
		tuple temp[7];
		memset(temp,0,56);
		addr = LoadData(buf,addr,temp,1);
		for(int i =0;i<7;i++)
		{
			if(temp[i].elem1 == value)
			{
				if(index == 7)
				{
					TupleWriteOneBlock(buf,outaddr,7,outlist,outaddr+1);
					memset(outlist,0,56);
					outaddr++;index=0;
				}
				outlist[index++] = temp[i];
			}
			else if (temp[i].elem1 > value)
			{
				/* code */
				flag = 1;
				break;
			}
			
		}
		if(flag == 1)
		{
			break;
		}

	}
	TupleWriteOneBlock(buf,outaddr,7,outlist,0);
	return 0;
}
/*b树搜索*/
int BTreeSearch(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int value)
{
	unsigned int addr,outaddr;
	addr = CreateBTree(buf,blkaddr);
	Result result = SearchBTree(buf,addr,value);
	if(result.flag == 0)
	{
		return -1;
	}
	addr = result.blkaddr;
	outaddr = outblkaddr;
	tuple outlist[7];
	memset(outlist,0,56);
	int index = 0;
	while(addr != 0)
	{
		Addr addr_list[10];
		memset(addr_list,0,64);
		AddrLoadOneBlock(buf,addr,addr_list);
		for(int i=0;i<10&&addr_list[i].blkaddr!=0;i++)
		{
			if(index == 7)
			{
				TupleWriteOneBlock(buf,outaddr,7,outlist,outaddr+1);
				memset(outlist,0,56);
				outaddr++;index++;
			}
			tuple temp[7];
			LoadOneBlock(buf,addr_list[i].blkaddr,temp);
			outlist[index++] = temp[addr_list[i].offset];
		}
	}
	TupleWriteOneBlock(buf,outaddr,7,outlist,0);
	return 0;
}
