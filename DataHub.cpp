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

int getReading() {

}

//Purpose of the hub is to recieve data from the probes,
//then to output which probe sent the message and the data
//that came with it

//recieves valid readings from the probes (valid reading is
//dependent on what number the random integer is divisible by)
 
//if alpha is probe A, if beta Probe B, if rho Probe C

bool isValid(int num, int probe) {
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
