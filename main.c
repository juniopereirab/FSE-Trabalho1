#include <stdio.h>
#include <wiringPi.h>

#define GREEN 8
#define YELLOW 9
#define RED 14

#define GREEN_2 30
#define YELLOW_2 21
#define RED_2 22

#define BUTTON_1 12
#define BUTTON_2 13

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

struct State {
    int s1;
    int s2;
    int s3;
    int s4;
    int s5;
    int s6;
    long time;
    bool minimum;
} typedef state;

semaphore Primary;
semaphore Secondary;

bool minimumHasPassed = false;
bool button1HasBeenPressed = false;
bool button2HasBeenPressed = false;

long lastInterruptTime = 0;
int position = 0;

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

void timer (state S) {
    setState(S.s1, S.s2, S.s3, S.s4, S.s5, S.s6);
    if(S.minimum) {
        delay(S.time / 2);
        minimumHasPassed = true;
        if (button1HasBeenPressed) {
            position = 0;
            minimumHasPassed = false;
            button1HasBeenPressed = false;
            return;
        }

        if (button2HasBeenPressed) {
            position = 2;
            minimumHasPassed = false;
            button2HasBeenPressed = false;
            return;
        }

        for (int i = 0; i < 10; i++) {
            delay((S.time / 2) / 10);
            if (button1HasBeenPressed) {
                position = 0;
                minimumHasPassed = false;
                button1HasBeenPressed = false;
                return;
            }

            if (button2HasBeenPressed) {
                position = 2;
                minimumHasPassed = false;
                button2HasBeenPressed = false;
                return;
            }
        }
    } else {
        delay(S.time);
    }

    button1HasBeenPressed = false;
    button2HasBeenPressed = false;
    minimumHasPassed = false;
    return;
}

void callback_button1 (void) {
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 300){
        button1HasBeenPressed = true;
    }
    lastInterruptTime = interruptTime;
}

void callback_button2 (void) {
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 300){
        button2HasBeenPressed = true;
    }
    lastInterruptTime = interruptTime;
}

int main () {
    printf("Iniciou\n");
    if(wiringPiSetup() == -1) return 1;

    state states[4];

    states[0].s1 = 1;
    states[0].s2 = 0;
    states[0].s3 = 0;
    states[0].s4 = 0;
    states[0].s5 = 0;
    states[0].s6 = 1;

    states[0].time = 20000;
    states[0].minimum = true;

    states[1].s1 = 0;
    states[1].s2 = 1;
    states[1].s3 = 0;
    states[1].s4 = 0;
    states[1].s5 = 0;
    states[1].s6 = 1;

    states[1].time = 3000;
    states[1].minimum = false;
    
    states[2].s1 = 0;
    states[2].s2 = 0;
    states[2].s3 = 1;
    states[2].s4 = 1;
    states[2].s5 = 0;
    states[2].s6 = 0;

    states[2].time = 10000;
    states[2].minimum = true;

    states[3].s1 = 0;
    states[3].s2 = 0;
    states[3].s3 = 1;
    states[3].s4 = 0;
    states[3].s5 = 1;
    states[3].s6 = 0;

    states[3].time = 3000;
    states[3].minimum = false;

    setupSemaphore();
  
    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);

    pullUpDnControl(BUTTON_1, PUD_UP);
    pullUpDnControl(BUTTON_2, PUD_UP);

    wiringPiISR(BUTTON_1, INT_EDGE_RISING, &callback_button1);
    wiringPiISR(BUTTON_2, INT_EDGE_RISING, &callback_button2);

    while(1) {
        timer(states[position]);
        position++;
        if( position == 4 ) {
            position = 0;
        }
    }

    return 0;
}
