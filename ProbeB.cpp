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

//Associated Files:
//DataHub.cpp

//Steven Centeno
//Saurav Chhapawala

//this probe will send messages until the datahub has recieved 10000 messages

int generateValue();
void sendToHub(int num);

int beta = 257;
int qid = msgget(ftok(".", 'u'), 0);

int skip = 0;

struct buf {
    long mtype; // required
    char greeting[50]; // mesg content
};
buf msg;
int len = sizeof(msg)-sizeof(long);

//need to change so that it returns a num divisible by its magic seed (beta)
int generateValue() {
    int num;
    bool generate;
    generate = false;
    while (!generate) {
        num = rand();
        if (num % beta == 0)
            generate = true;
    }
    return num;
}

void sendToHub(int num) {
    msg.mtype = 1;
    string messageToSnd;

    if (skip == 0) {
        messageToSnd = to_string(getpid());
        strcpy(msg.greeting, "B:");
        strcat(msg.greeting,  messageToSnd.c_str());
        msgsnd(qid, (struct msgbuf *)&msg, len, 0); // sending
        cout << getpid() << ": sends greeting: " << msg.greeting << endl;
        skip++;
    }

    messageToSnd = to_string(num);
    strcpy(msg.greeting, "B: ");
    strcat(msg.greeting,  messageToSnd.c_str());    
    
	msgsnd(qid, (struct msgbuf *)&msg, len, 0); // sending
    cout << getpid() << ": sends greeting: " << msg.greeting << endl;
}


int main() {
    while (true) {
        //produce reading
        int value;
        value = generateValue();
        sendToHub(value);
        
    }
}
