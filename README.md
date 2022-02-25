**Solution to the Multi-Dimensioned Producer Consumer Problem (More than 3 pairs)**
-

The above code implements a soltuion to the multiple producer consumer problem using message passing. The basic idea behind the implementation is as below
- If the producer receives a message (resource request) from consumer, it places a message in the queue with timestamp (which indicates the issue of resource)
- Producer sends the acknowledgement to the consumer
- Consumer determines the resource requirement 
- Consumer send the resource request to the producer 

**IMPLEMENTATION**

- There are 2 system V message queues , one for the consumer to send requests to the producer and the other one for the producer to send the acknowledgement
- When producer receives a request from a consumer, it sends the resource (a message is added in the producer queue for the consumer to read) and an acknowledgement is sent (in the form of semaphore signal) to the consumer. 
- Every time a consumer function is executed, a resource request is sent to producer via consumer message queue. 
- After the request is sent, consumer waits for the resource from producer and prints the message when it receives the acknowledgement. 
- Next it sends another requests for resource. 
- Mutex and sempahores are used in both producer and consumer function so that none of the consumers receive the acknowledgements simultaneiously or none of the producers send the resource at the same time and the amount of resources do not exceed the threshhold. 

**STEPS TO RUN THE CODE**
- ```make all``` will compile the code.
- Run the executable - ```./producer_consumer```
- Sample output 
```
Producer-3(5344): Resource request received from consumer 9936
Consumer-4(4096): Resource request sent
Producer-3(5344): Sending acknowledgement
Consumer-1(9936): Resource request sent
Consumer-3(4528): Resource request sent
Consumer-2(7232): Resource request sent
Producer-2(8048): Resource request received from consumer 4096
Producer-2(8048): Sending acknowledgement
Producer-1(752): Resource request received from consumer 7232
Producer-1(752): Sending acknowledgement
Producer-4(2640): Resource request received from consumer 4528
Producer-4(2640): Sending acknowledgement
Consumer-2(7232): Waiting for resource
Consumer-2(7232): Resource received from producer - Fri Feb 25 12:12:55 2022
: Message from producer - 3(5344)
Consumer-2(7232): Sending another request
Consumer-1(9936): Waiting for resource
Consumer-1(9936): Resource received from producer - Fri Feb 25 12:12:57 2022
: Message from producer - 2(8048)
Consumer-1(9936): Sending another request
Consumer-3(4528): Waiting for resource
Consumer-3(4528): Resource received from producer - Fri Feb 25 12:12:59 2022
: Message from producer - 1(752)
Consumer-3(4528): Sending another request
```

