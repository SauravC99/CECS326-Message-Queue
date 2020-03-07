#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

//The Datahub will do the following:
//I.  Get all messages from each probe and read them, until all the probes have exited
//
//II. Send back an acknowledgement message if the message came from Probe A
//
//III.Force quits Probe B if the datahub has recieved 10,000 messages, through force_patch
//
//IV. Terminates and deletes message queue after all probes have exited



//initializing all of the functions of the datahub
int findProbe();
void getReading();
void respondTOProbeA();
bool checkIfProbeAQuit();
void terminateProbeB();
void terminateProbeC();
void deleteQueue();

//listing of all the magic seed numbers of the probes
int alpha = 997;
int beta = 257;
int rho = 251;

//will be the id of the queue and creates the message queue
int qid = msgget(ftok(".",'u'), IPC_EXCL|IPC_CREAT|0600);

//the bufer used in all probes to interact with the message queue
struct buf {
    long mtype; // required
    char greeting[50]; // mesg content
};

//initializing buffer for use
buf msg;

//listing of which probes are active, once all probes have exited, the datahub will 
//exit and delete the message queue
bool ProbeAActive = true;
bool ProbeBActive = false;
bool ProbeCActive = false;


/*
* finds which probe where the message came from 
* and returns a number relative to the probe where the message came from
* 0 = came from Probe A
* 1 = came from Probe B
* 2 = came from Probe C
* -1 = error arose somehow or its an exit message
**/
int findProbe() {
    char firstElement = msg.greeting[0];

    if (firstElement == 'A'){
        return 0;
    }
    else if (firstElement == 'B'){
        return 1;
    }
    else if (firstElement == 'C')
    {
        return 2;
    }
    return -1;
    
}

//initiating the size for later use
int len;


/*
* gets the reading from the first element, since we are 
* assuming lossless data (in the instructions), we dont have to worry about overwritten data
* meaning we can send messages to the same port on the message queue, we will be getting our
* messages from port 1
*
* this function gets the message from the queue at port 0 and prints which function it came from 
* at the same time it detects when to:
*  1. reply to the Probe A on port 115, when it recieves a message from it
*  2. terminates Probe B when receiving 10,000 messages
*  3. terminates Probe C according to its terminate instructions
*  4. detects when Probe A quits
**/
void getReading() {

    //initiating the size of the buffer
    len = sizeof(msg)-sizeof(long);

    //sets the mtype to 1
    msg.mtype = 1;

    //reads the message and displays it
    msgrcv(qid, (struct msgbuf *)&msg, len, 1, 0);
    cout << getpid() << ": gets message" << endl;
    cout << "message: " << msg.greeting << endl;
	
    // detects which probe the greeting came from according to the message
    int Probe = findProbe();

    //first checks if its from probe A
    if(Probe == 0){

        //checks if it quit
        if(checkIfProbeAQuit()){

            //if it quit set the probe active variable to false, so that the terminal knows
            ProbeAActive = false;
        }

        //otherwise send a response
        else{
            respondTOProbeA();
        }
    }

    // checks if its from probe B
    else if (Probe == 1){

    }

    //checks if its from probe C

    else if(Probe == 2){

    }

}


/*
* responds to probe A messages on port 115
**/
void respondTOProbeA(){

    //creates a response to the Probe with the return message 
    strcat(msg.greeting, " and acknowledged.");
    cout << getpid() << ": sends reply" << endl;
    
    //setting the mtype to 115
    msg.mtype = 115;

    //sends the message then reverts to the port 1
    msgsnd(qid, (struct msgbuf *)&msg, len, 0);
    msg.mtype = 1;
}


/*
* checks if probe A has sent its quit message
**/
bool checkIfProbeAQuit(){

    //creates a temporary message buffer to use and compare to the data
    buf tempBuffer;
    strcpy(tempBuffer.greeting, "A_Leaves");

    if(msg.greeting == tempBuffer.greeting){
        return true;
    }
    return false;
}


/*
* terminates probe B
**/
void terminateProbeB(){

}


/*
* terminates Probe C
**/
void terminateProbeC(){

}


/*
* this function clears the queue and exits
**/
void deleteQueue(){
    
    //deletes the queue and exits the program
    msgctl (qid, IPC_RMID, NULL);
    exit(0);
}


/*
*
**/
int main() {

    //will continuously run until all probes become inactive, then delete the queue
    while(ProbeAActive || ProbeBActive || ProbeCActive){
        getReading();
    }
    deleteQueue();
    
}