#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "btree.h"
#include <assert.h>
#include "header.h"
// #include "myfunc.c"
// #include "extmem.c"
/*写入一块磁盘到b树*/
int BTreeWriteOneBlock(Buffer *buf,unsigned int blkaddr,BTNode *temp)
{
    unsigned char *outblk;
    outblk = getNewBlockInBuffer(buf);
    memcpy(outblk,temp,64);
    writeBlockToDisk(outblk, blkaddr, buf);
    freeBlockInBuffer(outblk,buf);
    printf("write block at addr:%d\n",blkaddr);
    return 0;
}
/*写到磁盘*/
int AddrWriteBlock(Buffer *buf,unsigned int blkaddr,Addr *addrlist)
{
    unsigned char *outblk;
    outblk = getNewBlockInBuffer(buf);
    memcpy(outblk,addrlist,64);
    writeBlockToDisk(outblk, blkaddr, buf);
    freeBlockInBuffer(outblk,buf);
    printf("write block at addr:%d\n",blkaddr);
    return 0;    
    return 0;
}
/*加载一个磁盘数据到addr*/
unsigned int AddrLoadOneBlock(Buffer *buf,unsigned int blkaddr,Addr *addrlist)
{
    unsigned char *blk;
    unsigned int addr;
    blk = readBlockFromDisk(blkaddr, buf);
    addr = blkaddr;
    printf("load block %d\n",addr);
    BTBlock *block;
    block = (BTBlock *)(blk);
    for(int i=0;i<10;i++)
    {
        addrlist[i] = block->addr[i];
        // memset(addrlist+i,&block->addr[i],6);
    }
    // memset(&addr,blk+60,4);
    addr = block->nblkaddr;
    freeBlockInBuffer(blk,buf);
    return addr;
}
/*加载一个磁盘数据到b树节点*/
void BTreeLoadOneBlock(Buffer *buf,unsigned int blkaddr, BTNode *btnode)
{
    unsigned char *blk;
    BTNode *read;
    blk = readBlockFromDisk(blkaddr, buf);
    read = (BTNode *)(blk);
    printf("load block %d\n",blkaddr);
    btnode->keynum = read->keynum;
    btnode->parent = read->parent;
    for(int i=0;i<7;i++)
    {
        btnode->key[i] = read->key[i];
        btnode->childs[i] = read->childs[i];
    }
    freeBlockInBuffer(blk,buf);     
}
int SearchBTNode(Buffer *buf,unsigned int blkaddr,int key)
{
    int i=0;
    BTNode btnode;
    memset(&btnode,0,64);
    BTreeLoadOneBlock(buf,blkaddr,&btnode);
    for(i=0;i<btnode.keynum;i++)
    {
        if(key <= btnode.key[i].elem)
        {
            return i;
        }
    }
    return -1;
}
/*搜索b树，找到则返回磁盘块地址以及偏移量，没找到则返回要插入的磁盘地址*/
Result SearchBTree(Buffer *buf,unsigned int blkaddr,int key)
{
    int found = -1;
    int flag = 0;
    unsigned int addr,paddr;
    addr = blkaddr;
    BTNode btnode;
    int i;
    while(addr != 0)
    {
        memset(&btnode,0,64);
        BTreeLoadOneBlock(buf,addr,&btnode);
        for(i=0;i<btnode.keynum;i++)
        {
            if(key <= btnode.key[i].elem)
            {
                found = i;
                // break;
            }
        }
        if(key == btnode.key[i].elem)
        {
            flag = 1;
            break;
        } else {
            paddr = addr;
            if(i == btnode.keynum -1 && key > btnode.key[i].elem)
            {
                addr = btnode.childs[i+1];
            } 
            else
            {
                addr = btnode.childs[i];
            }
        }
    }
    Result result;
    result.index = found;
    if(flag == 1) //找到给出元祖地址起始磁盘地址
    {
        result.blkaddr = btnode.key[found].blkaddr;
        result.flag = 1;
    }
    else //没找到，给出当前b树节点磁盘地址
    { 
        result.flag = 0;
        result.blkaddr = paddr;
    }
    return result;
}

/*在一个b树节点里插入*/
int InsertBTNode(Buffer *buf,unsigned int blkaddr,short offset,keynode key,unsigned int lchild_addr,unsigned int rchild_addr)
{
    BTNode btnode;
    BTreeLoadOneBlock(buf,blkaddr,&btnode);
    for(int i=btnode.keynum ;i>=offset;i--)
    {
        btnode.key[i+1] = btnode.key[i];
        btnode.childs[i+1] = btnode.childs[i];
    }
    btnode.key[offset] = key;
    btnode.childs[offset] = lchild_addr;
    btnode.childs[offset+1] = rchild_addr;
    btnode.keynum++;
    BTreeWriteOneBlock(buf,blkaddr,&btnode);
    return btnode.keynum;
    // if(child_addr != 0)
    // {
    //     memset(&btnode,0,64);
    //     btnode.parent = blkaddr;
    //     BTreeWriteOneBlock(buf,child_addr,&btnode);
    // }
    return 0;
}
/*插入关键字的地址节点*/
int InsertKeyAddr(Buffer *buf,unsigned int blkaddr,Addr *keyaddr,int *MaxAddr)
{
    unsigned int addr,naddr;
    addr = blkaddr;
    int flag = 0;
    while(addr != 0)
    {
        int count = 0;
        Addr addrlist[10];
        memset(addrlist,0,60);
        naddr = AddrLoadOneBlock(buf,addr,addrlist);
        for(int i=0;i<10&&addrlist[i].blkaddr!=0;i++)
        {
            if((addrlist[i].blkaddr == keyaddr->blkaddr ) && (addrlist[i].offset == keyaddr->offset))
            {
                flag = 1;
                break;
            }
            count++;
        }
        if(flag == 1)
        {
            break;
        }
        if(naddr == 0)
        {
            if(count < 10)
            {
                addrlist[count].blkaddr = keyaddr->blkaddr;
                addrlist[count].offset = keyaddr->offset;
                AddrWriteBlock(buf,addr,addrlist);
            } 
            else
            {
                memset(addrlist,0,60);
                addrlist[0].blkaddr = keyaddr->blkaddr;
                addrlist[0].offset = keyaddr->offset;  
                unsigned int max_addr;
                max_addr = *(MaxAddr) +1 ;
                *(MaxAddr) = max_addr;
                AddrWriteBlock(buf,max_addr,addrlist);
            }
            
        }
        addr = naddr;
    }
    return 0;
}
/*将结点分裂成两个结点,前一半保留,后一半移入结点*/
keynode SplitBTNode(Buffer *buf, unsigned int blkaddr,unsigned int split_addr)
{
    BTNode btnode;
    BTNode father;
    BTNode split_node;
    keynode key;
    BTreeLoadOneBlock(buf,blkaddr,&btnode);
    BTreeLoadOneBlock(buf,btnode.parent,&father);
    int mid = (rank - 1)/2;
    key = btnode.key[mid];
    for(int i=mid+1;i<=btnode.keynum;i++)
    {
        split_node.key[i-mid-1] = btnode.key[i];
        split_node.childs[i-mid-1] = btnode.childs[i];
    }

    split_node.keynum = btnode.keynum - mid  -1;
    split_node.parent = btnode.parent;
    btnode.keynum = mid;
    BTreeWriteOneBlock(buf,blkaddr,&btnode);
    BTreeWriteOneBlock(buf,split_addr,&split_node);
    for(int i=0;i<split_node.keynum;i++)
    {
        memset(&btnode,0,64);
        BTreeLoadOneBlock(buf,split_node.childs[i],&btnode);
        btnode.parent = split_addr;
        BTreeWriteOneBlock(buf,split_node.childs[i],&btnode);
    }    

    return key;
    // int s = (rank - 1)/2;
    // split_node.childs[0] = btnode.childs[s];
    // for(int i=s+1;i<btnode.keynum;i++)
    // {
    //     split_node.key[i-s] = btnode.key[i];
    //     split_node.childs[i-s] = btnode.childs[i];
    // }
    // btnode.keynum = s;
    // BTreeWriteOneBlock(buf,blkaddr,&btnode);
    // split_node.keynum = btnode.keynum - s;
    // split_node.parent = btnode.parent;
    // BTreeWriteOneBlock(buf,split_addr,&split_node);
    // for(int i=0;i<split_node.keynum;i++)
    // {
    //     memset(&btnode,0,64);
    //     BTreeLoadOneBlock(buf,split_node.childs[i],&btnode);
    //     btnode.parent = split_addr;
    //     BTreeWriteOneBlock(buf,split_node.childs[i],&btnode);
    // }

}
/*新建根节点*/
int NewRoot(Buffer *buf, unsigned int blkaddr,keynode key,unsigned int child1,unsigned int child2)
{
    BTNode btnode;
    memset(&btnode,0,64);
    btnode.keynum = 1;
    btnode.key[0] = key;
    btnode.childs[0] = child1;
    btnode.childs[1] = child2;
    BTreeWriteOneBlock(buf,blkaddr,&btnode);
    // memset(&btnode,0,64);
    // btnode.parent = blkaddr;
    // BTreeWriteOneBlock(buf,child1,&btnode);
    // memset(&btnode,0,64);
    // btnode.parent = blkaddr;
    // BTreeWriteOneBlock(buf,child2,&btnode);
    return 0;
}

/*在树t中插入关键字key*/
int InsertBTree(Buffer *buf,unsigned int blkaddr,short offset,keynode key)
{
    // BTNode btnode;
    int finish = 0;
    int newroot = 0;
    unsigned int addr,split_addr;
    addr = blkaddr;
    unsigned int lchild_addr = 0;
    unsigned int rchild_addr = 0;
    int index = offset;
    keynode knode = key;
    while((finish == 0) && (newroot == 0))
    {
        split_addr = avail_btnode_addr;
        int key_num = InsertBTNode(buf,addr,index,knode,lchild_addr,rchild_addr);
        if(key_num < rank)
        {
            finish = 1;
        } 
        else
        {
            knode = SplitBTNode(buf,addr,split_addr);
            avail_btnode_addr++;
            lchild_addr = addr;
            rchild_addr = split_addr;
            BTNode temp;
            BTreeLoadOneBlock(buf,addr,&temp);
            if(temp.parent!=0)
            {
                addr = temp.parent;
                index = SearchBTNode(buf,temp.parent,knode.elem);
            } 
            else
            {
                newroot = 1;
            }
        }
    }
    if(newroot == 1)
    {
        NewRoot(buf,avail_btnode_addr,knode,lchild_addr,rchild_addr);
        avail_btnode_addr++;
        root_addr = avail_btnode_addr;
    }
    return 0;
}

unsigned int CreateBTree(Buffer *buf,unsigned int blkaddr)
{
    unsigned int addr,naddr;
    int count = 0;

    while(addr != 0)    
    {
        tuple temp[7];
        memset(temp,0,56);
        naddr = LoadData(buf,addr,temp,1);
        for(int i=0;i<7;i++)
        {
            keynode key;
            key.elem = temp[i].elem1;
            key.blkaddr = avail_addr;
            avail_addr++;
            if(count == 0)
            {
                root_addr = avail_btnode_addr;
                NewRoot(buf,root_addr,key,0,0);
                avail_btnode_addr++;
                count++;
            } 
            else
            {
                Result result = SearchBTree(buf,root_addr,key.elem);
                if(result.flag == 1)
                {
                    Addr tmpaddr;
                    tmpaddr.blkaddr = addr;
                    tmpaddr.offset = i;
                    InsertKeyAddr(buf,result.blkaddr,&tmpaddr,&avail_addr);
                } 
                else
                {
                    InsertBTree(buf,result.blkaddr,result.index,key);
                }
            }
        }
        addr = naddr;
    }
    return root_addr;
}