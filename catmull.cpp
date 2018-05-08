#include <iostream>
#include <math.h>
#include <stdio.h>
#include <vector>
#include "catmull.hpp"
#ifdef __APPLE__
#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

void norm(float *p, float *p1, float *p2, float *res){
	//normal em p

	float v1[3];
	float v2[3];

	v1[0] = p1[0] - p[0];
	v1[1] = p1[1] - p[1];
	v1[2] = p1[2] - p[2];


	v2[0] = p2[0] - p[0];
	v2[1] = p2[1] - p[1];
	v2[2] = p2[2] - p[2];


	cross(v1,v2,res);
	normalize(res);
}

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
    if(l==0.0f){
      a[0]=0.0f; a[1]=0.0f; a[2]=0.0f;
    }
}

void vetMat(float t[4], float m[4][4], float *res){
	for(int i = 0;i < 4; i++){
		res[i]=0.0;
		for(int j = 0;j < 4;j++){
			res[i] += t[j]*m[j][i];
		}
	}
}

void vetMat1(float t[4], float m[4][1], float *res,int i){
	res[i] = 0.0;
	for(int j = 0;j < 4;j++){
		res[i] += t[j]*m[j][0];
	}
}

void normBezier(float u,float v, int *indices, float *res, float *p){
	float aux2[4],aux3[4],tg1[3],tg2[3];
	float aa[4];

	float uu[4] = {u*u*u, u*u, u, 1};
	float uuDeriv[4] = {3*u*u, 2*u, 1, 0};
  float vv[4][1] = {{v*v*v},
                    {v*v},
                    {v},
                    {1}};

  float vvDeriv[4][1] = {{3*v*v},
                         {2*v},
                         {1},
                         {0}};


	float mm[4][4] = {{-1.0f,  3.0f, -3.0f,  1.0f},
                    { 3.0f, -6.0f,  3.0f,  0.0f},
                    {-3.0f,  3.0f,  0.0f,  0.0f},
                    { 1.0f,  0.0f,  0.0f,  0.0f}};

    float xx[4][4] = {p[3*indices[0]],p[3*indices[4]],p[3*indices[8]],p[3*indices[12]],
                      p[3*indices[1]],p[3*indices[5]],p[3*indices[9]],p[3*indices[13]],
                      p[3*indices[2]],p[3*indices[6]],p[3*indices[10]],p[3*indices[14]],
                      p[3*indices[3]],p[3*indices[7]],p[3*indices[11]],p[3*indices[15]]};

    float yy[4][4] = {p[3*indices[0]+1],p[3*indices[4]+1],p[3*indices[8]+1],p[3*indices[12]+1],
                      p[3*indices[1]+1],p[3*indices[5]+1],p[3*indices[9]+1],p[3*indices[13]+1],
                      p[3*indices[2]+1],p[3*indices[6]+1],p[3*indices[10]+1],p[3*indices[14]+1],
                      p[3*indices[3]+1],p[3*indices[7]+1],p[3*indices[11]+1],p[3*indices[15]+1]};

    float zz[4][4] = {p[3*indices[0]+2],p[3*indices[4]+2],p[3*indices[8]+2],p[3*indices[12]+2],
                      p[3*indices[1]+2],p[3*indices[5]+2],p[3*indices[9]+2],p[3*indices[13]+2],
                      p[3*indices[2]+2],p[3*indices[6]+2],p[3*indices[10]+2],p[3*indices[14]+2],
                      p[3*indices[3]+2],p[3*indices[7]+2],p[3*indices[11]+2],p[3*indices[15]+2]};

    //U'*M*P*M*V                 			
    aa[0] = 0.0; aa[1] = 0.0; aa[2] = 0.0; aa[3] = 0.0;
  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
  	aux3[0] = 0.0; aux3[1] = 0.0; aux3[2] = 0.0; aux3[3] = 0.0;
    tg1[0] = 0.0; tg1[1] = 0.0; tg1[2] = 0.0;
    tg2[0] = 0.0; tg2[1] = 0.0; tg2[2] = 0.0;


  	vetMat(uuDeriv,mm,aa);


    vetMat(aa,xx,aux2);
    vetMat(aux2,mm,aux3);
    vetMat1(aux3,vv,tg1,0);

  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
  	aux3[0] = 0.0; aux3[1] = 0.0; aux3[2] = 0.0; aux3[3] = 0.0;

  	vetMat(aa,yy,aux2);
    vetMat(aux2,mm,aux3);
    vetMat1(aux3,vv,tg1,1);

  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
  	aux3[0] = 0.0; aux3[1] = 0.0; aux3[2] = 0.0; aux3[3] = 0.0;

  	vetMat(aa,zz,aux2);
    vetMat(aux2,mm,aux3);
    vetMat1(aux3,vv,tg1,2);


    //U*M*P*M*V'
    aa[0] = 0.0; aa[1] = 0.0; aa[2] = 0.0; aa[3] = 0.0;
    aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
    aux3[0] = 0.0; aux3[1] = 0.0; aux3[2] = 0.0; aux3[3] = 0.0;

    vetMat(uu,mm,aa);


    vetMat(aa,xx,aux2);
    vetMat(aux2,mm,aux3);
    vetMat1(aux3,vvDeriv,tg2,0);

  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
  	aux3[0] = 0.0; aux3[1] = 0.0; aux3[2] = 0.0; aux3[3] = 0.0;

  	vetMat(aa,yy,aux2);
    vetMat(aux2,mm,aux3);
    vetMat1(aux3,vvDeriv,tg2,1);

  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
  	aux3[0] = 0.0; aux3[1] = 0.0; aux3[2] = 0.0; aux3[3] = 0.0;

  	vetMat(aa,zz,aux2);
    vetMat(aux2,mm,aux3);
    vetMat1(aux3,vvDeriv,tg2,2);

    normalize(tg1);
    normalize(tg2);
    cross(tg1,tg2,res);
    normalize(res);

}

void getBezierPoint(float u,float v, int *indices, float *res, float *p) {

	float aux[4],aux2[4];
  	float uu[4] = {u*u*u, u*u, u, 1};
  	float vv[4][1] = {{v*v*v},
                      {v*v},
                      {v},
                      {1}};
  	float aa[4];

  	float mm[4][4] = {{-1.0f,  3.0f, -3.0f,  1.0f},
                    { 3.0f, -6.0f,  3.0f,  0.0f},
                    {-3.0f,  3.0f,  0.0f,  0.0f},
                    { 1.0f,  0.0f,  0.0f,  0.0f}};

  	float xx[4][4] = {p[3*indices[0]],p[3*indices[4]],p[3*indices[8]],p[3*indices[12]],
            		      p[3*indices[1]],p[3*indices[5]],p[3*indices[9]],p[3*indices[13]],
            		      p[3*indices[2]],p[3*indices[6]],p[3*indices[10]],p[3*indices[14]],
            		      p[3*indices[3]],p[3*indices[7]],p[3*indices[11]],p[3*indices[15]]};

    float yy[4][4] = {p[3*indices[0]+1],p[3*indices[4]+1],p[3*indices[8]+1],p[3*indices[12]+1],
            		      p[3*indices[1]+1],p[3*indices[5]+1],p[3*indices[9]+1],p[3*indices[13]+1],
            		      p[3*indices[2]+1],p[3*indices[6]+1],p[3*indices[10]+1],p[3*indices[14]+1],
            		      p[3*indices[3]+1],p[3*indices[7]+1],p[3*indices[11]+1],p[3*indices[15]+1]};

    float zz[4][4] = {p[3*indices[0]+2],p[3*indices[4]+2],p[3*indices[8]+2],p[3*indices[12]+2],
            		      p[3*indices[1]+2],p[3*indices[5]+2],p[3*indices[9]+2],p[3*indices[13]+2],
            		      p[3*indices[2]+2],p[3*indices[6]+2],p[3*indices[10]+2],p[3*indices[14]+2],
            		      p[3*indices[3]+2],p[3*indices[7]+2],p[3*indices[11]+2],p[3*indices[15]+2]};

  	//pontos
  	aa[0] = 0.0; aa[1] = 0.0; aa[2] = 0.0; aa[3] = 0.0;
  	aux[0] = 0.0; aux[1] = 0.0; aux[2] = 0.0; aux[3] = 0.0;
  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
  	res[0] = 0.0; res[1] = 0.0; res[2] = 0.0;

  	vetMat(uu,mm,aa);

  	vetMat(aa,xx,aux);
  	vetMat(aux,mm,aux2);
  	vetMat1(aux2,vv,res,0);


  	aux[0] = 0.0; aux[1] = 0.0; aux[2] = 0.0; aux[3] = 0.0;
  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;
  
  	vetMat(aa,yy,aux);
  	vetMat(aux,mm,aux2);
  	vetMat1(aux2,vv,res,1);


  	aux[0] = 0.0; aux[1] = 0.0; aux[2] = 0.0; aux[3] = 0.0;
  	aux2[0] = 0.0; aux2[1] = 0.0; aux2[2] = 0.0; aux2[3] = 0.0;

  	vetMat(aa,zz,aux);
  	vetMat(aux,mm,aux2);
  	vetMat1(aux2,vv,res,2);


}


void getCatmullRomPoint(float t, int *indices, float *res, float *deriv, float *p) {

	float tt[4] = {t*t*t, t*t, t, 1};
	float tderiv[4] = {3*t*t, 2*t, 1, 0};
	float aa[4];

	// catmull-rom matrix
	float mm[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f}};
						

	float pp[4][4] = {p[3*indices[0]],p[3*indices[0]+1],p[3*indices[0]+2],1,
					  p[3*indices[1]],p[3*indices[1]+1],p[3*indices[1]+2],1,
					  p[3*indices[2]],p[3*indices[2]+1],p[3*indices[2]+2],1,
					  p[3*indices[3]],p[3*indices[3]+1],p[3*indices[3]+2],1};

	//pontos
	aa[0] = 0.0; aa[1] = 0.0; aa[2] = 0.0; aa[3] = 0.0;
	res[0] = 0.0; res[1] = 0.0; res[2] = 0.0; res[3] = 0.0;
	vetMat(tt,mm,aa);
	vetMat(aa,pp,res);

	//derivada
	aa[0] = 0.0; aa[1] = 0.0; aa[2] = 0.0; aa[3] = 0.0;
	vetMat(tderiv,mm,aa);
	vetMat(aa,pp,deriv);

}

void getGlobalCatmullRomPoint(float gt, float *res, float *deriv,int size,float *p) {

	float t = gt * size; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment
	// indices store the points
	int indices[4]; 
	indices[0] = (index + size-1)%size;	
	indices[1] = (indices[0]+1)%size;
	indices[2] = (indices[1]+1)%size; 
	indices[3] = (indices[2]+1)%size;

	getCatmullRomPoint(t, indices, res, deriv,p);
}

void renderCatmull(float t,float *y,int size,float *p){
	float res[3],x[3],z[3],deriv[3];
	float rot[16];

	float a = 0;
	//pontos
	/*glBegin(GL_LINE_LOOP);
	for(int i = 1; i < 100; i++){
		a+=0.01f;
		getGlobalCatmullRomPoint(a,res,deriv,size,p);
		glVertex3f(res[0],res[1],res[2]);
	}
	glEnd();*/
	
	res[0] = 0.0f;res[1] = 0.0f; res[2] = 0.0f;
	deriv[0] = 0.0f;deriv[1] = 0.0f;deriv[2] = 0.0f;
	getGlobalCatmullRomPoint(t,res,deriv,size,p);
	
	x[0] = deriv[0]; x[1] = deriv[1]; x[2] = deriv[2];
	normalize(x);
	if (t==0.0f && abs(x[1]) == y[1]){ ///apenas para o caso do x inicial ser {0,1,0} ou {0,-1,0}
		y[0] = 1;						//pois nesse caso o cross()	da vetor nulo e a normalize()
		y[1] = 1;						//poe o vetor z como 'nan'
		y[2] = 1;
		normalize(y);
	}

	cross(x,y,z);
	normalize(z);

	cross(z,x,y);
	normalize(y);
	

	buildRotMatrix(x,y,z,rot);
	glTranslatef(res[0],res[1],res[2]);
	glMultMatrixf(rot);
}