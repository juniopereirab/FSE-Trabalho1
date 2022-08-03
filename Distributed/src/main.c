#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <pthread.h>
#include "socket.h"
#include "definitions.h"

semaphore Primary;
semaphore Secondary;

report *reports;
int reportsLength = 0;

pthread_t threadA;

bool minimumHasPassed = false;
bool button1HasBeenPressed = false;
bool button2HasBeenPressed = false;

long lastInterruptTime = 0;
int position = 0;

int milisIn = 0;
int milisIn2 = 0;

bool street_A_IN = false;
bool street_B_IN = false;

void func(int sockfd);
void *thread_func (void *arg);
void updateReportsLength ();
void addReport (int way, int speed, bool violation);
light setupLight (int color, int pin);
void setupSemaphore ();
void setState (int s1, int s2, int s3, int s4, int s5, int s6);
void timer (state S);
void callback_button1 (void);
void callback_button2 (void);
void sensor_aux (void);
void sensor_aux2 (void);
void callback_speed_in (void);
void callback_speed_out (void);
void callback_speed_in2 (void);
void callback_speed_out2 (void);

int main () {
    pthread_create(&threadA, NULL, thread_func, NULL);
    if(wiringPiSetup() == -1) return 1;

    state states[6];

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
    states[2].s4 = 0;
    states[2].s5 = 0;
    states[2].s6 = 1;

    states[2].time = 1000;
    states[2].minimum = false;
    
    states[3].s1 = 0;
    states[3].s2 = 0;
    states[3].s3 = 1;
    states[3].s4 = 1;
    states[3].s5 = 0;
    states[3].s6 = 0;

    states[3].time = 10000;
    states[3].minimum = true;

    states[4].s1 = 0;
    states[4].s2 = 0;
    states[4].s3 = 1;
    states[4].s4 = 0;
    states[4].s5 = 1;
    states[4].s6 = 0;

    states[4].time = 3000;
    states[4].minimum = false;

    states[5].s1 = 0;
    states[5].s2 = 0;
    states[5].s3 = 1;
    states[5].s4 = 0;
    states[5].s5 = 0;
    states[5].s6 = 1;

    states[5].time = 1000;
    states[5].minimum = false;

    setupSemaphore();
    
    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);

    pinMode(SENSOR_CAR_1, INPUT);
    pinMode(SENSOR_CAR_2, INPUT);

    pullUpDnControl(BUTTON_1, PUD_UP);
    pullUpDnControl(BUTTON_2, PUD_UP);

    pullUpDnControl(SENSOR_CAR_1, PUD_UP);
    pullUpDnControl(SENSOR_CAR_2, PUD_UP);

    wiringPiISR(BUTTON_1, INT_EDGE_RISING, &callback_button1);
    wiringPiISR(BUTTON_2, INT_EDGE_RISING, &callback_button2);

    wiringPiISR(SENSOR_CAR_2, INT_EDGE_RISING, &sensor_aux2);
    wiringPiISR(SENSOR_CAR_1, INT_EDGE_RISING, &sensor_aux);

    wiringPiISR(SENSOR_SPEED_1, INT_EDGE_FALLING, &callback_speed_out);
    wiringPiISR(SENSOR_SPEED_2, INT_EDGE_RISING, &callback_speed_in);

    wiringPiISR(SENSOR_SPEED_3, INT_EDGE_RISING, &callback_speed_in2);
    wiringPiISR(SENSOR_SPEED_4, INT_EDGE_FALLING, &callback_speed_out2);

    while(1) {
        timer(states[position]);
        position++;
        if( position == 6 ) {
            position = 0;
        }
    }

    free(reports);
    return 0;
}

void func(int sockfd) {
    char buff[MAX];
    int n;  

    while(1) {
        bzero(buff, sizeof(buff));
        printf("%d\n", reportsLength);
        n = 0;
        while(n < reportsLength) {
            buff[0] = reports[n].speed;
            buff[1] = reports[n].direction;
            buff[2] = reports[n].violation;
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            n++;
            if(n == reportsLength) {
                reportsLength = 0;
            }
        }
        delay(2000);        
    }
}

void *thread_func (void *arg) {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    sockfd = configureSocket();
    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
   
    connectSocket(sockfd, servaddr);
   
    func(sockfd);   
    close(sockfd);
}

void updateReportsLength () {
    reports = (report *) malloc((reportsLength + 1) * sizeof(report));
    reportsLength++;
}

void addReport (int way, int speed, bool violation) {
    updateReportsLength();
    reports[reportsLength - 1].direction = way;
    reports[reportsLength - 1].speed = speed;
    reports[reportsLength - 1].violation = violation;
}

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
            position = 3;
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
                position = 3;
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

void sensor_aux (void) {
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 300){
        bool violation = false;

        if (position == 3 || position == 4 || position == 5) {
            violation = true;
        }
        addReport(WAY_DOWN, 0, violation);
        button2HasBeenPressed = true;
    }
    lastInterruptTime = interruptTime;
}

void sensor_aux2 (void) {
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 300){
        bool violation = false;

        if (position == 3 || position == 4 || position == 5) {
            violation = true;
        }
        addReport(WAY_UP, 0, violation);
        button2HasBeenPressed = true;
    }
    lastInterruptTime = interruptTime;
}

void callback_speed_in (void) {
    milisIn = millis();
    street_A_IN = true;
}

void callback_speed_out (void) {
    long milisOut = millis();
    if(street_A_IN) {
        int v = DISTANCE / (milisOut - milisIn);
        int total = v * 3.6;
        bool violation = false;

        if (total > 60 || position == 0 || position == 1 || position == 2) {
            violation = true;
            printf("Passou sinal vermelho 1\n");
        }
        addReport(WAY_RIGHT, total, violation);
        street_A_IN = false;
    } else {
        addReport(WAY_RIGHT, 0, false);
    }
}

void callback_speed_in2 (void) {
    long milisIn2 = millis();
    street_B_IN = true;
}

void callback_speed_out2 (void) {
    long milisOut = millis();
    if(street_B_IN) {
        int v = DISTANCE / (milisOut - milisIn2);
        int total = v * 3.6;
        bool violation = false;

        if (total > 60 || position == 0 || position == 1 || position == 2) {
            violation = true;
            printf("Passou sinal vermelho 2\n");
        }
        addReport(WAY_LEFT, total, violation);
        street_B_IN = false;
    } else {
        addReport(WAY_LEFT, 0, false);
    }
}

