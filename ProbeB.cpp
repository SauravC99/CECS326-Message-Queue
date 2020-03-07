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

int generateValue();
void sendToHub(int num);

int beta = 257;

int qid = msgget(ftok(".", 'u'), 0);

int value;
int count;

struct buf {
    long mtype; // required
    char greeting[50]; // mesg content
};
buf msg;
int size = sizeof(msg)-sizeof(long);

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
    //buf msg;
	int size = sizeof(msg)-sizeof(long);

    //int qid = msgget(ftok(".", 'u'), 0);

    string messageToSnd = to_string(num);
    strcpy(msg.greeting, messageToSnd.c_str()); //converts string to array of char

    msg.mtype = 2;
    
	
	//msg.mtype = 117; 	// set message type mtype = 117
	msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
    cout << getpid() << ": sends greeting" << endl;

    count++;
}

//change so that it will terminate after 10000 messages using force patch file
bool end() {
    if (count <= 10000)
        return true;
    return false;
}

int main() {
    count = 1;
    while (!end()) {
        //produce reading
        sendToHub(generateValue());
        
    }
}
