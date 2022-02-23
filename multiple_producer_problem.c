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
	printf("In the producer function - %d\n",tid);
	while(1){
		printf("In the while loop - producer\n");
		if(msgrcv(msg_id_cons, &message_cons, sizeof(message_cons), 0, 0) < 0 ){
			printf("Error receiving message from consumer\n");
			exit(EXIT_FAILURE);
		}
		printf("Data received is: %s\n",message_cons.mesg_text);	
		
		snprintf(message_prod.mesg_text,sizeof(message_prod.mesg_text),"Message from producer - %d, Thread ID - %lu\n",tid,pthread_self());
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
	int i;
        key_t key_prod, key_cons;
        int msg_id_prod, msg_id_cons;
        int tid = (intptr_t)threadid;
        key_cons = ftok("consumer_file", 15);
        key_prod = ftok("producer_file", 96);
        msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
        message_cons.mesg_type = 2;
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);
        snprintf(message_cons.mesg_text,sizeof(message_cons.mesg_text),"");
        for(i = 0; i < 4; i++){
                if(msgsnd(msg_id_cons, &message_cons,sizeof(message_cons),0) < 0){
                        printf("Error sending message to producer\n");
                        exit(EXIT_FAILURE);
                }
        }
	while(1){
		printf("In the while loop - consumer\n");
        	printf("Consumer %d is trying to receieve messages and print it. Thread ID - %lu\n",tid,pthread_self());
        	if(msgrcv(msg_id_prod, &message_prod, sizeof(message_prod),1, 0) < 0 ){
        		fprintf(stderr,"Error receiving the message from producer\n");
			exit(EXIT_FAILURE);
		}
		 printf("Message from producer - %s\n",message_prod.mesg_text);

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

	return 0;
}
