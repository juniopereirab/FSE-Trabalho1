#ifndef DEFINITION_H
#define DEFINITION_H

#define true 1
#define false 0
typedef int bool;

#define DISTANCE 1000 // ( 1 meter * 1000 ms )

#define WAY_RIGHT 1 // Indo para direita (Via principal)
#define WAY_LEFT 2 // Indo para esquerda (Via principal)
#define WAY_UP 3 // Indo para cima (Via auxiliar)
#define WAY_DOWN 4 // Indo para baixo (Via auxiliar)

struct Report {
    int speed;
    int direction;
    bool violation;
} typedef report;

#endif