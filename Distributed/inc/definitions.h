#ifndef DEFINITION_H
#define DEFINITION_H

#define GREEN 8
#define YELLOW 9
#define RED 14

#define GREEN_2 30
#define YELLOW_2 21
#define RED_2 22

#define BUTTON_1 12
#define BUTTON_2 13

#define SENSOR_CAR_1 7
#define SENSOR_CAR_2 0

#define SENSOR_SPEED_1 2 // Passa Carro 1
#define SENSOR_SPEED_2 3 // Para Carro 1
#define SENSOR_SPEED_3 23 // Passa Carro 2
#define SENSOR_SPEED_4 24 // Para Carro 2

#define true 1
#define false 0
typedef int bool;

#define COLOR_GREEN 1
#define COLOR_YELLOW 2
#define COLOR_RED 3

#define DISTANCE 1000 // ( 1 meter * 1000 ms )

#define WAY_RIGHT 1 // Indo para direita (Via principal)
#define WAY_LEFT 2 // Indo para esquerda (Via principal)
#define WAY_UP 3 // Indo para cima (Via auxiliar)
#define WAY_DOWN 4 // Indo para baixo (Via auxiliar)

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

struct Report {
    int speed;
    int direction;
    bool violation;
} typedef report;

#endif