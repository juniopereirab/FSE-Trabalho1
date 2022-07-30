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

#define COLOR_GREEN 1
#define COLOR_YELLOW 2
#define COLOR_RED 3

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

semaphore Primary;
semaphore Secondary;

light setupLight (int color, int pin) {
    light L;
    L.color = color;
    L.pin = pin;
    L.state = false;
    L.prevState = false;

    pinMode(pin, OUTPUT);

    return L;
}

void setupSemaphore () {
    Primary.green = setupLight(COLOR_GREEN, GREEN);
    Primary.yellow = setupLight(COLOR_YELLOW, YELLOW);
    Primary.red = setupLight(COLOR_RED, RED);

    Secondary.green = setupLight(COLOR_GREEN, GREEN_2);
    Secondary.yellow = setupLight(COLOR_YELLOW, YELLOW_2);
    Secondary.red = setupLight(COLOR_RED, RED_2);
}

void setState (int s1, int s2, int s3, int s4, int s5, int s6) {
    digitalWrite(Primary.green.pin, s1);
    digitalWrite(Primary.yellow.pin, s2);
    digitalWrite(Primary.red.pin, s3);
    digitalWrite(Secondary.green.pin, s4);
    digitalWrite(Secondary.yellow.pin, s5);
    digitalWrite(Secondary.red.pin, s6);
}

void temporizador () {
    setState(1, 0, 0, 0, 0, 1);
    delay(17000);
    setState(0, 1, 0, 0, 0, 1);
    delay(3000);
    setState(0, 0, 1, 1, 0, 0);
    delay(7000);
    setState(0, 0, 1, 0, 1, 0);
    delay(3000);
}

int main () {
    printf("Iniciou\n");
    if(wiringPiSetup() == -1) return 1;

    setupSemaphore();
  
    while(1) {
        temporizador();
    }

    return 0;
}
