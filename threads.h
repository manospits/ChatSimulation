#ifndef _THREADSCH_
#define _THREADSCH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>
#include <unistd.h>
#include <openssl/md5.h>
#define MESSAGE_LENGTH 100

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };

typedef struct {
    char word[ MESSAGE_LENGTH ];
    unsigned char hash[MD5_DIGEST_LENGTH];
} message;

typedef struct{
    int flag;
} flag;

typedef struct{
    message* msg,*msg2;
    int id,id2,id3,id4,id5,id6;
    flag *TERMflag;
}args;




void *readmsg(void *param);
void *ch_read_and_send(void *param);
void *en_read_and_decode(void *param);
void *read_and_encode(void *param);
void *writemsg(void *param);

#endif
