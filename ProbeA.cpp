#include <iostream>
#include <stdlib.h>

using namespace std;

int alpha = 997;
int beta = 257;
int rho = 251;

int generateValue() {
    int num;
    num = rand();
    return num;
}

void sendToHub(int num) {

}

bool waitForResponse() {

}

int main() {
    for (int i = 0; i < 100; i++) {
        cout << generateValue() << endl;
    }
    return 0;
}
