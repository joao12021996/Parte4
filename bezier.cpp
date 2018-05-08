#include <iostream>
#include <math.h>
#include <stdio.h>
#include <vector>
#ifdef __APPLE__
#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


void buildRotMatrix(float *x, float *y, float *z, float *m){
	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0; 
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0; 
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0; 
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) { 
	res[0] = a[1]*b[2] - a[2]*b[1]; 
	res[1] = a[2]*b[0] - a[0]*b[2]; 
	res[2] = a[0]*b[1] - a[1]*b[0];}


void normalize(float *a) { 
	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]); 
	a[0] = a[0]/l; a[1] = a[1]/l; a[2] = a[2]/l;
}

void vetMat(float t[4], float m[4][4], float *res){
	for(int i = 0;i < 4; i++){
		res[i]=0.0;
		for(int j = 0;j < 4;j++){
			res[i] += t[j]*m[j][i];

		}
	}
}

void getBezierPoint(float u,float v, int *indices, float *res, float *deriv, float *p) {

	float uu[4] = {u*u*u, u*u, u, 1};
	float vv[4] = {v*v*v, v*v, v, 1};
	float aa[4];
/*
	// catmull-rom matrix
	float mm[4][4] = {	{-1.0f,  3.0f, -3.0f,  1.0f},
						{ 3.0f, -6.0f,  3.0f,  0.0f},
						{-3.0f,  3.0f,  0.0f,  0.0f},
						{ 1.0f,  0.0f,  0.0f,  0.0f}};
						

	float pp[4][4] = {p[3*indices[0]],p[3*indices[0]+1],p[3*indices[0]+2],1,
					  p[3*indices[1]],p[3*indices[1]+1],p[3*indices[1]+2],1,
					  p[3*indices[2]],p[3*indices[2]+1],p[3*indices[2]+2],1,
					  p[3*indices[3]],p[3*indices[3]+1],p[3*indices[3]+2],1};

	//pontos
	aa[0] = 0.0; aa[1] = 0.0; aa[2] = 0.0; aa[3] = 0.0;
	res[0] = 0.0; res[1] = 0.0; res[2] = 0.0; res[3] = 0.0;
	vetMat(tt,mm,aa);
	vetMat(aa,pp,res);

	//derivada60
	aa[0] = 0.0; aa[1] = 0.0; aa[2] = 0.0; aa[3] = 0.0;
	vetMat(tderiv,mm,aa);
	vetMat(aa,pp,deriv);


	// matriz dos pontos de acordo com os indices dados. for u {for v} para receber a strip dos pontos.
	// quando u e v acabarem, passar para a prox lista de indicies e repetir o processo. depois e desenhar a strip
	// com os pontos dados.
}