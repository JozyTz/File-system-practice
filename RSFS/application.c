/*
    application that tests the API
*/

#include "def.h"
#include <unistd.h>

struct thread_arg{
    int id;
    char *filename; 
    int sleep_time; //in second
    char *str; //content to write
};

//reader thread
void *reader_thread(void *ptr){
    struct thread_arg *arg = (struct thread_arg *)ptr;

    //open a file with RSFS_RDONLY
    int fd = RSFS_open(arg->filename,RSFS_RDONLY);
    printf("[reader %d] open file %s with READONLY; return fd=%d.\n", 
        arg->id, arg->filename, fd);
    if(fd<0){
        printf("[reader %d] fail to open the file as fd<0.\n",
            arg->id);
        return NULL;
    }    

    //print fs status
    RSFS_stat();

    //reset the position to the begining
    RSFS_fseek(fd,0,RSFS_SEEK_SET);

    //read the full content of the file
    char buf[256];
    int ret = RSFS_read(fd,buf,256);
    if(ret>0){
        printf("[reader %d] read %d bytes of string: %s\n",
            arg->id, ret, buf);
    }else{
        printf("[reader %d] fail to read anything.\n",
            arg->id);
    }


    //sleep for sleep_time
    sleep(arg->sleep_time);

    //close the file
    printf("[reader %d] close the file.\n", arg->id);
    ret = RSFS_close(fd);
    

    RSFS_stat();        
}


//writer thread
void *writer_thread(void *ptr){
    struct thread_arg *arg = (struct thread_arg *)ptr;

    //open a file with RSFS_RDONLY
    int fd = RSFS_open(arg->filename,RSFS_RDWR);
    printf("[writer %d] open file %s with RDWR; return fd=%d.\n", 
        arg->id, arg->filename, fd);
    if(fd<0){
        printf("[writer %d] fail to open the file as fd<0.\n",  arg->id);
        return NULL;
    }    

    //print fs status
    RSFS_stat();

    //append to the file
    RSFS_fseek(fd,0,RSFS_SEEK_END);
    int ret = RSFS_write(fd,arg->str,strlen(arg->str));
    if(ret>0){
        printf("[writer %d] write %d bytes of string.\n", arg->id, ret);
    }else{
        printf("[write %d] fail to write anything.\n", arg->id);
    }


    //read the whole content
    ret=RSFS_fseek(fd,0,RSFS_SEEK_SET);
    char buf[256];
    ret = RSFS_read(fd,buf,256);
    if(ret>0){
        printf("[writer %d] read %d bytes of string: %s\n", arg->id, ret, buf);
    }else{
        printf("[writer %d] fail to read anything.\n", arg->id);
    }

    RSFS_stat();

    //sleep for sleep_time
    sleep(arg->sleep_time);

    //close the file
    printf("[writer %d] close the file.\n", arg->id);
    ret = RSFS_close(fd);
    
}


void test_concurrency(){

    //create a file named "alice"
    int ret = RSFS_create("alice");
    printf("[main] result of RSFS_create('alice'): %d\n", ret);

    //write initial content to the file
    char msg_to_write[55] = "hello 1, hello 2, hello 3, hello 4, hello 5, hello 6, ";
    struct thread_arg writer_arg[2];
    writer_arg[0].id = 0;
    writer_arg[0].filename="alice";
    writer_arg[0].sleep_time=0;
    writer_arg[0].str = msg_to_write;
    writer_thread(&writer_arg[0]);

    //prepare 2 writer threads
    pthread_t writer_threads[2];
    for(int i=0; i<2; i++){
        writer_arg[i].id=i;
        writer_arg[i].filename="alice";
        writer_arg[i].sleep_time=2;
        writer_arg[i].str = msg_to_write;
    }

    //prepare 4 reader threads
    pthread_t reader_threads[4];
    struct thread_arg reader_arg[4];
    for(int i=0; i<4; i++){
        reader_arg[i].id=i;
        reader_arg[i].filename="alice";
        reader_arg[i].sleep_time=2;
    }

    //launch the threads
    pthread_create(&writer_threads[0],NULL,writer_thread,&writer_arg[0]);
    pthread_create(&reader_threads[0],NULL,reader_thread,&reader_arg[0]);
    pthread_create(&writer_threads[1],NULL,writer_thread,&writer_arg[1]);
    pthread_create(&reader_threads[1],NULL,reader_thread,&reader_arg[1]);
    pthread_create(&reader_threads[2],NULL,reader_thread,&reader_arg[2]);
    pthread_create(&reader_threads[3],NULL,reader_thread,&reader_arg[3]);

    //wait for all threads to complete
    for(int i=0; i<2; i++){
        pthread_join(writer_threads[i],NULL);
    }
    for(int i=0; i<4; i++){
        pthread_join(reader_threads[i],NULL);
    }

}


void test_basic(){

    //preparation
    char str[8][16] = {"Alice", "Bob", "Charlie", "David",
                        "Elaine", "Frank", "George", "Harry"
                    };
    
    
    //create NUM_INODES new files
    for(int i=0; i<NUM_INODES; i++){
        printf("%d\b",i);
        int ret = RSFS_create(str[i]);
        if(ret!=0){
            printf("[test_basic] fail to create file: %s.\n", str[i]);
        }
    }
    printf("[test_basic] have called to create %d files.\n", NUM_INODES);
    RSFS_stat();

    //open each file
    int fd[NUM_INODES];
    for(int i=0; i<NUM_INODES; i++){
        fd[i] = RSFS_open(str[i], RSFS_RDWR);
        if(fd[i]<0){
            printf("[test_basic] fail to open file: %s\n", str[i]);
        }
    }
    printf("[test_basic] have called to open %d files.\n", NUM_INODES);
    RSFS_stat();

    //write to each file
    for(int i=0; i<NUM_INODES; i++){
        // RSFS_fseek(fd[i],0,RSFS_SEEK_END);
        for(int j=0; j<=i; j++){
            int ret = RSFS_write(fd[i],str[i],strlen(str[i]));
        }
    }
    printf("[test_basic] have written to each file.\n");
    RSFS_stat();


    //close the files
    for(int i=0; i<NUM_INODES; i++){
        int ret=RSFS_close(fd[i]);
        if(ret!=0){
            printf("[test_basic] fail to close file: %s.\n", str[i]);
        }
    }
    printf("[test_basic] have closed each file.\n");
    RSFS_stat();

    //open each file again
    for(int i=0; i<NUM_INODES; i++){
        fd[i] = RSFS_open(str[i], RSFS_RDONLY);
    }

    printf("[test_basic] have opened each file again.\n");
    RSFS_stat();


    //read each file and then close it
    for(int i=0; i<NUM_INODES; i++){
        char buf[NUM_POINTER*BLOCK_SIZE];
        memset(buf,0,NUM_POINTER*BLOCK_SIZE);
        RSFS_fseek(fd[i],0,RSFS_SEEK_SET);
        RSFS_read(fd[i],buf,NUM_POINTER*BLOCK_SIZE);
        printf("File '%s' content: %s\n", str[i], buf);
        RSFS_close(fd[i]);
    }
    printf("\n[test_basic] have read and then closed each file.\n");
    RSFS_stat();

    //delete all files 
    for(int i=0; i<NUM_INODES; i++){
        RSFS_delete(str[i]);
    }
    printf("[test_basic] have deleted all files.\n");
    RSFS_stat();

}



//test: reader-writer problem
void main(){

    //initialize the file system
    int ret = RSFS_init();
    printf("[main] result of calling sys_init: %d\n", ret);
    if(ret!=0){
        printf("[main] fail to initialize the system; run again later1\n");
        return; 
    }

    printf("\n\n-------------------Basic Test-------------------------\n\n");
    test_basic();

    // optional: 
    printf("\n\n--------Test for Concurrent Readers/Writers-----------\n\n");
    test_concurrency();

    

}
