#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "viz_utils.h"

/*
////////////////////////////////////////////////////////////////////////////////////////
// Function:	void UnProject(GLdouble* worldX, GLdouble* worldY, GLdouble* worldZ,
//							   int screenX,	int screenY, int screenZ)
// Arguments:	GLdouble *worldX, *worldY, *worldZ	:	world location of point	 (out)
//				int screenX, screenY, screenZ		:	screen location of point (in)
// Purpose:		Converts a point in screen to world space by unprojecting.
////////////////////////////////////////////////////////////////////////////////////////
*/

void UnProject(GLdouble* worldX, GLdouble* worldY, GLdouble* worldZ,
			   int screenX,	int screenY, int screenZ)
{
	GLint viewport[4];
	GLdouble modelviewMatrix[16], projectionMatrix[16];
	GLint realY;	/* OpenGL y coordinate position	*/

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

	realY = viewport[3] - (GLint)screenY - 1;

	gluUnProject( (GLdouble)screenX, (GLdouble)realY, (GLdouble)1.0,
		modelviewMatrix, projectionMatrix, viewport,
		worldX, worldY, worldZ );
}

/*
////////////////////////////////////////////////////////////////////////////////////////
// Function:	void DrawScanline(int x, int y, SColor* scanline, int numPixels)
// Arguments:	int x, y			:	starting raster position
//				SColor* scanline	:	array of pixel colors to copy along scanline
//				int numPixels		:	length of array to copy
// Purpose:		Copies an array of pixel colors to the screen.  The array is drawn
//				horizontally down the yth scanline starting at the xth pixel.
////////////////////////////////////////////////////////////////////////////////////////
*/
void DrawScanline(int x, int y, SColor* scanline, int numPixels)
{
	glRasterPos2i(x, y);
	glDrawPixels( numPixels, 1, GL_RGB, GL_FLOAT, scanline );
}

/*
////////////////////////////////////////////////////////////////////////////////////////
// The rest of the utility functions are fairly self explanatory.  WritePPM() copies the
// contents of the current openGL window to a PPM file.
////////////////////////////////////////////////////////////////////////////////////////
*/
void DrawPoint(int x, int y, SColor* color)
{
	if(color)
		glColor3d(color->red, color->green, color->blue);

	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void DrawLine(double x1, double y1, double x2, double y2, SColor* color1, SColor* color2)
{
	glBegin(GL_LINES);
	if(color1)
		glColor3d(color1->red, color1->green, color1->blue);
	glVertex2d(x1, y1);
	if(color2)
		glColor3d(color2->red, color2->green, color2->blue);
	glVertex2d(x2, y2);			
	glEnd();
}

void DrawRectangle(double x1, double y1, double x2, double y2)
{
	glBegin(GL_LINE_LOOP);
	glVertex2d(x1, y1);
	glVertex2d(x2, y1);
	glVertex2d(x2, y2);
	glVertex2d(x1, y2);
	glEnd();
}

int GetWindowHeight()
{
	GLint viewport[4];	
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	return viewport[3];
}

void GetPixel(SColor* color, int x, int y)
{		
	glReadPixels( x, GetWindowHeight() - y, 1, 1, GL_RGB, GL_FLOAT, &color->red);	
}

void WritePPM(const char* Filename)
{
	int RowCnt, ScanlineIndex;
	FILE* fp = 0;
	float* OutBuffer = 0;
	int width, height;
	GLint viewport[4];	
	glGetIntegerv(GL_VIEWPORT, viewport);
	width = viewport[2];
	height = viewport[3];

	/* Check for bad param*/
	if(Filename == 0)
		return;
	if(strlen(Filename) < 1)
		return;

	/*allocate space for the color data	*/
	OutBuffer  = (float*)malloc(3 * width * height * sizeof(float));
	glReadBuffer(GL_FRONT);

	/*Read the Framebuffer Data*/
	glReadPixels( 0, 0, width, height, GL_RGB, GL_FLOAT, OutBuffer);

	/*And Write the PPM*/
	fp = fopen(Filename, "w");

	if(!fp)
		return; /*error*/
	
	fprintf(fp, "P3\n%i %i\n255\n", width, height);

	/*Row Order is Reversed....*/
	for(RowCnt = height - 1; RowCnt >= 0; RowCnt--)
	{
		for(ScanlineIndex = 0; ScanlineIndex < width * 3; ScanlineIndex+=3)
		{
			int i = RowCnt * width * 3 + ScanlineIndex;
			fprintf(fp, "%i %i %i ", (int)(OutBuffer[i] * 255.0f), (int)(OutBuffer[i+1] * 255.0f), (int)(OutBuffer[i+2] * 255.0f));
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	free(OutBuffer);
}
