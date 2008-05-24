#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#ifdef _WIN32
#define GLUT_BUILDING_LIB
#endif
#include <GL/glut.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	double x;
	double y;
} SPoint;

typedef struct {
	float red;
	float green;
	float blue;
} SColor;

void UnProject(GLdouble* worldX, GLdouble* worldY, GLdouble* worldZ,
			   int screenX,	int screenY, int screenZ /*= 0.0*/);

void DrawPoint(int x, int y, SColor* color /*= NULL*/);
void DrawLine(double x1, double y1, double x2, double y2, SColor* color1 /*= NULL*/, SColor* color2 /*= NULL*/);
void DrawRectangle(double x1, double y1, double x2, double y2);
void DrawScanline(int x, int y, SColor* scanline, int numPixels);
void GetPixel(SColor* color, int x, int y);

void WritePPM(const char* Filename);

int GetWindowHeight();
#ifdef __cplusplus
}
#endif
#endif /* __UTILITIES_H__*/
