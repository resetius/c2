#ifndef C2_TURTLE_H
#define C2_TURTLE_H

struct State {
	int s;
	double m;
	int i;
	char * str;
};

extern struct State st;

#define NUMBER 256    /*умножить угол на число*/
#define COLOR 257     /*установить цвет*/
#define INCNUMBER 258 /*прибавить к углу значение (в градусах)*/
#define INCCOLOR  259 /*сместить цвет*/

#endif

