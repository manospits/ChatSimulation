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
#include "threads.h"

extern pthread_mutex_t mutex;
extern int prob_of_error;
extern struct sembuf semdown;
extern struct sembuf semup;

void *readmsg(void *param){//process p1/p2 read thread (prints the messages)
    /* printf("Read message created\n"); */
    args *arguments = param;
    message *msg2rcv=arguments->msg;
    flag* thisflag=arguments->TERMflag;
    int semid=arguments->id;
    int semid2=arguments->id2;
    char temp[ MESSAGE_LENGTH ];
    while(1){
        semop(semid2,&semdown,1);//blocked until enc(decoder thread) sends message
        semop(semid,&semdown,1);//access shared memory between enc(decoder thread) and read thread
        strcpy(temp,msg2rcv->word);
        semop(semid,&semup,1);
        printf("\n------Message received------\n%s------END_OF_MESSAGE------\n",temp );
        pthread_mutex_lock(&mutex);
        if(thisflag->flag==1){
            pthread_mutex_unlock(&mutex);
            break;
        }
        else if(strcmp("TERM\n",temp)==0){
            thisflag->flag=1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    printf("READ EXITED \n");
    pthread_exit(0);
}

void *ch_read_and_send(void *param){
    int p;
    args *arguments = param;
    /* printf("channel created\n"); */
    message *msg2rcv=arguments->msg;
    message *msg2s=arguments->msg2;
    flag* thisflag=arguments->TERMflag;
    srand(time(NULL));
    int semid=arguments->id;
    int semid2=arguments->id2;
    int semid3=arguments->id3;
    int semid4=arguments->id4;
    int semid5=arguments->id5;
    int semid6=arguments->id6;
    char temp[ MESSAGE_LENGTH ];
    unsigned char hash[MD5_DIGEST_LENGTH];
    while(1){
        semop(semid3,&semdown,1);//blocked until enc sends message
        semop(semid,&semdown,1);//access memory between chan-enc
        strcpy(temp,msg2rcv->word);
        memcpy(hash,msg2rcv->hash,MD5_DIGEST_LENGTH);
        semop(semid,&semup,1);
        /* printf("Message in channel : %s",temp ); */
        if(prob_of_error!=0 && strcmp (temp,"TERM\n")!=0){//change the message
            p=rand()%100;
            if(p<prob_of_error){
                strcpy(temp,"%#$6#$$#$");
                /* printf("Error in channel\n"); */
            }
        }
        semop(semid2,&semdown,1);//access memmory between chan and enc(decoder thread)
        strcpy(msg2s->word,temp);
        memcpy(msg2s->hash,hash,MD5_DIGEST_LENGTH);
        semop(semid2,&semup,1);
        semop(semid4,&semup,1);//decoder can continue
        pthread_mutex_lock(&mutex);
        if(thisflag->flag==1){
            pthread_mutex_unlock(&mutex);
            break;
        }
        else if(strcmp("TERM\n",temp)==0){
            thisflag->flag=1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        semop(semid5,&semdown,1);//blocked until decoder sends ack/nack
        semop(semid2,&semdown,1);
        strcpy(temp,msg2s->word);
        semop(semid2,&semup,1);
        semop(semid,&semdown,1);
        strcpy(msg2rcv->word,temp);
        semop(semid,&semup,1);
        semop(semid6,&semup,1);//encoder can now check if message is nack/ack
    }
    printf("CH_READ_SEND EXITED \n");
    pthread_exit(0);
}


void *en_read_and_decode(void *param){
    /* printf("decoder created\n"); */
    args *arguments = param;
    message *msg2rcv=arguments->msg;
    message *msg2s=arguments->msg2;
    flag* thisflag=arguments->TERMflag;
    int semid=arguments->id;
    char temp[ MESSAGE_LENGTH ];
    int semid2=arguments->id2;
    int semid3=arguments->id3;
    int semid4=arguments->id4;
    int semid5=arguments->id5;
    unsigned char hash[MD5_DIGEST_LENGTH];
    char mdString1[33];
    char mdString2[33];
    int i;
    while(1){
        semop(semid3,&semdown,1);//cant continue until chan sends a message
        semop(semid,&semdown,1);//accessing memory between chan-enc(decoder thread)
        strcpy(temp,msg2rcv->word);
        for(i=0;i<16;i++)
            sprintf(&mdString2[i*2],"%02x",(unsigned int)msg2rcv->hash[i]);
        semop(semid,&semup,1);
        MD5((unsigned char *)&msg2rcv->word,strlen(temp),(unsigned char*)&(hash));
        for(i=0;i<16;i++)
            sprintf(&mdString1[i*2],"%02x",(unsigned int)hash[i]);
        /* printf("%s\n%s, %s\n",mdString1,mdString2,temp); */
        if(strcmp(mdString1,mdString2)==0 || strcmp(temp,"TERM\n")==0){//checking if message is correct
            printf("DEC: Message read succesfully.\n");
            semop(semid2,&semdown,1);//accessing memory between decoder and procces1/2 (read thread);
            strcpy(msg2s->word,temp);
            semop(semid2,&semup,1);
            pthread_mutex_lock(&mutex);
            if(thisflag->flag==1){
                pthread_mutex_unlock(&mutex);
                semop(semid4,&semup,1);
                break;
            }
            else if(strcmp("TERM\n",temp)==0){
                thisflag->flag=1;
                pthread_mutex_unlock(&mutex);
                semop(semid4,&semup,1);
                break;
            }
            pthread_mutex_unlock(&mutex);
            semop(semid4,&semup,1);//read thread can now continue read the message
            semop(semid,&semdown,1);//sending in chan ack
            strcpy(msg2rcv->word,"ACK");
            semop(semid,&semup,1);
            semop(semid5,&semup,1);//chan can now read if message is ack/nack
        }
        else{
            printf("DEC: Error found in message checksum requesting resend...\n");
            semop(semid,&semdown,1);//send nack
            strcpy(msg2rcv->word,"NACK");
            semop(semid,&semup,1);
            semop(semid5,&semup,1);//chan can now read if message is ack/nack
        }
    }
    printf("READ_DECODE EXITED \n");
    pthread_exit(0);
}

void *read_and_encode(void *param){
    /* printf("encoder created\n"); */
    args *arguments = param;
    message *msg2rcv=arguments->msg;
    message *encodedmessage=arguments->msg2;
    flag* thisflag=arguments->TERMflag;
    int semid=arguments->id;
    int semid2=arguments->id2;
    int semid3=arguments->id3;
    int semid4=arguments->id4;
    int semid5=arguments->id5;
    int semid6=arguments->id6;
    char temp[MESSAGE_LENGTH];
    char buffer[MESSAGE_LENGTH];
    while(1){
        semop(semid4,&semdown,1);//cant continue until write does a semup
        semop(semid,&semdown,1);//access memmory between enc-write
        strcpy(temp,msg2rcv->word);
        semop(semid,&semup,1);
        /* printf("Message encoding..... : %s",temp ); */
        semop(semid2,&semdown,1);//access memory between enc chan
        strcpy(encodedmessage->word,temp);
        MD5((unsigned char *)&temp,strlen(temp),(unsigned char*)&(encodedmessage->hash));
        semop(semid2,&semup,1);
        semop(semid3,&semup,1);//chan can continue now
        pthread_mutex_lock(&mutex);
        if(thisflag->flag==1){
            pthread_mutex_unlock(&mutex);
            break;
        }
        else if(strcmp("TERM\n",temp)==0){
            thisflag->flag=1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        while(1){//check for ack/nack
            semop(semid5,&semdown,1);//blocked until chan sends ack/nack
            semop(semid2,&semdown,1);
            strcpy(buffer,encodedmessage->word);
            semop(semid2,&semup,1);
            if(strcmp(buffer,"ACK")==0){
                printf("ENC: Message sent.\n");
                break;//go in starting state
            }
            else{
                printf("ENC: Message not sent retrying...\n");//resent message
                semop(semid2,&semdown,1);
                strcpy(encodedmessage->word,temp);
                semop(semid2,&semup,1);
                semop(semid3,&semup,1);
            }
        }
        semop(semid6,&semup,1);//write can continue now
    }
    printf("READ_ENCODE EXITED \n");
    pthread_exit(0);
}

void *writemsg(void *param){
    /* printf("Write created.\n"); */
    args *arguments = param;
    message *msg2s=arguments->msg;
    flag *thisflag=arguments->TERMflag;
    int semid=arguments->id;
    int semid2=arguments->id2;
    int semid3=arguments->id3;
    char temp[MESSAGE_LENGTH];
    while(1){
        fgets(temp,100,stdin);
        pthread_mutex_lock(&mutex);
        if(thisflag->flag==1){
            strcpy(temp,"TERM\n");
        }
        pthread_mutex_unlock(&mutex);
        /* printf("%s",temp); */
        semop(semid,&semdown,1); //access memory
        strcpy(msg2s->word,temp);
        semop(semid,&semup,1);
        if(strcmp("TERM\n",temp)==0){
            pthread_mutex_lock(&mutex);
            thisflag->flag=1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        semop(semid2,&semup,1);//encoder can continue
        semop(semid3,&semdown,1);//write cant continue until last message sent succesfully
    }
    printf("WRITE EXITED\n");
    semop(semid2,&semup,1);//if TERM encoder must continue sending term to other threads
    pthread_exit(0);
}
