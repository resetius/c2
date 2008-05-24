/*  $Id: vizgl2.c 363 2005-10-11 14:40:47Z manwe $                                                           */

/* Copyright (c) 2003, 2004 Alexey Ozeritsky
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Alexey Ozeritsky.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*  3D vizializer
 *
 * Часть кода взята из примера gear библиотеки QT
 */

#ifdef __WIN32__
#define GLUT_BUILDING_LIB
#include <windows.h>
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "asp_lib.h"

void initializeGL();

/*сколько точек рисовать ?*/
#define RCONST 50000

int data_type=0; /*0 - plain text, 1 - binary*/
int *bounds; /*границы между объектами при множественной
                           визуализации
                        (количество точек в объектах)*/
int boundsc; /*размер массива bounds*/

char **confs; /*файлы визуализации*/
int confsc; /*из количество*/

int NN; /*размерность массивов*/
double *data_x,*data_y,*data_z; /*массивы с координатами*/

double xa=0,ya=0,za=0.0;
double xo=-5,yo=-2,zo=1.0;
double dx,dy,dz; /*центр*/

double minx=1e10,maxx=-1e10;
double miny=1e10,maxy=-1e10;
double minz=1e10,maxz=-1e10;


double omx,omy;

GLfloat colors[][4]={
	{1.0,0.0,0.0,1.0},
	{0.0,1.0,0.0,1.0},
	{0.0,0.0,1.0,1.0},
	{1.0,1.0,0.0,1.0},
	{0.0,1.0,1.0,1.0},
	{1.0,0.0,1.0,1.0},
	{1.0,1.0,1.0,1.0},
	{0.0,0.0,0.0,1.0}
};


int points=1;
/*
 * Draw a curve
 */
static void gear()
{
	extern int *bounds;
	extern int boundsc;
	int i=0,j=0,k=0,l=0;
	GLenum mode=GL_LINE_STRIP;
	if(points) mode=GL_POINTS;
	
	for ( i=0; i<boundsc;i++ )
	{
		/*glColor4dv(colors[k]);*/
		printf("%d:%d\n",i,bounds[i]);
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colors[k] );
		glBegin(mode);
		/*for(int i=0;i<NN;i++)*/
		for(l=j;l<bounds[i];l++)
		{
			glVertex3d(data_x[l],data_y[l],data_z[l]);
		}
		glEnd();
		j=bounds[i];
		k+=1;
	}
}

static void cube()
{
/*перёд*/
	glBegin(GL_LINE_LOOP);
		glVertex3d(minx,miny,minz);
		glVertex3d(minx,miny,maxz);
		glVertex3d(maxx,miny,maxz);
		glVertex3d(maxx,miny,minz);
	glEnd();
/*зад*/
	glBegin(GL_LINE_LOOP);
		glVertex3d(minx,maxy,minz);
		glVertex3d(minx,maxy,maxz);
		glVertex3d(maxx,maxy,maxz);
		glVertex3d(maxx,maxy,minz);
	glEnd();
/*лево*/
	glBegin(GL_LINE_LOOP);
		glVertex3d(minx,miny,minz);
		glVertex3d(minx,miny,maxz);
		glVertex3d(minx,maxy,maxz);
		glVertex3d(minx,maxy,minz);
	glEnd();
/*право*/
	glBegin(GL_LINE_LOOP);
		glVertex3d(maxx,miny,minz);
		glVertex3d(maxx,miny,maxz);
		glVertex3d(maxx,maxy,maxz);
		glVertex3d(maxx,maxy,minz);
	glEnd();
/*низ*/
	glBegin(GL_LINE_LOOP);
		glVertex3d(minx,miny,minz);
		glVertex3d(minx,maxy,minz);
		glVertex3d(maxx,maxy,minz);
		glVertex3d(maxx,miny,minz);
	glEnd();
/*верх*/
	glBegin(GL_LINE_LOOP);
		glVertex3d(minx,miny,maxz);
		glVertex3d(minx,maxy,maxz);
		glVertex3d(maxx,maxy,maxz);
		glVertex3d(maxx,miny,maxz);
	glEnd();

}

static GLfloat view_rotx=20.0, view_roty=30.0, view_rotz=0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;


char ** loadconfs(char *conf)
{
	extern int confsc;
	extern int data_type;
	char **ret=(char**)malloc(256*sizeof(char*));
	char cfg[256];
	char type[255];
	
	FILE *file=fopen(conf,"r");
	
	confsc=0;
	
	NOMEM(ret);
	IOERR(file);
	fscanf(file,"%250s",type);
	if(strcmp(type,"binary")==0)
		data_type=1;
	
	while(fscanf(file,"%250s",cfg)>0)
	{
		ret[confsc]=(char*)malloc((strlen(cfg)+1)*sizeof(char));
		strcpy(ret[confsc],cfg);
		confsc+=1;
	}
	fclose(file);
	
	return ret;
}

void loaddata_text()
{
	extern int confsc;
	extern int boundsc;
	extern int *bounds;

	double t;
	double x1,y1,z1;
	double Q;
	
	int M;
	int i=0,j=0;
	int files=0;

	bounds=(int*)malloc(confsc*sizeof(int));
	boundsc=confsc;
	NOMEM(bounds);

	fprintf(stdout,"Number of points ... "); fflush(stdout);

	/*число точек*/
	for(i=0;i<confsc;i++)
	{
		FILE *f=fopen(confs[i],"r");
		IOERR(f);
		
		while(1)
		{
			if(fscanf(f,"%lf",&t)!=1) break;
			if(fscanf(f,"%lf",&t)!=1) break;
			if(fscanf(f,"%lf",&t)!=1) break;		
			j+=1;
		}
		fclose(f);
	}
	/*bounds=b;*/
	
	M=j;
	NN=M;
/*	NN=RCONST;*/
	printf("%d\n",M);
	data_x=(double*)malloc(M*sizeof(double));
	data_y=(double*)malloc(M*sizeof(double));
	data_z=(double*)malloc(M*sizeof(double));
	
	NOMEM(data_x); 
	NOMEM(data_y); 
	NOMEM(data_z);

	fprintf(stdout,"confs::%d\n",confsc);
	printf("Loading text data.... ");fflush(stdout);
	
/*	Q=(double)M/(double)NN;*/
/*	printf("Q=%f\n",Q);*/

	j=0;
	for(i=0;i<confsc;i++)
	{
		int k=0;
		FILE *f=fopen(confs[i],"r");
		printf("\nLoading text data from '%s' ...",confs[i]);fflush(stdout);
		IOERR(f);

		while(1)
		{
			if(fscanf(f,"%lf",&x1)<0||fscanf(f,"%lf",&y1)<0||fscanf(f,"%lf",&z1)<0)
				break;
		
			data_x[j]=x1;
			data_y[j]=y1;
			data_z[j]=z1;
			j+=1;
		}
		printf("done\n");
		k=i;
		bounds[i]=j;
		fclose(f);
	}
	printf("done\n");
}

#ifdef BUF_SIZE
#undef BUF_SIZE
#endif
#define BUF_SIZE 8192 

void loaddata_binary()
{
	extern int confsc;
	extern int boundsc;
	extern int *bounds;

	double t;
	double x1,y1,z1;
	double Q;
	int M;
	int files=0;
	int i=0,j=0,k=0,n=0;
	int read_count=0;
	double buf[3*BUF_SIZE];

	bounds=(int*)malloc(confsc*sizeof(int));
	boundsc=confsc;
	NOMEM(bounds);

	fprintf(stdout,"confs::%d\n",confsc);
	fprintf(stdout,"Number of points ... "); fflush(stdout);
	
	/*число точек*/
	for(j=0,i=0;i<confsc;i++)
	{
		FILE *f=fopen(confs[i],"rb");
		IOERR(f);
		
		while(1)
		{
			read_count=fread(buf,sizeof(double),BUF_SIZE,f);
			j+=read_count;
			/*fprintf(stderr,"read: %d\n",read_count);*/
			if(read_count<BUF_SIZE) break;
		}
		fclose(f);
	}
	fprintf(stdout,"%d\n",j);
	j/=3;
	M=j;
	NN=M;

	fprintf(stdout,"%d\n",M);
	data_x=(double*)malloc(M*sizeof(double));
	data_y=(double*)malloc(M*sizeof(double));
	data_z=(double*)malloc(M*sizeof(double));
	
	NOMEM(data_x); 
	NOMEM(data_y); 
	NOMEM(data_z);
	
	printf("Loading binary data.... ");fflush(stdout);
	
	j=0; n=0;
	for(i=0;i<confsc;i++)
	{
		int k=0;
		FILE *f=fopen(confs[i],"rb");
		printf("\nLoading binary data from '%s' ...",confs[i]);
		fflush(stdout);
		IOERR(f);

		while(1)
		{
			read_count=fread(buf,sizeof(double),3*BUF_SIZE,f);
			
			for(k=0;k<read_count;k+=3,n++)
			{
				data_x[n]=buf[k];
				data_y[n]=buf[k+1];
				data_z[n]=buf[k+2];
				/*printf("%le %le %le\n",data_x[*/
			}
		
			j+=read_count/3;
			if(read_count<3*BUF_SIZE) break;
			
		}
		printf("done\n");
		k=i;
		bounds[i]=j;
		fclose(f);
	}
	printf("done\n");
}

#undef BUF_SIZE

void loaddata()
{
	extern char **confs;
	int i=0;

	confs=loadconfs("vizgl.conf");
	
	if(data_type==1)
	{
		printf("Data Type::Binary\n");
		loaddata_binary();
		printf("ok\n");
	}
	else
	{
		printf("Data Type::Text\n");
		loaddata_text();
	}

	printf("Finding center....\n");fflush(stdout);
	for(i=0;i<NN;i++)
	{
		if(data_x[i]>maxx) maxx=data_x[i];
		if(data_y[i]>maxy) maxy=data_y[i];
		if(data_z[i]>maxz) maxz=data_z[i];
		
		if(data_x[i]<minx) minx=data_x[i];
		if(data_y[i]<miny) miny=data_y[i];
		if(data_z[i]<minz) minz=data_z[i];
	}
	
	dx=(maxx+minx)/2.0;
	dy=(maxy+miny)/2.0;
	dz=(maxz+minz)/2.0;
	printf("x=%f y=%f z=%f\n",dx,dy,dz);
	printf("minx=%e miny=%e minz=%e\n",minx,miny,minz);
	printf("maxx=%e maxy=%e maxz=%e\n",maxx,maxy,maxz);
	for(i=0;i<NN;i++)
	{
		data_x[i]-=dx;
		data_y[i]-=dy;
		data_z[i]-=dz;
	}
	minx-=dx; maxx-=dx;
	miny-=dy; maxy-=dy;
	minz-=dz; maxz-=dz;
	printf("done\n");
}

static void draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    glTranslatef(0.0,0.0,-100);    
    glScalef(zo,zo,zo);
    glRotatef(xa,1.0,0.0,0.0);
    glRotatef(ya,0.0,1.0,0.0);
    glRotatef(za,0.0,0.0,1.0);

    glCallList(gear1);
}


static int timer_interval = 10;			/* timer interval (millisec)*/


void updateGL()
{
	glutPostRedisplay();
}

void mousePressEvent ( int button, int state, int x, int y )
{

	omx=x;
	omy=y;
}

void mouseMoveEvent ( int x, int y )
{
	int mx,my;
	mx=x;
	my=y;

	/*printf("%f %f %f %f\n",omx,omy,mx,my);*/
	xa+=my-omy;
	ya+=mx-omx;
	xa=(int)xa%360;
	ya=(int)ya%360;
	updateGL();

	omx=mx;
	omy=my;
}


void keyPressEvent1 ( unsigned char key, int x, int y )
{
	/*тип прорисовки: 1- точками или 0-линиями*/
	extern int points;
	switch(key){
	case '[': /*вращения по z*/
		za+=10;
		break;
	case ']':
		za+=-10;
		break;
	case 'm':
		points=1;
		initializeGL();
		break;
	case 'n':
		points=0;
		initializeGL();
		break;
	case 'q':/*выход*/
		exit(0);
		break;
	default:
		break;
	}
	
	xa=(int)xa%360;
	ya=(int)ya%360;
	za=(int)za%360;
	
	updateGL();	
}

void keyPressEvent2 ( int key, int x, int y )
{
	/*тип прорисовки: 1- точками или 0-линиями*/
	extern int points;
	switch(key){
	case GLUT_KEY_UP:
		xa+=-10;
		break;
	case GLUT_KEY_DOWN:
		xa+=10;
		break;
	case GLUT_KEY_RIGHT:
		ya+=10;
		break;
	case GLUT_KEY_LEFT:
		ya+=-10.;
		break;
	case GLUT_KEY_F5: /*плюс*/
		zo+=0.25;
		break;
	case GLUT_KEY_F6: /*минус*/
		zo-=0.25;
		break;
	default:
		break;
	}
	
	xa=(int)xa%360;
	ya=(int)ya%360;
	za=(int)za%360;
	
	updateGL();	
}

/*GearWidget::GearWidget( QWidget *parent, const char *name )
     : QGLWidget( parent, name )
{
	loaddata();*/
/*    startTimer( timer_interval );*/
/*}*/

void initializeGL()
{
     static GLfloat pos[4] = {5.0, 5.0, 10.0, 1.0 };
/*     static GLfloat pos[4] = {0.0, 0.0, 0.0, 1.0 };*/
     static GLfloat ared[4] = {0.8, 0.1, 0.0, 1.0 };
/*    static GLfloat agreen[4] = {0.0, 0.8, 0.2, 1.0 };*/
     static GLfloat ablue[4] = {0.2, 0.2, 1.0, 1.0 };


/*    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glDepthFunc(GL_LEQUAL);
     glShadeModel(GL_SMOOTH);*/


     glLightfv( GL_LIGHT0, GL_POSITION, pos );
     
     /*glCullFace(GL_BACK);
     glEnable( GL_CULL_FACE );*/
     
     glEnable( GL_LIGHTING );
     glEnable( GL_LIGHT0 );
     glEnable( GL_DEPTH_TEST );

     /*glFrontFace(GL_CW);*/
    /* make the gears */
     gear1 = glGenLists(1);
     glNewList(gear1, GL_COMPILE);
/*     glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ared );
    glColor3b(255,0,0);*/
     gear();
     glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ablue );
/*    glColor3b(0,0,255);*/
     cube();

     glEndList();

/*     gear2 = glGenLists(1);
     glNewList(gear2, GL_COMPILE);
     glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, agreen );
     gear( 0.5, 2.0, 2.0, 10, 0.7 );
     glEndList();

     gear3 = glGenLists(1);
     glNewList(gear3, GL_COMPILE);
     glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ablue );
     gear( 1.3, 2.0, 0.5, 10, 0.7 );
     glEndList();
    qglClearColor(white);
    glEnable( GL_NORMALIZE );*/
}


void resizeGL( int width, int height )
{
    GLfloat w = (float) width / (float) height;
    GLfloat h = 1.0;

    glViewport( 0, 0, width, height );

    glMatrixMode(GL_PROJECTION);
    /*glMatrixMode(GL_MODELVIEW);*/
    glLoadIdentity();
    /*glFrustum( -w, w, -h, h, 5, 100.0 );*/
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -100.0 );
}


void paintGL()
{
    /*printf("%d\n",glIsEnabled(GL_DEPTH_TEST));*/
    draw();
    /*glFlush();*/
    glutSwapBuffers();
}

/* void GearWidget::timerEvent(QTimerEvent*)
 {
     updateGL();
 }*/


void registerEvents()
{
	glutDisplayFunc(paintGL);
	glutReshapeFunc(resizeGL);
	glutMouseFunc(mousePressEvent);
	glutMotionFunc(mouseMoveEvent);
	glutKeyboardFunc(keyPressEvent1);
	glutSpecialFunc(keyPressEvent2);
}

int main( int argc, char **argv )
{
	char rev[]="1.22";
	fprintf(stderr,"%s, revision %s\n",__FILE__,rev);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512,384);
	glutCreateWindow("3DViz2 1.22");
	loaddata();
	initializeGL();
	registerEvents();
	glutMainLoop();
	return 1;
}
