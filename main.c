#include <stdio.h>
#include <wiringPi.h>

#define GREEN 8
#define YELLOW 9
#define RED 14

#define GREEN_2 30
#define YELLOW_2 21
#define RED_2 22

#define true 1
#define false 0
typedef int bool;

struct Light {
    int color;
    int pin;
    bool state;
    bool prevState;
} typedef light;

struct Semaphore {
    light green;
    light yellow;
    light red;
} typedef semaphore;

void temporizador () {
    digitalWrite(GREEN, 1);
    delay(20000);
    digitalWrite(GREEN, 0);
    digitalWrite(YELLOW, 1);
    delay(3000);
    digitalWrite(YELLOW, 0);
    digitalWrite(RED, 1);
    delay(10000);
    digitalWrite(RED, 0);
}

int main () {
    if(wiringPiSetup() == -1) return 1;
    
    printf("Configura portas");
    pinMode(GREEN, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(RED, OUTPUT);

    while(1) {
        temporizador();
    }

    return 0;
}
