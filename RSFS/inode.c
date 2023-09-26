/*
    global variables inodes, inode bitmap, and mutexes to guard them;
    routines for inode
*/

#include "def.h"


struct inode inodes[NUM_INODES];
pthread_mutex_t inodes_mutex;

int inode_bitmap[NUM_INODES];
pthread_mutex_t inode_bitmap_mutex;


//to allocate an empty inode and return the inode-number; 
//if no free inode is available, return -1
int allocate_inode(){

    int inode_number=-1; //init 

    pthread_mutex_lock(&inode_bitmap_mutex);

    for(int i=0; i<NUM_INODES; i++){
        if(inode_bitmap[i]==0){//find an available inode
            
            inode_number=i;
            inode_bitmap[i]=1; //mark it as allocated
            
            //initialize the inode
            inodes[i].length=0;
            for(int j=0; j<NUM_POINTER; j++) inodes[i].block[j]=-1;
            
            break;
        }
    }

    pthread_mutex_unlock(&inode_bitmap_mutex);

    return inode_number;
}

//to free an inode with provided inode_number
void free_inode(int inode_number){

    pthread_mutex_lock(&inode_bitmap_mutex);
    
    inode_bitmap[inode_number]=0; //mark it as available
    
    pthread_mutex_unlock(&inode_bitmap_mutex);
}

