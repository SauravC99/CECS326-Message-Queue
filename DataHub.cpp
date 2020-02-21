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

int alpha = 997;
int beta = 257;
int rho = 251;

//will be the id of the queue
int qid;

struct buf {
    long mtype; // required
    char greeting[50]; // mesg content
};

//Purpose of the hub is to recieve data from the probes,
//then to output which probe sent the message and the data
//that came with it

//recieves valid readings from the probes (valid reading is
//dependent on what number the random integer is divisible by)
 
//if alpha is probe A, if beta Probe B, if rho Probe C

int isValid(int num, int probe) {
    if (probe == 1) {
        if (num % alpha == 0)
            return true;
        return false;
    }
    if (probe == 2) {
        if (num % beta == 0)
            return true;
        return false;
    }
    if (probe == 3) {
        if (num % rho == 0)
            return true;
        return false;
    }
}

int createMessageQueue(){
    // create my msgQ with key value from ftok()
    int qid = msgget(ftok(".",'u'), IPC_EXCL|IPC_CREAT|0600);
    return qid;
}

int getReading() {

    buf msg;
    int size = sizeof(msg)-sizeof(long);

    msgrcv(qid, (struct msgbuf *)&msg, size, 117, 0); // read mesg
						// don't read "fake" mesg
    cout << getpid() << ": gets message" << endl;
    cout << "message: " << msg.greeting << endl;
	
    strcat(msg.greeting, " and Adios.");
    cout << getpid() << ": sends reply" << endl;
    msg.mtype = 314; // only reading mesg with type mtype = 314
    msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    cout << getpid() << ": now exits" << endl;

    msgrcv (qid, (struct msgbuf *)&msg, size, -112, 0);
    cout << getpid() << ": gets message" << endl;
    cout << "message: " << msg.greeting << endl;
    
    msgrcv (qid, (struct msgbuf *)&msg, size, 0, 0);
    cout << getpid() << ": gets message" << endl;
    cout << "message: " << msg.greeting << endl;
    
    msgrcv (qid, (struct msgbuf *)&msg, size, 117, 0);
    cout << getpid() << ": gets message" << endl;
    cout << "message: " << msg.greeting << endl;

    // now safe to delete message queue
    msgctl (qid, IPC_RMID, NULL);
    return 0;
}

int main() {
    qid = createMessageQueue();
    getReading();
    
}
