#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <openssl/md5.h>
#include "threads.h"
#define MESSAGE_LENGTH 100
#define  SHMKEY1  1000
#define  SHMKEY2 172772
#define  SEMKEY1 131313
#define PERMS 0666
#define  SEMKEY2 131144
#define SEMKEY3 131367
#define SEMKEY4 123478
#define SEMKEY5 654321
#define SEMKEY6 343434
#define SEMKEY7 991244
#define SEMKEY8 112334
#define SEMKEY9 975464
#define SEMKEY10 764633
pthread_mutex_t mutex;

int prob_of_error=0;

struct sembuf semdown={0,-1,0};
struct sembuf semup={0,1,0};

int main(int argc,char*argv[]) {
    int sembool;
    union semun arg;
    arg.val=0;
    puts("---Chat started---");
    if((sembool=semget((key_t)SEMKEY1,1,PERMS))<0 ){
        if(argc>1){
            prob_of_error=atoi(argv[1]);
        }
        printf(" -Probabilty of error in channel: %d \n",prob_of_error);
        printf(" -Process 1\n");
        sembool=semget((key_t)SEMKEY1,1,PERMS | IPC_CREAT);
        int semids,semidr,shmids,shmidr,semiddec2c,semidw2c;
        int shmthrd;
        int shm_ch_encs,semid_enc_chans;
        int shm_en_chanr2,semid_enc_chanr,semid_enc_cr2,shm_ch_encs2,semid_ch_encs2,shm_enc_cr,semid_chanr_encs;
        int sem_for_resend,sem_for_resend2,sem_ch2c,sem_ch2c2,sem_resend,sem_resend2,semid_resend_ch2,sem_resend_dec;
        message *msg2s,*msg2rcv,*msg_enc_chans,*msg_ch_enr2,*msg_ch_encs2,*msg_ch_enr;
        flag *tempflag;
        pthread_t id1,id2;
        pthread_attr_t attr;
        pthread_mutex_init (&mutex,NULL);
        int sem1;
        //SHARED MEMORY
        if((shmids = shmget ((key_t)IPC_PRIVATE , sizeof(message), PERMS| IPC_CREAT))==-1) return -1;
        if((shmidr = shmget ((key_t)IPC_PRIVATE , sizeof(message), PERMS| IPC_CREAT))==-1) return -1;
        if((shmthrd = shmget((key_t)IPC_PRIVATE , sizeof(flag), PERMS | IPC_CREAT))==-1) return -1;
        //enc1-chan
        if((shm_ch_encs = shmget((key_t)IPC_PRIVATE,sizeof(message),PERMS | IPC_CREAT))==-1) return -1;
        tempflag=(flag*)shmat(shmthrd,0,0);
        //chan-enc2
        if((semid_resend_ch2=semget((key_t)SEMKEY10 , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((shm_en_chanr2 = shmget((key_t)SHMKEY2,sizeof(message),PERMS | IPC_CREAT))==-1) return -1;
        if((msg_ch_enr2=(message*) shmat(shm_en_chanr2,0,0))== (void *) (-1)) return -1;
        if((shm_ch_encs2 = shmget((key_t)SHMKEY1,sizeof(message),PERMS | IPC_CREAT))==-1) return -1;
        if((msg_ch_encs2 = (message *) shmat(shm_ch_encs2,0,0))== (void *) (-1)) return -1;
        tempflag->flag=0;
        //chan-enc1
        if((shm_enc_cr=shmget((key_t)IPC_PRIVATE, sizeof(message), PERMS | IPC_CREAT))==-1) return -1;
        if((msg_ch_enr=(message*)shmat(shm_enc_cr,0,0))==(void*)(-1)) return -1;
        /* --------------------------------------------------------------------------------- */
        if((msg2s=(message *)shmat(shmids,0,0))== (void*)(-1)) return -1;
        if((msg2rcv=(message *)shmat(shmidr,0,0)) == (void*)(-1)) return -1;
        if((msg_enc_chans=(message *)shmat(shm_ch_encs,0,0))==(void *)(-1)) return -1;
        /* SEMAPHORES */
        //p1-enc
        if((semids=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semidr=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semiddec2c=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semidw2c=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((sem_resend_dec=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((sem1=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        //enc-chani
        if((sem_resend=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((sem_resend2=semget((key_t)SEMKEY9 , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((sem_for_resend=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((sem_for_resend2=semget((key_t)SEMKEY6 , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semid_enc_chans=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semid_enc_chanr=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semid_chanr_encs=semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        //chan-enc2
        if((sem_ch2c = semget((key_t)SEMKEY8, 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((sem_ch2c2 = semget((key_t)SEMKEY7, 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semid_ch_encs2 = semget((key_t)SEMKEY3, 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semid_enc_cr2=semget((key_t)SEMKEY2, 1, PERMS|IPC_CREAT))==-1) return -1;
        union semun arg,arg2;
        arg.val=1;
        arg2.val=0;
        semctl(sem_resend_dec,0,SETVAL,arg2);
        semctl(semid_resend_ch2,0,SETVAL,arg2);
        semctl(sem_resend,0,SETVAL,arg2);
        semctl(sem_resend2,0,SETVAL,arg2);
        semctl(semiddec2c,0,SETVAL,arg2);
        semctl(semidw2c,0,SETVAL,arg2);
        semctl(semid_ch_encs2,0,SETVAL,arg);
        semctl(semidr,0,SETVAL,arg);
        semctl(semids,0,SETVAL,arg);
        semctl(sem1,0,SETVAL,arg2);
        semctl(sem_for_resend,0,SETVAL,arg2);
        semctl(sem_for_resend2,0,SETVAL,arg2);
        semctl(semid_enc_chans,0,SETVAL,arg);
        semctl(sem_ch2c,0,SETVAL,arg2);
        semctl(sem_ch2c2,0,SETVAL,arg2);
        semctl(semid_chanr_encs,0,SETVAL,arg);
        semctl(semid_enc_chanr,0,SETVAL,arg);
        semctl(semid_enc_cr2,0,SETVAL,arg);
        pid_t encpid,chanpid;
        int status;
        if((encpid=fork())!=0){
            /* PTHREADS */
            //P1
            args arguments,arguments2;
            arguments.msg=msg2rcv;
            arguments.id=semidr;
            arguments.id2=semiddec2c;
            arguments.TERMflag=tempflag;
            arguments2.id3=sem1;
            pthread_attr_init(&attr);
            arguments2.msg=msg2s;
            arguments2.id=semids;
            arguments2.id2=semidw2c;
            arguments2.TERMflag=tempflag;
            pthread_create(&id2,&attr,writemsg,&arguments2);
            pthread_create(&id1,&attr,readmsg,&arguments);
            pthread_join(id1,NULL);
            pthread_join(id2,NULL);
            waitpid(encpid, &status,WUNTRACED);
        }
        else{
            if((chanpid=fork())!=0){
                //ENC
                args arguments,arguments2;
                arguments.msg=msg2s;
                arguments.msg2=msg_enc_chans;
                arguments.id=semids;
                arguments.TERMflag=tempflag;
                arguments.id2=semid_enc_chans;
                arguments.id3=sem_for_resend;
                arguments.id4=semidw2c;
                arguments.id5=sem_resend;
                arguments.id6=sem1;
                pthread_attr_init(&attr);
                pthread_create(&id1,&attr,read_and_encode,&arguments);
                arguments2.msg2=msg2rcv;
                arguments2.msg=msg_ch_enr;
                arguments2.id=semid_enc_chanr;
                arguments2.id2=semidr;
                arguments2.TERMflag=tempflag;
                arguments2.id3=sem_ch2c2;
                arguments2.id4=semiddec2c;
                arguments2.id5=sem_resend_dec;
                pthread_create(&id2,&attr,en_read_and_decode,&arguments2);
                pthread_join(id1,NULL);
                pthread_join(id2,NULL);
                waitpid(chanpid, &status, WUNTRACED);
                exit(0);
            }
            else {
                //CHAN
                args arguments,arguments2;
                pthread_attr_init(&attr);
                arguments.msg=msg_enc_chans;
                arguments.msg2=msg_ch_enr2;
                arguments.id=semid_enc_chans;
                arguments.id2=semid_enc_cr2;
                arguments.id3=sem_for_resend;
                arguments.TERMflag=tempflag;
                arguments.id4=sem_ch2c;
                arguments.id5=sem_resend2;
                arguments.id6=sem_resend;
                arguments2.msg2=msg_ch_enr;
                arguments2.msg=msg_ch_encs2;
                arguments2.id2=semid_enc_chanr;
                arguments2.TERMflag=tempflag;
                arguments2.id=semid_ch_encs2;
                arguments2.id3=sem_for_resend2;
                arguments2.id4=sem_ch2c2;
                arguments2.id5=sem_resend_dec;
                arguments2.id6=semid_resend_ch2;
                pthread_create(&id1,&attr,ch_read_and_send,&arguments);
                pthread_create(&id2,&attr,ch_read_and_send,&arguments2);
                pthread_join(id2,NULL);
                pthread_join(id1,NULL);
                exit(0);
            }
        }
        semctl(semid_ch_encs2,0,IPC_RMID);
        semctl(semidr,0,IPC_RMID);
        semctl(semids,0,IPC_RMID);
        semctl(semidw2c,0,IPC_RMID);
        semctl(semiddec2c,0,IPC_RMID);
        semctl(sem1,0,IPC_RMID);
        semctl(semid_enc_chans,0,IPC_RMID);
        semctl(semid_chanr_encs,0,IPC_RMID);
        semctl(sem_for_resend , 0 , IPC_RMID);
        semctl(sem_resend2 , 0 , IPC_RMID);
        semctl(semid_resend_ch2 , 0 , IPC_RMID);
        semctl(sem_resend , 0 , IPC_RMID);
        semctl(sem_resend_dec , 0 , IPC_RMID);
        semctl(sem_for_resend2 , 0 , IPC_RMID);
        semctl(sem_ch2c , 0 , IPC_RMID);
        semctl(sem_ch2c2 , 0 , IPC_RMID);
        semctl(semid_enc_chanr,0,IPC_RMID);
        semctl(semid_enc_cr2,0,IPC_RMID);
        shmctl(shmidr,0,IPC_RMID);
        shmctl(shmids,0,IPC_RMID);
        shmctl(shm_ch_encs ,0,IPC_RMID);
        shmctl(shm_enc_cr,0,IPC_RMID);
        shmctl(shm_en_chanr2,0,IPC_RMID);
        shmctl(shm_ch_encs2 ,0,IPC_RMID);
        shmctl(shmthrd,0,IPC_RMID);
    }
    else {
        printf(" -Process 2\n" );
        int semids,semidr,shmids,shmidr,semidw2c,semiddec2c;
        int shmthrd;
        int shm_ch_encs,semid_enc_chans,sem_for_resend2,sem_ch2c,sem_resend2,semid_resend_ch2;
        int shm_en_chanr2,semid_enc_chanr;
        message *msg2s,*msg2rcv,*msg_enc_chans,*msg_ch_enr2;
        flag *tempflag;
        pthread_t id1,id2;
        pthread_attr_t attr;
        pthread_mutex_init (&mutex,NULL);
        int sem1;
        //SHARED MEMORY
        if((shmids = shmget ((key_t)IPC_PRIVATE , sizeof(message), PERMS| IPC_CREAT))==-1) return -1;
        if(( shmidr = shmget ((key_t)IPC_PRIVATE , sizeof(message), PERMS| IPC_CREAT))==-1) return -1;
        if((shmthrd = shmget((key_t)IPC_PRIVATE,sizeof(flag), PERMS | IPC_CREAT))==-1) return -1;
        //enc2-chan
        if((shm_ch_encs = shmget((key_t)SHMKEY1,sizeof(message),PERMS | IPC_CREAT))==-1) return -1;
        if((tempflag=(flag*)shmat(shmthrd,0,0))==(void *)-1) return -1;
        //chan-enc2
        if((shm_en_chanr2 = shmget((key_t)SHMKEY2,sizeof(message),PERMS | IPC_CREAT))==-1) return -1;
        if((msg_ch_enr2=(message*) shmat(shm_en_chanr2,0,0))==(void *)-1) return -1;
        tempflag->flag=0;
        if((msg2s=(message *)shmat(shmids,0,0))==(void *)-1) return -1;
        if((msg2rcv=(message *)shmat(shmidr,0,0))==(void *)-1) return -1;
        if((msg_ch_enr2=(message*) shmat(shm_en_chanr2,0,0))==(void *)-1) return -1;
        if((msg_enc_chans=(message *)shmat(shm_ch_encs,0,0))==(void *)-1) return -1;
        strcpy(msg2rcv->word,"");
        /* SEMAPHORES */
        //p2-enc
        if((semiddec2c = semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semidw2c = semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semids = semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((semidr = semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        if((sem1 = semget((key_t)IPC_PRIVATE , 1 , PERMS | IPC_CREAT))==-1) return -1;
        //enc-chan
        if((semid_resend_ch2=semget((key_t)SEMKEY10 , 1 , PERMS ))==-1) return -1;
        if((sem_resend2=semget((key_t)SEMKEY9 , 1 , PERMS ))==-1) return -1;
        if((sem_for_resend2 = semget((key_t)SEMKEY6 , 1 , PERMS))==-1) return -1;
        if((sem_ch2c = semget((key_t)SEMKEY8, 1 , PERMS ))==-1) return -1;
        if((semid_enc_chans = semget((key_t)SEMKEY3 , 1 , PERMS ))==-1) return -1;
        if((semid_enc_chanr = semget((key_t)SEMKEY2 , 1 , PERMS ))==-1) return -1;
        union semun arg,arg2;
        arg.val=1;
        arg2.val=0;
        semctl(semiddec2c,0,SETVAL,arg2);
        semctl(semidw2c,0,SETVAL,arg2);
        semctl(semidr,0,SETVAL,arg);
        semctl(semids,0,SETVAL,arg);
        semctl(sem1,0,SETVAL,arg2);
        semctl(semid_enc_chans,0,SETVAL,arg);
        pid_t encpid;
        int status;
        if((encpid=fork())!=0){
            //P2
            /* PTHREADS */
            args arguments,arguments2;
            pthread_attr_init(&attr);
            arguments2.msg=msg2s;
            arguments2.id=semids;
            arguments2.id2=semidw2c;
            arguments2.TERMflag=tempflag;
            arguments2.id3=sem1;
            pthread_create(&id2,&attr,writemsg,&arguments2);
            arguments.msg=msg2rcv;
            arguments.id=semidr;
            arguments.id2=semiddec2c;
            arguments.TERMflag=tempflag;
            pthread_create(&id1,&attr,readmsg,&arguments);
            pthread_join(id1,NULL);
            pthread_join(id2,NULL);
            waitpid(encpid, &status, WUNTRACED);
        }
        else{
            //ENC2
            args arguments,arguments2;
            pthread_attr_init(&attr);
            arguments2.msg2=msg2rcv;
            arguments2.id2=semidr;
            arguments2.msg=msg_ch_enr2;
            arguments2.id=semid_enc_chanr;
            arguments2.TERMflag=tempflag;
            arguments2.id3=sem_ch2c;
            arguments2.id4=semiddec2c;
            arguments2.id5=sem_resend2;
            arguments.msg=msg2s;
            arguments.msg2=msg_enc_chans;
            arguments.id=semids;
            arguments.TERMflag=tempflag;
            arguments.id2=semid_enc_chans;
            arguments.id3=sem_for_resend2;
            arguments.id4=semidw2c;
            arguments.id5=semid_resend_ch2;
            arguments.id6=sem1;
            pthread_create(&id1,&attr,read_and_encode,&arguments);
            pthread_create(&id2,&attr,en_read_and_decode,&arguments2);
            pthread_join(id1,NULL);
            pthread_join(id2,NULL);
            exit(0);
        }
        semctl(semidw2c,0,IPC_RMID);
        semctl(semiddec2c,0,IPC_RMID);
        semctl(semidr,0,IPC_RMID);
        semctl(semids,0,IPC_RMID);
        semctl(sem1,0,IPC_RMID);
        semctl(semid_enc_chans,0,IPC_RMID);
        shmctl(shmidr,0,IPC_RMID);
        shmctl(shmids,0,IPC_RMID);
        shmctl(shmthrd,0,IPC_RMID);
    }
    semctl(sembool,0,IPC_RMID);
    return 0;
}

