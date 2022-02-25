**Solution to the Multi-Dimensioned Producer Consumer Problem (More than 3 pairs)**
-

The above code implements a soltuion to the multiple producer consumer problem using message passing. The basic idea behind the implementation is as below
- If the producer receives a message (resource request) from consumer, it places a message in the queue (which indicates the issue of resource)
- Producer sends the acknowledgement to the consumer
- Consumer determines the resource requirement 
- Consumer send the resource request to the producer 

**IMPLEMENTATION**

- There are 2 system V message queues , one for the consumer to send requests to the producer and the other one for the producer to send the acknowledgement
- When producer receives a request from a consumer, it sends the resource and an acknowledgement (in the form of message) to the producer queue.
- Every time a consumer function is executed, a resource request is sent to producer via consumer queue. 
- After the request is sent, consumer waits for the resource from producer and prints the message when it receives the acknowledgement. 
- Next it sends another requests for resource. 
- Mutex and sempahores are used in both producer and consumer function so that none of the consumers receive the acknowledgements simultaneiously or none of the producers send the resource at the same time and the amount of resources do not exceed the threshhold. 

**STEPS TO RUN THE CODE**
- ```make all``` will compile the code.
- Run the executable - ```./producer_consumer```
- Sample output 
```
Producer-1(8976): Resource request received from consumer 8160
Producer-1(8976): Sending acknowledgement
Consumer-1(8160): Resource request sent
Consumer-2(5456): Resource request sent
Producer-2(6272): Resource request received from consumer 5456
Producer-2(6272): Sending acknowledgement
Producer-3(3568): Resource request received from consumer 32
Producer-3(3568): Sending acknowledgement
Consumer-3(32): Resource request sent
Producer-4(864): Resource request received from consumer 2752
Producer-4(864): Sending acknowledgement
Consumer-1(8160): Waiting for resource
Consumer-1(8160): Resource received from producer - Message from producer - 1(8976)
Consumer-1(8160): Sending another request
Producer-1(8976): Resource request received from consumer 8160
Producer-1(8976): Sending acknowledgement
Consumer-1(8160): Waiting for resource
Consumer-1(8160): Resource received from producer - Message from producer - 2(6272)
Consumer-1(8160): Sending another request
Producer-2(6272): Resource request received from consumer 8160
Producer-2(6272): Sending acknowledgement
Consumer-1(8160): Waiting for resource
Consumer-1(8160): Resource received from producer - Message from producer - 3(3568)
```

