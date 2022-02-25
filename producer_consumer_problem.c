#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<stdint.h>
#include<semaphore.h>
#include<signal.h>

#define NUM_USERS 4  // Number of producer-consumer pairs
#define BUFFER_SIZE 4 // Number of resources that can be accessed at a time
#define LOOP_SIZE 10 // Number of times each consumer and producer run

//Message structure which contains the text and the type of message to disinguish the producer and consumer messages
struct mesg_buffer{
	long mesg_type;
	char mesg_text[100];
};

// Initiialization of mutex and semaphore
pthread_mutex_t mutex;
sem_t empty;
sem_t full;


// producer function
void* producer_func(void* threadid){
	struct mesg_buffer message_prod;
	message_prod.mesg_type = 1; // message type for producer

	struct mesg_buffer message_cons;
	key_t key_prod, key_cons;
	int msg_id_prod, msg_id_cons;
	int tid = (intptr_t)threadid;
	key_cons = ftok("consumer", 15);
	key_prod = ftok("producer", 96);
	msg_id_cons = msgget(key_cons, 0666 | IPC_CREAT);
	msg_id_prod = msgget(key_prod, 0666 | IPC_CREAT);
	int count = 0;
	while(count < LOOP_SIZE){                   
		sem_wait(&empty);                       // Producer can provide the resource only if there are empty slots 
		pthread_mutex_lock(&mutex);
		if(msgrcv(msg_id_cons, &message_cons, sizeof(message_cons), 0, 0) < 0 ){ // receiving request from consumers
			printf("Error receiving message from consumer\n");
			exit(EXIT_FAILURE);
		}
		printf("Producer-%d(%lu): Resource request received from consumer %s\n",tid,pthread_self()%10000,message_cons.mesg_text);	
		
		snprintf(message_prod.mesg_text,sizeof(message_prod.mesg_text),"Message from producer - %d(%lu)\n",tid,pthread_self()%10000);// creating a message , which would logically mean a resource is being provided to the consumer
		printf("Producer-%d(%lu): Sending acknowledgement\n",tid,pthread_self()%10000);

		if(msgsnd(msg_id_prod, &message_prod, sizeof(message_prod), 0) < 0){  // acknowledging the request and sending the resource
			printf("Error sending message to the consumer\n");
			exit(EXIT_FAILURE);
		}
		pthread_mutex_unlock(&mutex); // release the mutex lock 
		sem_post(&full);
		count++;
	}
	return 0;
}

// Consumer function
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

        snprintf(message_cons.mesg_text,sizeof(message_cons.mesg_text),"%lu",pthread_self()%10000); // Creating the resource request
        if(msgsnd(msg_id_cons, &message_cons,sizeof(message_cons),0) < 0){  // Resource request sent by the consumer
               printf("Error sending message to producer\n");
               exit(EXIT_FAILURE);
        } 

        printf("Consumer-%d(%lu): Resource request sent\n",tid,pthread_self()%10000);
	int count = 0;
	while(count < LOOP_SIZE){
		sem_wait(&full);          // critical section for the consumer 
		pthread_mutex_lock(&mutex);
        	printf("Consumer-%d(%lu): Waiting for resource\n",tid,pthread_self()%10000);
        	if(msgrcv(msg_id_prod, &message_prod, sizeof(message_prod),1, 0) < 0 ){ // receives acnowedgement from the producer and gets the access to  resource
        		fprintf(stderr,"Error receiving the message from producer\n");
			exit(EXIT_FAILURE);
		}
		
		printf("Consumer-%d(%lu): Resource received from producer - %s",tid,pthread_self()%10000,message_prod.mesg_text); // prints the message which is equivalent to using the resource 
		
		printf("Consumer-%d(%lu): Sending another request\n",tid,pthread_self()%10000);
		if(msgsnd(msg_id_cons, &message_cons,sizeof(message_cons),0) < 0){ // Once the resource is utilized , sends another request for resource
                        printf("Error sending message to producer\n");
                        exit(EXIT_FAILURE);
                }
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);  // end of critical section
		count++;
	}
        return 0;
}



int main(){
        // Defined mutex and semaphores	
	pthread_mutex_init(&mutex,NULL);
	sem_init(&empty,0,BUFFER_SIZE);
	sem_init(&full,0,0);
 
	//Creating files that are used for creation of the keys
	FILE* p;
	FILE* c;
	p = fopen("consumer","w+");
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
	for(i = 0; i < NUM_USERS; i++){
		pthread_create(&prod_threads[i], NULL, producer_func, (void *)(i+1));
	}

	for(i = 0; i < NUM_USERS; i++){
		pthread_create(&cons_threads[i], NULL, consumer_func,(void *)(i+1));
	}
       
	for(i = 0; i < NUM_USERS; i++){
		pthread_join(prod_threads[i], NULL);
	}

	for(i = 0; i < NUM_USERS; i++){
	       pthread_join(cons_threads[i], NULL);
      	}	       
	
	// Destroying the mutex, semaphore variables and the message queues
	printf("Destroying the resources\n");
	pthread_mutex_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);

	if(msgctl(msg_id_cons, IPC_RMID, NULL) == 0){
		printf("Successfuly deleted consumer queue\n");
	}

	if(msgctl(msg_id_prod, IPC_RMID, NULL) == 0){
		printf("Successfully deleted producer queue\n");
	}

	fclose(p);
	remove("producer");
	fclose(c);
	remove("consumer");
	return 0;
}
