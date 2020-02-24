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

int alpha = 997;

int value;
struct buf {
    long mtype; // required
    char greeting[50]; // mesg content
};

const char* convert(int num);

/*
*    generates the value divisible by alpha and returns it
*    also if returns 0 then terminate the program
**/
int generateValue() {
    int num;
    bool generated = false;
    while (!generated) {
        num = rand();
         
        //will terminate the program if 
        //the random num is less than 50
	    if(false) {//terminate(num)) {
	        generated = true;
	        num = 0;
        }
        else if (num % alpha == 0)
            generated = true;
    }
    return num;
}

void sendToHub(int num) {
    //string message = to_string(num);
    //send data to other program
    buf msg;
	int size = sizeof(msg)-sizeof(long);

    int qid = msgget(ftok(".", 'u'), 0);

	// prepare my message to send
	//strcpy(msg.greeting, "Hello there");
    string messageToSnd = to_string(num);
    strcpy(msg.greeting, messageToSnd.c_str()); //converts string to array of char

    
	cout << getpid() << ": sends greeting" << endl;
	msg.mtype = 117; 	// set message type mtype = 117
	msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending

	msgrcv(qid, (struct msgbuf *)&msg, size, 314, 0); // reading
	cout << getpid() << ": gets reply" << endl;
	cout << "reply: " << msg.greeting << endl;
	cout << getpid() << ": now exits" << endl;

	msg.mtype = 117;
	msgsnd (qid, (struct msgbuf *)&msg, size, 0);

	exit(0);
}

bool waitForResponse() {
    //get response from other program
    //returns true if need to wait
}

bool terminate(int num) {
    if (num < 50)
        return true;
    return false;
}

int main() {
    cout << "Start" << endl;
    cout << "sleep" << endl;
    int a = generateValue();
    cout << a << endl;
    sendToHub(a);
    cout << "done" << endl;
}
