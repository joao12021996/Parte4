#ifndef CATMULL
#define CATMULL
void norm(float *a, float *b, float *c, float *res);
void normBezier(float u,float v, int *indices, float *res, float *p);
void buildRotMatrix(float *x, float *y, float *z, float *m);
void cross(float *a, float *b, float *res);
void normalize(float *a);
void getGlobalCatmullRomPoint(float gt, float *res, float *deriv);
void vetMat(float t[4], float m[4][4], float *res);
void getCatmullRomPoint(float t, int *indices, float *res, float *deriv);
void renderCatmull(float t,float y[3],int size,float *p);
void getBezierPoint(float u,float v, int *indices, float *res, float *p);
void vetMat1(float t[4], float m[4][1], float *res,int i);
#endif