#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<stdint.h>
#include<semaphore.h>


struct mesg_buffer{
	long mesg_type;
	char mesg_text[100];
};

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

void* producer_func(void* threadid){
	struct mesg_buffer message_prod;
	message_prod.mesg_type = 1;

	struct mesg_buffer message_cons;
	key_t key_prod, key_cons;
	int msg_id_prod, msg_id_cons;
	int tid = (intptr_t)threadid;
	key_cons = ftok("consumer", 15);
	key_prod = ftok("producer", 96);
	msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);
	while(1){
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		if(msgrcv(msg_id_cons, &message_cons, sizeof(message_cons), 0, 0) < 0 ){
			printf("Error receiving message from consumer\n");
			exit(EXIT_FAILURE);
		}
		printf("Producer-%d(%lu): Resource request received from consumer %s\n",tid,pthread_self()%10000,message_cons.mesg_text);	
		
		snprintf(message_prod.mesg_text,sizeof(message_prod.mesg_text),"Message from producer - %d(%lu)\n",tid,pthread_self()%10000);
		printf("Producer-%d(%lu): Sending acknowledgement\n",tid,pthread_self()%10000);

		if(msgsnd(msg_id_prod, &message_prod, sizeof(message_prod), 0) < 0){
			printf("Error sending message to the consumer\n");
			exit(EXIT_FAILURE);
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
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

        key_cons = ftok("consumer", 15);
        key_prod = ftok("producer", 96);

        msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);

        snprintf(message_cons.mesg_text,sizeof(message_cons.mesg_text),"%lu",pthread_self()%10000);
        if(msgsnd(msg_id_cons, &message_cons,sizeof(message_cons),0) < 0){
               printf("Error sending message to producer\n");
               exit(EXIT_FAILURE);
        }

        printf("Consumer-%d(%lu): Resource request sent\n",tid,pthread_self()%10000);

	while(1){
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
        	printf("Consumer-%d(%lu): Waiting for resource\n",tid,pthread_self()%10000);
        	if(msgrcv(msg_id_prod, &message_prod, sizeof(message_prod),1, 0) < 0 ){
        		fprintf(stderr,"Error receiving the message from producer\n");
			exit(EXIT_FAILURE);
		}
		
		printf("Consumer-%d(%lu): Resource received from producer - %s",tid,pthread_self()%10000,message_prod.mesg_text);
		
		printf("Consumer-%d(%lu): Sending another request\n",tid,pthread_self()%10000);
		if(msgsnd(msg_id_cons, &message_cons,sizeof(message_cons),0) < 0){
                        printf("Error sending message to producer\n");
                        exit(EXIT_FAILURE);
                }
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
        return 0;
}


int main(){
        // Defined mutex and semaphores	
	pthread_mutex_init(&mutex,NULL);
	sem_init(&empty,0,4);
	sem_init(&full,0,0);
        

	//Creating files that are used for creation of the keys
	FILE* p;  
	p = fopen("consumer","w+");
	FILE* c ;
        c = fopen("producer","w+");

	// Initialization of message queues, one for producers and one for consumers 
	int msg_id_prod, msg_id_cons;
	key_t key_prod, key_cons;
	key_cons = ftok("consumer", 15);
        key_prod = ftok("producer", 96);
        msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);

	// array of Threads for producers and consumers 
	pthread_t prod_threads[4];
	pthread_t cons_threads[4];
	int i;

	// Creation of threads
	for(i = 0; i < 4; i++){
		pthread_create(&prod_threads[i], NULL, producer_func, (void *)(i+1));
	}

	for(i = 0; i < 4; i++){
		pthread_create(&cons_threads[i], NULL, consumer_func,(void *)(i+1));
	}

	for(i = 0; i < 4; i++){
		pthread_join(prod_threads[i], NULL);
	}

	for(i = 0; i < 4; i++){
	       pthread_join(cons_threads[i], NULL);
      	}	       
	
	// Destroying the mutex, semaphore variables and the message queues
	pthread_mutex_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);

	msgctl(msg_id_cons, IPC_RMID, NULL);
	msgctl(msg_id_prod, IPC_RMID, NULL);
	fclose(p);
	fclose(c);
	return 0;
}
