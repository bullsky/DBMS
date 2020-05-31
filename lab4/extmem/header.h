#ifndef HEADER_H
#define HEADER_H
#include "extmem.h"

#define bufsize 520
#define rblkaddr 10000000
#define sblkaddr 20000000
#define sortaddr 100

/*元祖结构体 16字节*/
typedef struct tuple
{
	int elem1;
	int elem2;
}tuple;
/*磁盘块结构体*/
typedef struct block{
    tuple items[7];
    int unuse;
    unsigned int nblkaddr;
}block;
/*投影磁盘块结构体*/
typedef struct pblock{
    int items[15];
    unsigned int nblkaddr;
}pblock;
/*连接元祖*/
typedef struct jtuple
{
	int elem1;
	int elem2;
    int elem3;
}jtuple;
/*连接磁盘块*/
typedef struct jblock{
    jtuple items[5];
    unsigned int nblkaddr;
}jblock;
/*hash节点*/
typedef struct hashnode{
    tuple item;
    struct hashnode *next;
}hashnode;
/*hash桶*/
typedef struct hashbucket{
    int key;
    int num;
    int blknum;
    unsigned int first_addr;
    unsigned int end_addr;
}hashbucket;
int DataPrep(Buffer *buf,unsigned int blkaddr,int len);
/*线性搜索*/
int LinearSearch(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int value);
/*二分搜索*/
int Binary(int num, tuple *temp, int len);
/*冒泡排序*/
tuple * BubbleSort(tuple *temp, int len);
/*加载一个磁盘块数据*/
unsigned int LoadOneBlock(Buffer *buf,unsigned int addr, tuple * temp);
/*加载磁盘块数据*/
unsigned int LoadData(Buffer *buf,unsigned int blkaddr, tuple * temp,int blknum);
/*将数据写入磁盘*/
int WriteDataOrigin(Buffer *buf,unsigned int blkaddr,int num,int *temp,int len);
int TupleWriteData(Buffer *buf,unsigned int blkaddr,int len,tuple *temp);
int JTupleWriteData(Buffer *buf,unsigned int blkaddr,int len,jtuple *temp);
/*写入一块磁盘数据*/
int WriteOneBlockOrigin(Buffer *buf,unsigned int blkaddr,int num,int *temp,int len,unsigned int nblkaddr);
int TupleWriteOneBlock(Buffer *buf,unsigned int blkaddr,int tuplenum,tuple *temp,unsigned int nblkaddr);
int JtupleWriteOneBlock(Buffer *buf,unsigned int blkaddr,int jtuplenum,jtuple *temp,unsigned int nblkaddr);
/*从磁盘读取结果*/
int ReadAll(Buffer *buf,unsigned int addr,int type) ;
/*读一个磁盘块*/
unsigned int ReadOneBlock(Buffer *buf,unsigned int addr,int type);
/*二分搜索*/
int BinarySearch(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int value,int len) ;
/*投影*/
int projection(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int len);
/*Nest-Loop- Join算法*/
int NestLoopJoin(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr);
/*Sort-Merge-Join算法*/
int SortMergeJoin(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr);
/*Hash-Join算法*/
int HashJoin(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr);
/*创建hash桶*/
int CreateHashBucket(Buffer *buf,unsigned int blkaddr,int len,hashbucket *bucket,int bucketsize,unsigned int oblkaddr);
/*读取hash桶*/
void ReadBucket(Buffer *buf,hashbucket *bucket,int bucketsize);
/*删除hash桶*/
void DeleteBucket(Buffer *buf,hashbucket *bucket,int bucketsize);
/*交集*/
int IntersectionSet(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr);
/*并集*/
int UnionSet(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr) ;
/*差集*/
int DifferenceSet(Buffer *buf,unsigned int r_blkaddr,unsigned int s_blkaddr,unsigned int outblkaddr) ;
/*合并相邻子序列*/
unsigned int Merge(Buffer *buf,unsigned int laddr,unsigned int raddr);
/*外部排序*/
unsigned int ExternSort(Buffer *buf,unsigned int *tempblkaddr,int left,int right);
/*讲磁盘数据排序*/
unsigned int SortBlockData(Buffer *buf,unsigned int blkaddr,unsigned int oblkaddr,int len,int blknum);
void PrintTupleList(tuple *tlist, int len);
/*创建聚集索引*/
unsigned int CreateIndex(Buffer *buf,unsigned int blkaddr,unsigned int oblkaddr, int len);
/*索引搜索*/
int IndexSearch(Buffer *buf,unsigned int blkaddr,unsigned int indexaddr,unsigned int outblkaddr,int value,int len);
int BTreeSearch(Buffer *buf,unsigned int blkaddr,unsigned int outblkaddr,int value);
#endif