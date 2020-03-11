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
#include "force_patch.h"

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
void terminateProbeB(pid_t id);
bool checkIfProbeCQuit();
void deleteQueue();

//listing of all the magic seed numbers of the probes
int alpha = 997;
int beta = 257;
int rho = 251;

int countForB = 0;
int skip = 0;
pid_t id;

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
bool ProbeBActive = true;
bool ProbeCActive = true;

//will hold the PID of the Probes when receiving the first mesage
string ProbeAPID = "";
string ProbeBPID = "";
string ProbeCPID = "";


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

        //if the Hub doesnt have the PID of the Probe yet make it remember the Probe ID
        if(ProbeAPID == ""){
            char a = msg.greeting[3];
            string aa(1, a);
            char b = msg.greeting[4];
            string bb(1, b);
            char c = msg.greeting[5];
            string cc(1, c);
            char d = msg.greeting[6];
            string dd(1, d);
            ProbeAPID = aa + bb + cc + dd;
        }
        return 0;
    }

    else if (firstElement == 'B'){
        
        if(ProbeBPID == ""){
            char a = msg.greeting[2];
            string aa(1, a);
            char b = msg.greeting[3];
            string bb(1, b);
            char c = msg.greeting[4];
            string cc(1, c);
            char d = msg.greeting[5];
            string dd(1, d);
            ProbeBPID = aa + bb + cc + dd;
        }
        return 1;
    }
    else if (firstElement == 'C'){

        if(ProbeCPID == ""){
            char a = msg.greeting[3];
            string aa(1, a);
            char b = msg.greeting[4];
            string bb(1, b);
            char c = msg.greeting[5];
            string cc(1, c);
            char d = msg.greeting[6];
            string dd(1, d);
            ProbeCPID = aa + bb + cc + dd;
        }
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

    //reads the message
    msgrcv(qid, (struct msgbuf *)&msg, len, 1, 0);
	
    // detects which probe the greeting came from according to the message
    int Probe = findProbe();

    //first checks if its from probe A
    if(Probe == 0){

        //displays the message
        cout << getpid() << ": gets message from probe A: PID(" << ProbeAPID;
        cout << ") message: " << msg.greeting << endl;

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
        
        //displays message
        cout << getpid() << ": gets message from probe B: PID(" << ProbeBPID;
        cout << ") message: " << msg.greeting << endl;

        //id = getpid();
        //cout << id << endl;

        //will be used for the purposes of the force_patch
        string aaa;
        int bbb;
        if (skip == 0) {
            countForB = 0;
            char a = msg.greeting[2];
            string aa(1, a);
            char b = msg.greeting[3];
            string bb(1, b);
            char c = msg.greeting[4];
            string cc(1, c);
            char d = msg.greeting[5];
            string dd(1, d);
            aaa = aa + bb + cc + dd;
            bbb = stoi(aaa);
            skip++;
        }
        //cout << aaa << endl;
        //cout << bbb << endl;
        //id = getpid();

        countForB++;

        //cout << countForB << endl;
        if (countForB == 10000) {
            //cout << "terminate B: "<<bbb<<endl;
            terminateProbeB(bbb);
        }
    }

    //checks if its from probe C
    else if(Probe == 2){

        //displays the message
        cout << getpid() << ": gets message from probe C: PID(" << ProbeCPID;
        cout << ") message: " << msg.greeting << endl;

        //checks if it quit
        if(checkIfProbeCQuit()){

            //if it quit set the probe active variable to false, so that the terminal knows
            ProbeCActive = false;
        }
    }

}


/*
* responds to probe A messages on port 115
**/
void respondTOProbeA(){

    //creates a response to the Probe with the return message 
    strcat(msg.greeting, " and acknowledged.");
    cout << getpid() << ": sends reply" << endl;
    cout << "message sent: " << msg.greeting << endl;
    
    //setting the mtype to 115
    msg.mtype = 115;

    //sends the message then reverts to the port 115
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

    if(strcmp(msg.greeting,tempBuffer.greeting) == 0){
        cout << "ProbeA quit" << endl;
        return true;
    }
    return false;
}


/*
* terminates probe B
**/
void terminateProbeB(pid_t id){
    force_patch(id);
    ProbeBActive = false;
    cout << "KIlled Probe B" << endl;
}


/*
* checks if probe C has sent its quit message
**/
bool checkIfProbeCQuit(){

    //creates a temporary message buffer to use and compare to the data
    buf tempBuffer;
    strcpy(tempBuffer.greeting, "C_Leaves");

    if(strcmp(msg.greeting,tempBuffer.greeting) == 0){
        cout << "ProbeC quit" << endl;
        return true;
    }
    return false;
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
    cout << "Exited queue and deleted it" << endl;
    deleteQueue();
    
}