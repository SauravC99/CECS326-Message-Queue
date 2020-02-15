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

using namespace std;

int alpha = 997;
//int beta = 257; not needed only probe B
//int rho = 251; not needed only probe C

int value;


/*
*    generates the value divisible by alpha and returns it
**/
int generateValue() {
    int num;
    bool generated = false;
    while (!generated) {
        num = rand();
        if (num % alpha = 0)
            generated = true;
    }
    return num;
}

void sendToHub(int num) {

}

bool waitForResponse() {

}

bool terminate(int num) {
    if (num < 50)
        return true;
    return false;
}

int main() {
    while (!terminate(100)) {
        //produce reading
        value = generateValue();
        sendToHub(value);
        
    }
}
