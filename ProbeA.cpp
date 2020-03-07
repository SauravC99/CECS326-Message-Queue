#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string>

using namespace std;

//Probe A will do the following:

//I.  It will first generate a random number divisible by its seed number alpha, keep doing so
//
//II. It will send it to the datahub through port 1 and wait to recieve a response from 
//    the datahub in port 115 to proceed with the number generation
//
//III.This probe will turn itself off when it generates a number below 50 and tell the datahub it's quit



//initializing methods for use in program
int generateValue();
void sendToHub(int num);
bool terminate(int num);

//the seed number
int alpha = 997;

//retrieves the mesage queue ID that is active, will return a -1 if its not found
int qid = msgget(ftok(".", 'u'), 0);

//initializing the terminate boolean, will become true when the program is told to terminate
//when it generates a number less than 50
bool term = false;

//the buffer that is used for the messages in the queue
struct buf {
    long mtype;
    char greeting[50]; 
}

//initializing the buffer
buf msg;

//initializing the size of the message buffer
int size = sizeof(msg)-sizeof(long);

//saves the message sent earlier to compare to later
string savedmsg;


/*
*    generates the value divisible by alpha and returns it
*    also if returns 0 
**/
int generateValue() {
    int num;
    bool generated = false;
    while (!generated) {
        num = rand(RAND_MAX);
         
        //will terminate the program if 
        //the random num is less than 50
        term =  terminate(num);
	    if(term) {
	        generated = true;
        }
        else if (num % alpha == 0)
            generated = true;
    }
    return num;
}


/*
*sends the number generated to the datahub, 
*will terminate instead if the number is less than 50
**/
void sendToHub(int num) {

	// prepare my message to send
    string messageToSnd = to_string(num);

    //The message sent needs the A in front of it because mathmatically if
    //a random value is generated, there is a possibility that
    //the number will be divisible by the Common demominator of alpha, beta and rho
    //the max number of the random function is 2147483647 and alpha * beta = 256299
    //meaning if a number is divisible by both alpha and beta, the datahub wont know
    //where the message truly came from, therfore we must add which probe the message
    //derived from
    strcpy(msg.greeting, "A: " + messageToSnd.c_str());

    //saves the message for later use
    strcpy(savedmsg, "A: " + messageToSnd.c_str());
    
    //prepares the mtype (the port to send to)
	msg.mtype = 1;

    //sends the message and prints the message sent to the datahub
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    cout << getpid() << ": sends greeting: " << msg.greeting << endl;

    //waits for the acknoledgement message from the datahub, in a while loop checks to see
    // if it matches with the message recently sent so that 
    bool hasRecievedAcknolwdgement = false;
    while(!hasRecievedAcknolwdgement){
	    
        //repeatedly checks to see if the message matches with the one sent so that
        //it wont get a false positive when reading back the acknoledgement 
        msgrcv(qid, (struct msgbuf *)&msg, size, 115, 0);
        
        //replicates the message recieved to see if the message sent earlier was truly the one recieved
        msgcpy(savedmsg, " and acknowledged.");
        if(savedmsg == msg.greeting){
            cout << getpid() << ": got acknoledgement" << endl;
	        cout << "reply: " << msg.greeting << endl;
            hasRecievedAcknolwdgement = true;
        }
	    
    }
}


/*
* returns true if the number is less than 50 so that the proces can terminate
**/
bool terminate(int num) {
    if (num < 50)
        //sends the exit message through port 1
        msg.mtype = 1;

        //prepares the message to send to the datahub and then exists through the exit code
        strcpy(msg.greeting, "A_Leaves");
	    msgsnd (qid, (struct msgbuf *)&msg, size, 0);
        cout << getpid() << ": sends greeting" << msg.greeting << endl;

        //exits the message queue
        exit(0);
        return true;
    return false;
}

int main() {
    
    sendToHub(generateValue());
}
