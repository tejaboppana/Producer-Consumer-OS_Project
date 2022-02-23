#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<stdint.h>


struct mesg_buffer{
	long mesg_type;
	char mesg_text[100];
};


void* producer_func(void* threadid){
	struct mesg_buffer message_prod;
	message_prod.mesg_type = 1;

	struct mesg_buffer message_cons;
	key_t key_prod, key_cons;
	int msg_id_prod, msg_id_cons;
	int tid = (intptr_t)threadid;
	key_cons = ftok("consumer_file", 15);
	key_prod = ftok("producer_file", 96);
	msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);
	while(1){
		if(msgrcv(msg_id_cons, &message_cons, sizeof(message_cons), 0, 0) < 0 ){
			printf("Error receiving message from consumer\n");
			exit(EXIT_FAILURE);
		}
		printf("Producer-%lu(%d): Resource request received from consumer %s\n",pthread_self()%10000,tid,message_cons.mesg_text);	
		
		snprintf(message_prod.mesg_text,sizeof(message_prod.mesg_text),"Message from producer - %d, Thread ID - %lu\n",tid,pthread_self()%10000);
		printf("Producer-%lu(%d): Sending acknowledgement\n",pthread_self()%10000,tid);

		if(msgsnd(msg_id_prod, &message_prod, sizeof(message_prod), 0) < 0){
			printf("Error sending message to the consumer\n");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

void* consumer_func(void* threadid){
	struct mesg_buffer message_prod;

        struct mesg_buffer message_cons;
	message_cons.mesg_type = 2;

	int i;
        key_t key_prod, key_cons;
        int msg_id_prod, msg_id_cons;
        int tid = (intptr_t)threadid;

        key_cons = ftok("consumer_file", 15);
        key_prod = ftok("producer_file", 96);

        msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);

        snprintf(message_cons.mesg_text,sizeof(message_cons.mesg_text),"%lu",pthread_self()%10000);

        if(msgsnd(msg_id_cons, &message_cons,sizeof(message_cons),0) < 0){
               printf("Error sending message to producer\n");
               exit(EXIT_FAILURE);
        }

        printf("Consumer-%lu(%d): Resource request sent\n",pthread_self()%10000,tid);

	while(1){
        	printf("Consumer-%lu(%d): Waiting for resource\n",pthread_self()%10000,tid);
        	if(msgrcv(msg_id_prod, &message_prod, sizeof(message_prod),1, 0) < 0 ){
        		fprintf(stderr,"Error receiving the message from producer\n");
			exit(EXIT_FAILURE);
		}

		printf("Consumer-%lu(%d): Resource received from producer and hence printing the message - %s\n",pthread_self()%10000,tid,message_prod.mesg_text);
		
		printf("Consumer-%lu(%d): Sending another request\n",pthread_self()%10000,tid);
		if(msgsnd(msg_id_cons, &message_cons,sizeof(message_cons),0) < 0){
                        printf("Error sending message to producer\n");
                        exit(EXIT_FAILURE);
                }
	}
        return 0;
}


int main(){
	
	int msg_id_prod, msg_id_cons;
	key_t key_prod, key_cons;
	key_cons = ftok("consumer_file", 15);
        key_prod = ftok("producer_file", 96);
        msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);

	pthread_t prod_threads[4];
	pthread_t cons_threads[4];
	int i;
	for(i = 0; i < 4; i++){
		pthread_create(&prod_threads[i], NULL, producer_func, (void *)i);
		pthread_create(&cons_threads[i], NULL, consumer_func,(void *)i);
	}

	for(i = 0; i < 4; i++){
		pthread_join(prod_threads[i], NULL);
		pthread_join(cons_threads[i], NULL);
	}
	 
	msgctl(msg_id_cons, IPC_RMID, NULL);
	msgctl(msg_id_prod, IPC_RMID, NULL);
	return 0;
}
