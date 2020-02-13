#include <iostream>
#include <stdlib.h>

using namespace std;

int alpha = 997;
int beta = 257;
int rho = 251;

int getReading() {

}

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