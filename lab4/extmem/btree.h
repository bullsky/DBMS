#ifndef BTREE_H
#define BTREE_H

// #define  m  3   // B树的阶，此设为4
// #define KeyType tuple  
// #define Status  int 
#define rank 6 //B树的阶,最大为6
// #define root_blkaddr 8000 //b树根的地址
// #define max_btnode_addr 8000
// #define maxaddr 9000
//6字节
typedef struct Addr{
    unsigned int blkaddr;
    short offset;
}Addr;
//64字节
typedef struct BTBlock{
    Addr addr[10];
    unsigned int nblkaddr;
}BTBlock;
//8字节
typedef struct keynode{
    int elem;   //关键字
    unsigned int blkaddr;//存储关键字匹配的地址的磁盘块地址
}keynode;
//64字节
typedef struct BTNode{
    int keynum; //最大为6
    unsigned int parent;  // 指向双亲结点的磁盘块地址
    keynode key[7]; // 关键字向量，0号单元未用
    unsigned int childs[7];   // 子树指针磁盘块地址
}BTNode;

typedef struct Result{
    unsigned int blkaddr;//结果所在磁盘地址
    short index;   //所在磁盘块中第几个元素
    int flag;  //是否找到:0没有，1yes
}Result;
// typedef struct BTnode{
//     int keynum;        // 结点中关键字个数，即结点的大小
//     struct BTNode *parent;        // 指向双亲结点
//     KeyType key[m + 1];      // 关键字向量，0号单元未用
//     struct BTNode *ptr[m + 1];      // 子树指针向量
// }BTNode,*BTree;

// typedef struct  {  
//     BTNode * pt;  
//     int i;  
//     int tag;  
// }Result;  
int avail_btnode_addr = 8000;
int avail_addr = 9000;
int root_addr = 8000;
/*写入一块磁盘到b树*/
int BTreeWriteOneBlock(Buffer *buf,unsigned int blkaddr,BTNode *temp);
/*加载一个磁盘数据到b树节点*/
void BTreeLoadOneBlock(Buffer *buf,unsigned int blkaddr, BTNode *btnode);

Result SearchBTree(Buffer *buf,unsigned int blkaddr,int key);

unsigned int CreateBTree(Buffer *buf,unsigned int blkaddr);
/*在树t中插入关键字key*/
int InsertBTree(Buffer *buf,unsigned int blkaddr,short offset,keynode key);
/*新建根节点*/
int NewRoot(Buffer *buf, unsigned int blkaddr,keynode key,unsigned int child1,unsigned int child2);
/*将结点分裂成两个结点,前一半保留,后一半移入结点*/
keynode SplitBTNode(Buffer *buf, unsigned int blkaddr,unsigned int split_addr);
/*插入关键字的地址节点*/
int InsertKeyAddr(Buffer *buf,unsigned int blkaddr,Addr *keyaddr,int *MaxAddr);
/*在一个b树节点里插入*/
int InsertBTNode(Buffer *buf,unsigned int blkaddr,short offset,keynode key,unsigned int lchild_addr,unsigned int rchild_addr);
int SearchBTNode(Buffer *buf,unsigned int blkaddr,int key);
/*加载一个磁盘数据到addr*/
unsigned int AddrLoadOneBlock(Buffer *buf,unsigned int blkaddr,Addr *addrlist);
/*写到磁盘*/
int AddrWriteBlock(Buffer *buf,unsigned int blkaddr,Addr *addrlist);


#endif