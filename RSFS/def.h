/*
    definitions of constants, data structures, and API
*/


#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>


//global constants
#define NUM_INODES 8 //total number of inodes
#define NUM_DBLOCKS 32 //total number of data blocks
#define NUM_POINTER 8 //total number of pointers for each inode; i.e., each file can have at most this number of data blocks
#define BLOCK_SIZE 32 //size of each data block (unit: byte)
#define NUM_OPEN_FILE 8 //maximum number of files that can be open at a time

#define RSFS_RDONLY 0 //a value for access_flag in RSFS_open(): file is open for read only
#define RSFS_RDWR 1 //a value for access_flag in RSFS_open(): file is open for read and write  

#define RSFS_SEEK_SET 0 //a value for whence in RSFS_fseek()
#define RSFS_SEEK_CUR 1 //a value for whence in RSFS_fseek()
#define RSFS_SEEK_END 2 //a value for whence in RSFS_fseek()

#define DEBUG 0 //1-enable debug, 0-disable debug prints

//directory entry
struct dir_entry{
    char *name; //file name
    int inode_number; //inode_number identifies the inode of the file
    struct dir_entry *next; //pointers to form a linked list of directory entries
    struct dir_entry *prev;
};

//root directory: it is a linked list of dir_entry (directory entries) 
struct root_dir{
    struct dir_entry *head; //pointer to the first entry of the list
    struct dir_entry *tail; //pointer to the last entry of the list
    pthread_mutex_t mutex; //mutex to guard mutual exclusive access of the list
};
extern struct root_dir root_dir; //global variable of root directory

//inode data structure
struct inode {
    int block[NUM_POINTER]; //pointers to data blocks; note: value<0 means not used
    int length; //length of the file of the inode

    //Hint - to add (for concurrent readers/writers):
    //you may need to add some more fields to support concurrent reading 
    //and exclusive writing; 
    //recall the solution of reader/writer's problem discussed in class

};
extern struct inode inodes[NUM_INODES]; //global variable of inodes
extern pthread_mutex_t inodes_mutex; //mutex to guard mutually-exclusive access of inodes

//inode bitmap
extern int inode_bitmap[NUM_INODES]; //global variable of inode bitmap
extern pthread_mutex_t inode_bitmap_mutex; //mutex to guard mutually-exclusive access of the bitmap

//data bitmap
extern int data_bitmap[NUM_DBLOCKS]; //global variable of data bitmap
extern pthread_mutex_t data_bitmap_mutex; //mutex to guard mutually-exclusive access of the bitmap

//data blocks
extern void *data_blocks[NUM_DBLOCKS]; //global variable - list of data blocks

//open file entry 
struct open_file_entry{
    int used; //0-the entry is not in use, or 1- it is in use (already allocated)
    pthread_mutex_t entry_mutex; //mutex to guard M.E. access to this entry
    struct dir_entry *dir_entry; //pointer to the directory entry of the opened file
    int position; //current position of the file
    int access_flag; //RSFS_RDONLY or RSFS_RDWR - how the file can be accessed
};
extern struct open_file_entry open_file_table[NUM_OPEN_FILE]; //global varialbe - open file table
extern pthread_mutex_t open_file_table_mutex; //mutex to guard access to the table


//routines for directory
struct dir_entry *search_dir(char *file_name); //look for the dir_entry for the given file name
struct dir_entry *insert_dir(char *file_name); //create a dir_entry for the given file name and insert it to the root directory
int delete_dir(char *file_name); //delete the dir_entry for the given file name from the global directory


//routines for inode
int allocate_inode(); //allocate an unused inode, the inode_number is returned
void free_inode(int inode_number); //free (release) an inode


//routines for data block
int allocate_data_block(); //allocate an unused data block, the block_number is returned
void free_data_block(int block_number); //free (release) a data block


//routines for open file entry
int allocate_open_file_entry(int access_flag, struct dir_entry *dir_entry); 
        //allocate an open file entry and initialize it with provided parameters
void free_open_file_entry(int fd); //free (release) an open file entry


//api
int RSFS_init();
int RSFS_create(char *file_name);
int RSFS_open(char *file_name, int access_flag);
int RSFS_read(int fd, void *buf, int size);
int RSFS_write(int fd, void *buf, int size);
int RSFS_fseek(int fd, int offset, int whence);
int RSFS_close(int fd);
int RSFS_delete(char *file_name);
void RSFS_stat();


