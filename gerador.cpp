#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "catmull.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#ifdef __APPLE__
#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

using namespace std;
vector<int> indices;
vector<float> pontos;




int main(int argc, char const *argv[]) {
  FILE *f,*k;
  char *garbage;
  float zero=0.0;
  f=fopen(argv[argc-1],"w");

  if(strcmp("bezier",argv[1])==0){
  	  k = fopen(argv[2],"r");
  	  int numP = 0;
  	  fscanf(k, "%d\n", &numP);
  	  vector<int> indices;
  	  vector<float> pontos;
  	  int aux[16];

  	  //indices
  	  for(int j = 0;j<numP;j++){
  	  	fscanf(k,"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",&aux[0],&aux[1],&aux[2],&aux[3],&aux[4],&aux[5],&aux[6],&aux[7],&aux[8],&aux[9],&aux[10],&aux[11],&aux[12],&aux[13],&aux[14],&aux[15]);
  	  	for(int i = 0;i<16;i++){
  	  		indices.push_back(aux[i]);
  	  	}
  	  }
  	  //pontos
      int numPontos = 0;
  	  float faux[3];
  	  fscanf(k, "%d\n", &numPontos);
  	  for(int j = 0;j<numPontos;j++){
  	  	fscanf(k,"%f, %f, %f\n",&faux[0],&faux[1],&faux[2]);
  	  	for(int i = 0;i<3;i++){
  	  		pontos.push_back(faux[i]);
  	  	}
  	  }
      /*for (int i = 1; i < pontos.size()+1; i++)
      {
        printf("%f ,  ",pontos[i-1]);
        if(i % 3 == 0){
          printf("\n\n");
        }
      }*/
  	 //todos os pontos de bezier
      float inc = 0.2f;
      float v = 0.0f;
      float v2 = inc;
      float u = 0.0f;
      float u2 = inc;

      int ind[16];
      float p1[3],p2[3],p3[3],p4[3];

      for(int i = 0; i < numP; i++){
        for(int j = 0;j < 16; j++){
          ind[j] = indices[j+(i*16)];
          //printf("%d  ",ind[j] );
        }
        //printf("\n");
        float n1[3],n2[3],n3[3],n4[4];
        v = 0.0f;
        v2 = inc;
        float a,b;
        for(int k = 0; k < 5; k++){
          u = 0.0f;
          u2 = inc;
          for(int t = 0; t < 5; t++){
            n1[0] = 0.0f; n1[1] = 0.0f; n1[2] = 0.0f;
            n2[0] = 0.0f; n2[1] = 0.0f; n2[2] = 0.0f;
            n3[0] = 0.0f; n3[1] = 0.0f; n3[2] = 0.0f;
            n4[0] = 0.0f; n4[1] = 0.0f; n4[2] = 0.0f;
            p1[0] = 0.0f; p1[1] = 0.0f; p1[2] = 0.0f;
            p2[0] = 0.0f; p2[1] = 0.0f; p2[2] = 0.0f;
            p3[0] = 0.0f; p3[1] = 0.0f; p3[2] = 0.0f;
            p4[0] = 0.0f; p4[1] = 0.0f; p4[2] = 0.0f;
            getBezierPoint(u,v,ind,p1,&pontos[0]);
            getBezierPoint(u,v2,ind,p2,&pontos[0]);
            getBezierPoint(u2,v,ind,p3,&pontos[0]);
            getBezierPoint(u2,v2,ind,p4,&pontos[0]);

            normBezier(u,v,ind,n1,&pontos[0]);
            normBezier(u,v2,ind,n2,&pontos[0]);
            normBezier(u2,v,ind,n3,&pontos[0]);
            normBezier(u2,v2,ind,n4,&pontos[0]);

            	//0 2 1 para ter o teapot alinhado com o eixo do x

            fprintf(f,"%f %f %f\n",p3[0],p3[2],p3[1]);
            fprintf(f,"%f %f %f\n",n3[0],n3[2],n3[1]);
            fprintf(f, "%f %f\n",u2,v);

            fprintf(f,"%f %f %f\n",p1[0],p1[2],p1[1]);
            fprintf(f,"%f %f %f\n",n1[0],n1[2],n1[1]);
            fprintf(f, "%f %f\n",u,v); 

            fprintf(f,"%f %f %f\n",p4[0],p4[2],p4[1]);
            fprintf(f,"%f %f %f\n",n4[0],n4[2],n4[1]);
            fprintf(f, "%f %f\n",u2,v2);


            fprintf(f,"%f %f %f\n",p1[0],p1[2],p1[1]);
            fprintf(f,"%f %f %f\n",n1[0],n1[2],n1[1]);
            fprintf(f, "%f %f\n",u,v);

            fprintf(f,"%f %f %f\n",p2[0],p2[2],p2[1]);
            fprintf(f,"%f %f %f\n",n2[0],n2[2],n2[1]);
            fprintf(f, "%f %f\n",u,v2);

            fprintf(f,"%f %f %f\n",p4[0],p4[2],p4[1]);
            fprintf(f,"%f %f %f\n",n4[0],n4[2],n4[1]);
            fprintf(f, "%f %f\n",u2,v2);


            u += inc;
            u2 += inc;

          }
          v += inc;
          v2 += inc;
        }
      }
  }

  if(strcmp("plane",argv[1])==0){
      float comp=strtod(argv[2],&garbage)/2;
      float larg=strtod(argv[3],&garbage)/2;
      //printf("%f---%f\n",comp,larg);
      fprintf(f,"%f %f %f\n0 1 0\n0 0\n",-comp,zero,-larg);
      fprintf(f,"%f %f %f\n0 1 0\n0 1\n",-comp,zero,larg);
      fprintf(f,"%f %f %f\n0 1 0\n1 0\n",comp,zero,-larg);
      fprintf(f,"%f %f %f\n0 1 0\n0 1\n",-comp,zero,larg);
      fprintf(f,"%f %f %f\n0 1 0\n1 1\n",comp,zero,larg);
      fprintf(f,"%f %f %f\n0 1 0\n1 0\n",comp,zero,-larg);

  }
  if(strcmp("box",argv[1])==0){

        float comp=strtod(argv[2],&garbage)/2;
        float alt=strtod(argv[3],&garbage)/2;
        float larg=strtod(argv[4],&garbage)/2;
        float divs=strtod(argv[5],&garbage);
        float iX=comp*2/divs;
        float iY=alt*2/divs;
        float iZ=larg*2/divs;
        float cx=-comp,cy=-alt,cz=-larg;
        //printf("%f %f %f,%f\n",iX,iY,iZ,alt);

        cy=-alt;
        for(int i=0;i<divs;i++){
          cz=-larg;
          for(int j=0;j<divs;j++){
            //printf("--%f %f\n",cy,cz);
            //-X
            fprintf(f,"%f %f %f\n-1 0 0\n%f %f\n",-comp,cy+iY,cz,0.5f+j/(divs*4.0f),0.333f + (divs -i-1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n-1 0 0\n%f %f\n",-comp,cy,cz,0.5f+j/(divs*4.0f),0.333f + (divs -i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n-1 0 0\n%f %f\n",-comp,cy,cz+iZ,0.5f+(j+1)/(divs*4.0f),0.333f + (divs -i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n-1 0 0\n%f %f\n",-comp,cy+iY,cz,0.5f+j/(divs*4.0f),0.333f + (divs - i-1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n-1 0 0\n%f %f\n",-comp,cy,cz+iZ,0.5f+(j+1)/(divs*4.0f),0.333f + (divs -i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n-1 0 0\n%f %f\n",-comp,cy+iY,cz+iZ,0.5f+(j+1)/(divs*4.0f),0.333f + (divs - i-1)/(divs*3.0f));

            //+X
            fprintf(f,"%f %f %f\n1 0 0\n%f %f\n",comp,cy,cz+iZ,(divs - j-1)/(divs*4.0f),0.333f + (divs - i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n1 0 0\n%f %f\n",comp,cy,cz,(divs -j)/(divs*4.0f),0.333f + (divs -i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n1 0 0\n%f %f\n",comp,cy+iY,cz,(divs -j)/(divs*4.0f),0.333f + (divs -i-1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n1 0 0\n%f %f\n",comp,cy+iY,cz+iZ,(divs - j-1)/(divs*4.0f),0.333f + (divs - i - 1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n1 0 0\n%f %f\n",comp,cy,cz+iZ,(divs - j-1)/(divs*4.0f),0.333f + (divs - i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n1 0 0\n%f %f\n",comp,cy+iY,cz,(divs -j)/(divs*4.0f),0.333+(divs -i-1)/(divs*3.0f));

            cz=cz+iZ;
          }
          cy=cy+iY;
        }
          //lados
          
          cx=-comp;
          for(int i=0;i<divs;i++){
            cy = -alt;
            for(int j=0;j<divs;j++){
              //-Z
            	fprintf(f,"%f %f %f\n0  0 -1\n%f %f\n",cx+iX,cy,-larg,0.25+(divs-i-1)/(divs*4.0f),0.333+(divs-j)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0  0 -1\n%f %f\n",cx,cy,-larg,0.25+(divs-i)/(divs*4.0f),0.333+(divs-j)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0  0 -1\n%f %f\n",cx,cy+iY,-larg,0.25+(divs-i)/(divs*4.0f),0.333+(divs -j-1)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0  0 -1\n%f %f\n",cx+iX,cy+iY,-larg,0.25+(divs-i-1)/(divs*4.0f),0.333+(divs -j-1)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0  0 -1\n%f %f\n",cx+iX,cy,-larg,0.25+(divs-i-1)/(divs*4.0f),0.333+(divs-j)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0  0 -1\n%f %f\n",cx,cy+iY,-larg,0.25+(divs-i)/(divs*4.0f),0.333+(divs -j-1)/(divs*3.0f));

              //+Z
              fprintf(f,"%f %f %f\n0 0 1\n%f %f\n",cx,cy+iY,larg,0.75f+i/(divs*4.0f),0.333+(divs - j - 1)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0 0 1\n%f %f\n",cx,cy,larg,0.75f+i/(divs*4.0f),0.333+(divs - j)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0 0 1\n%f %f\n",cx+iX,cy,larg,0.75f+(i+1)/(divs*4.0f),0.333+(divs - j)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0 0 1\n%f %f\n",cx,cy+iY,larg,0.75f+i/(divs*4.0f),0.333+(divs - j - 1)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0 0 1\n%f %f\n",cx+iX,cy,larg,0.75f+(i+1)/(divs*4.0f),0.333+(divs - j)/(divs*3.0f));
              fprintf(f,"%f %f %f\n0 0 1\n%f %f\n",cx+iX,cy+iY,larg,0.75f+(i+1)/(divs*4.0f),0.333+(divs - j - 1)/(divs*3.0f));
              
              cy=cy+iY;
            }
            cx=cx+iX;
          }
        
        cz = -larg;
        for(int i=0;i<divs;i++){
          cx=-comp;
          for(int j=0;j<divs;j++){
            //topos
              //+Y certo
            fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",cx+iX,alt,cz,0.25+(j+1)/(divs*4.0f),i/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",cx,alt,cz,0.25+j/(divs*4.0f),i/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",cx,alt,cz+iZ,0.25+j/(divs*4.0f),(i+1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",cx,alt,cz+iZ,0.25+j/(divs*4.0f),(i+1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",cx+iX,alt,cz+iZ,0.25+(j+1)/(divs*4.0f),(i+1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",cx+iX,alt,cz,0.25+(j+1)/(divs*4.0f),i/(divs*3.0f));

              //-Y certo
            fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",cx,-alt,cz+iZ,0.25+j/(divs*4.0f),0.666f + (divs - i-1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",cx,-alt,cz,0.25+j/(divs*4.0f),0.666f + (divs-i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",cx+iX,-alt,cz,0.25+(j+1)/(divs*4.0f),0.666f + (divs-i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",cx+iX,-alt,cz,0.25+(j+1)/(divs*4.0f),0.666f + (divs-i)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",cx+iX,-alt,cz+iZ,0.25+(j+1)/(divs*4.0f),0.666f + (divs - i - 1)/(divs*3.0f));
            fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",cx,-alt,cz+iZ,0.25+j/(divs*4.0f),0.666f + (divs-i-1)/(divs*3.0f));

            cx=cx+iX;
            }
          cz=cz+iZ;
        }
  }

  if(strcmp("sphere",argv[1])==0){
      float radius=strtod(argv[2],&garbage);
      int slices=atoi(argv[3]);
      int stacks=atoi(argv[4]);

      	float beta = M_PI/2;
      	float beta2;
      	float b = M_PI/stacks;

      	float alfa = 0.0f;
      	float alfa2;
      	float a = M_PI*2/slices;

      	float n[3],n2[3],n3[3],n4[3],p[3],p2[3],p3[3],p4[3],p5[3],p6[3],p7[3],p8[3];

      	int x,z;

      	beta2 = beta - b;
      	for(int i = 0; i < stacks; i++){
      		alfa = 0;
      		alfa2 = alfa + a;
      		for(int j = 0;j < slices; j++ ){
            n[0]=0;n[1]=1;n[2]=0;
            n2[0]=0;n2[1]=0;n2[2]=0;
            n3[0]=0;n3[1]=0;n3[2]=0;
            n4[0]=0;n4[1]=0;n4[2]=0;

      /*p[0]=radius * cos(beta2)*cos(alfa);
	  p[1]=radius * sin(beta2);
	  p[2]=radius * cos(beta2)*sin(alfa);

	  p2[0]=radius * cos(beta) * cos(alfa2);
	  p2[1]=radius * sin(beta);
	  p2[2]=radius * cos(beta) * sin(alfa2);

	  p3[0]=radius * cos(beta) * cos(alfa);
	  p3[1]=radius * sin(beta);
	  p3[2]=radius * cos(beta) * sin(alfa);

	  p4[0]=radius * cos(beta2) * cos(alfa2);
	  p4[1]=radius * sin(beta2);
	  p4[2]=radius * cos(beta2) * sin(alfa2);*/
      			//a
      			p[0] = radius * cos(beta) * sin(alfa);
      			p[1] = radius * sin(beta);
      			p[2] = radius * cos(beta) * cos(alfa);

      			//b
      			p2[0] = radius * cos(beta) * sin(alfa2);
      			p2[1] = radius * sin(beta);
      			p2[2] = radius * cos(beta) * cos(alfa2);

      			//c
      			p3[0] = radius * cos(beta2) * sin(alfa);
      			p3[1] = radius * sin(beta2);
      			p3[2] = radius * cos(beta2) * cos(alfa);

      			//d
      			p4[0] = radius * cos(beta2) * sin(alfa2);
      			p4[1] = radius * sin(beta2);
      			p4[2] = radius * cos(beta2) * cos(alfa2);
      			alfa += a;
      			alfa2 = alfa + a;

          if (i != 0){
            norm(p2,p,p3,n2);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p2[0],p2[1],p2[2],p2[0],p2[1],p2[2],(j+1)/(slices*1.0f),i/(stacks*1.0f));
            norm(p,p3,p2,n);
          	fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p[0],p[1],p[2],p[0],p[1],p[2],j/(slices*1.0f),i/(stacks*1.0f));
            norm(p3,p2,p,n3);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p3[0],p3[1],p3[2],p3[0],p3[1],p3[2],j/(slices*1.0f),(i+1)/(stacks*1.0f));
           
          }
          if (i != stacks-1){
            norm(p2,p3,p4,n2);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p2[0],p2[1],p2[2],p2[0],p2[1],p2[2],(j+1)/(slices*1.0f),i/(stacks*1.0f));
            norm(p3,p4,p2,n3);
          	fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p3[0],p3[1],p3[2],p3[0],p3[1],p3[2],j/(slices*1.0f),(i+1)/(stacks*1.0f));
            norm(p4,p2,p3,n4);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p4[0],p4[1],p4[2],p4[0],p4[1],p4[2],(j+1)/(slices*1.0f),(i+1)/(stacks*1.0f));            
          }
      		}
      		beta-=b;
      		beta2=beta-b;
      	}
  }

  if(strcmp("sky",argv[1])==0){
      float radius=strtod(argv[2],&garbage);
      int slices=atoi(argv[3]);
      int stacks=atoi(argv[4]);

        float beta = M_PI/2;
        float beta2;
        float b = M_PI/stacks;

        float alfa = 0.0f;
        float alfa2;
        float a = M_PI*2/slices;

        float n[3],n2[3],n3[3],n4[3],p[3],p2[3],p3[3],p4[3];

        int x,z;

        beta2 = beta - b;
        for(int i = 0; i < stacks; i++){
          alfa = 0;
          alfa2 = alfa + a;
          for(int j = 0;j < slices; j++ ){
            n[0]=0;n[1]=1;n[2]=0;
            n2[0]=0;n2[1]=0;n2[2]=0;
            n3[0]=0;n3[1]=0;n3[2]=0;
            n4[0]=0;n4[1]=0;n4[2]=0;
            //a
            p[0] = radius * cos(beta) * sin(alfa);
            p[1] = radius * sin(beta);
            p[2] = radius * cos(beta) * cos(alfa);

            //b
            p2[0] = radius * cos(beta) * sin(alfa2);
            p2[1] = radius * sin(beta);
            p2[2] = radius * cos(beta) * cos(alfa2);

            //c
            p3[0] = radius * cos(beta2) * sin(alfa);
            p3[1] = radius * sin(beta2);
            p3[2] = radius * cos(beta2) * cos(alfa);

            //d
            p4[0] = radius * cos(beta2) * sin(alfa2);
            p4[1] = radius * sin(beta2);
            p4[2] = radius * cos(beta2) * cos(alfa2);
            alfa += a;
            alfa2 = alfa + a;

          if (i != 0){
            norm(p3,p2,p,n3);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p3[0],p3[1],p3[2],n3[0],n3[1],n3[2],j/(slices*1.0f),(i+1)/(stacks*1.0f));
            norm(p,p3,p2,n);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p[0],p[1],p[2],n[0],n[1],n[2],j/(slices*1.0f),i/(stacks*1.0f));
            norm(p2,p,p3,n2);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p2[0],p2[1],p2[2],n2[0],n2[1],n2[2],(j+1)/(slices*1.0f),i/(stacks*1.0f));
          }
          if (i != stacks-1){
            norm(p4,p2,p3,n4);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p4[0],p4[1],p4[2],n4[0],n4[1],n4[2],(j+1)/(slices*1.0f),(i+1)/(stacks*1.0f)); 
            norm(p3,p4,p2,n3);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p3[0],p3[1],p3[2],n3[0],n3[1],n3[2],j/(slices*1.0f),(i+1)/(stacks*1.0f));
            norm(p2,p3,p4,n2);
            fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p2[0],p2[1],p2[2],n2[0],n2[1],n2[2],(j+1)/(slices*1.0f),i/(stacks*1.0f));  
          }
          }
          beta-=b;
          beta2=beta-b;
        }
  }


  if(strcmp("anel",argv[1])==0){
    float radius=strtod(argv[2],&garbage);
    float radiusE=strtod(argv[3],&garbage);
    int slices=atoi(argv[4]);
    int stacks=atoi(argv[5]);

    float alfa = 0.0f;
    float a = M_PI*2/slices;
    float alfa2 = alfa + a;
    float raio = (radiusE-radius)/stacks;
    radiusE = radius + raio;


    float px,px2,px3,px4,py,py2,py3,py4,pz,pz2,pz3,pz4;

    for(int i = 0;i < stacks;i++){
      for(int j = 0;j < slices; j++ ){

      //a
        px = radius * sin(alfa);
        py = 0.0f;
        pz = radius * cos(alfa);

      //b
        px2 = radius * sin(alfa2);
        py2 = 0.0f;
        pz2 = radius * cos(alfa2);

      //c
        px3 = radiusE * sin(alfa);
        py3 = 0.0f;
        pz3 = radiusE * cos(alfa);
      
      //d
        px4 = radiusE * sin(alfa2);
        py4 = 0.0f;
        pz4 = radiusE * cos(alfa2);

        alfa += a;
        alfa2 = alfa + a;
    
        //baixo
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px,py,pz,i/(stacks*1.0f),j/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px2,py2,pz2,i/(stacks*1.0f),(j+1)/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px3,py3,pz3,i/(stacks*1.0f),j/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px4,py4,pz4,(i+1)/(stacks*1.0f),(j+1)/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px3,py3,pz3,i/(stacks*1.0f),j/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px2,py2,pz2,i/(stacks*1.0f),(j+1)/(slices*1.0f));


        //cima
        fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",px3,py3,pz3,i/(stacks*1.0f),j/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",px2,py2,pz2,i/(stacks*1.0f),(j+1)/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",px,py,pz,i/(stacks*1.0f),j/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",px2,py2,pz2,i/(stacks*1.0f),(j+1)/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",px3,py3,pz3,i/(stacks*1.0f),j/(slices*1.0f));
        fprintf(f,"%f %f %f\n0 1 0\n%f %f\n",px4,py4,pz4,(i+1)/(stacks*1.0f),(j+1)/(slices*1.0f));

          }
          radius=radiusE;
          radiusE+=raio;
        }
  }
  if(strcmp("cone",argv[1])==0){
      float radius=strtod(argv[2],&garbage);
      float height=strtod(argv[3],&garbage);
      int slices=atoi(argv[4]);
      int stacks=atoi(argv[5]);
      float p1[3],p2[3],p3[3],p4[3],n[3];


      	float x = 2*M_PI / slices;
      	float ang,px,pz,px2,pz2,px3,pz3,px4,pz4;
      	ang = 0.0f;

      	for(int i = 0; i < slices; i++){
      		px = radius * sin(ang);
      		pz = radius * cos(ang);
      		float ang1 = ang;
      		ang += x;
      		px2 = radius * sin(ang);
      		pz2 = radius * cos(ang);

      	//base
      	fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px,zero,pz,0.25+sin(ang1)/4,0.25+cos(ang1)/4);
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",zero,zero,zero,0.25f,0.5f);
        fprintf(f,"%f %f %f\n0 -1 0\n%f %f\n",px2,zero,pz2,0.25+sin(ang)/4,0.25+cos(ang)/4);

      	/*fprintf(f,"%f %f %f\n",px,zero,pz);
        fprintf(f,"%f %f %f\n",zero,zero,zero);
        fprintf(f,"%f %f %f\n",px2,zero,pz2);*/

      	}

      	float k = height/stacks;
      	float cima = k;
      	float baixo = 0.0f;
      	float raio = radius/stacks;
      	float aux = radius;
      	float ang2 = x;

      	//lado
      	for(int j = 0;j < stacks; j++){
      		ang2 = x;
      		ang = 0.0f;
      		for(int i = 0;i < slices; i++){
      			radius = aux;
      			//baixo
      			px = radius * sin(ang);
      			pz = radius * cos(ang);
      			px2 = radius * sin(ang2);
      			pz2 = radius * cos(ang2);
      			radius-=raio;

      			//cima
      			px3 = radius * sin(ang);
      			pz3 = radius * cos(ang);
      			px4 = radius * sin(ang2);
      			pz4 = radius * cos(ang2);

      			ang2 += x;
      			ang += x;

      			p1[0] = px; p1[1] = baixo; p1[2] = pz;
      			p2[0] = px2; p2[1] = baixo; p2[2] = pz2;
      			p3[0] = px3; p3[1] = cima; p3[2] = pz3;
      			p4[0] = px4; p4[1] = cima; p4[2] = pz4;


            	/*fprintf(f,"%f %f %f\n",px3,h,pz3);
      			
            	fprintf(f,"%f %f %f\n",px,h1,pz);
            	
            	fprintf(f,"%f %f %f\n",px2,h1,pz2);
            		
            		fprintf(f,"%f %f %f\n",px3,h,pz3);
            		
            		fprintf(f,"%f %f %f\n",px2,h1,pz2);
            		
            		fprintf(f,"%f %f %f\n",px4,h,pz4);*/

      			  norm(p3,p1,p2,n);
            	fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p3[0],p3[1],p3[2],n[0],n[1],n[2],0.6f+(i+1)/(slices*3.0f),1.0f); //+.6 e *3 porque a text tem erro. (deveria ser .5 *2)
      			  norm(p1,p2,p3,n);
            	fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p1[0],p1[1],p1[2],n[0],n[1],n[2],0.6f+i/(slices*3.0f),0.0f);
            	norm(p2,p3,p1,n);
            	fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p2[0],p2[1],p2[2],n[0],n[1],n[2],0.6f+(i+1)/(slices*3.0f),0.0f);

            	if(j != stacks-1){
            		norm(p4,p3,p2,n);
            		fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p4[0],p4[1],p4[2],n[0],n[1],n[2],0.6f+(i+1)/(slices*3.0f),1.0f);
            		norm(p3,p2,p4,n);
            		fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p3[0],p3[1],p3[2],n[0],n[1],n[2],0.6f+i/(slices*3.0f),1.0f);
            		norm(p2,p4,p3,n);
            		fprintf(f,"%f %f %f\n%f %f %f\n%f %f\n",p2[0],p2[1],p2[2],n[0],n[1],n[2],0.6f+(i+1)/(slices*3.0f),0.0f);
            	}
      		}
      		baixo = cima;
      		cima += k;
      		aux -= raio;
      	}
  }
  fclose(f);

  return 0;
}
