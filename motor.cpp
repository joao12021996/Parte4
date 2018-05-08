#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "catmull.hpp"
#include "pugixml.hpp"
#ifdef __APPLE__
//#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <math.h>
#include <map>
#include <vector>
#include <IL/il.h>

#define _PI_ 3.14159

float r=0;
float alfa = 0,alfa2 = 0;
float beta = 0,beta2 = 0;
float camX, camY, camZ;
float lx, ly, lz,thirdly;
float raio = 10;

float fps = 60.0f;
float k = 0.0f;
int contador = 0;
int timed = 0;
int paragem = 1;

int catmull = 0;

int w,h;
float black[4] = {0.0f,0.0f,0.0f,0.0f};
float white[4] = {1.0f, 1.0f , 1.0f,1.0f};
char text[64];

GLuint buffers[3];
using namespace pugi;
using namespace std;
typedef map<string,vector<float> > Map;
typedef map<int,vector<float> > CatMap;
typedef map<string,unsigned int> TexMap;
typedef map<float,string> TextMap;
Map files;
Map normal;
Map texture;


TexMap texFile;
TextMap texto;

CatMap pontos;
CatMap yy;

void spherical2Cartesian() {

	camX = raio * cos(beta) * sin(alfa);
	camY = 50 + raio * sin(beta);
	camZ = 242 + raio * cos(beta) * cos(alfa);

	lx = camX - raio * cos(beta) * sin(alfa);
	ly = camY - raio * sin(beta);
	lz = camZ - raio * cos(beta) * cos(alfa);
	thirdly = ly - 3;
}

class Camera{
public:
	vector<float> files;
	vector<float> normais;
	vector<float> texturas;
	int tex;
	Camera(){
		tex = 0;
	}
	Camera(vector<float> files, vector<float> normais, vector<float> texturas, int tex){
		this->files=files;
		this->normais=normais;
		this->texturas=texturas;
		this->tex = tex;
	}
};

Camera cam;

class Transf{ //transformacao
public:
	float ang,t1,t2,t3,inc;
	Transf(float ang,float r1,float r2,float r3,float inc){
		this->ang=ang;
		this->t1=r1;
		this->t2=r2;
		this->t3=r3;
		this->inc=inc;

	}
};

class Catmull{ //transformacao
public:
	int i;		//numero no map
	float t;	//ponto atual
	float inc;	//inc no t [0..1] com 1/fps*time

	Catmull(int i,float t,float inc){
		this->i=i;
		this->t=t;
		this->inc=inc;
	}
};

class Material{
public:
	float r,g,b;
	GLenum pname;
	Material(float x,float y,float z,GLenum pname){
		this->r=x;
		this->g=y;
		this->b=z;
		this->pname=pname;
	}
};
class Light{
public:
	float x,y,z,w,ambx,amby,ambz,diffx,diffy,diffz,spX,spY,spZ,cutoff,exponent;
	int i;
	Light(int i,float *pos,float *spotDir,float *amb,float *diff,float cutoff,float exponent){

		this->x=pos[0];
		this->y=pos[1];
		this->z=pos[2];
		this->w=pos[3];
		this->ambx=amb[0];
		this->amby=amb[1];
		this->ambz=amb[2];
		this->diffx=diff[0];
		this->diffy=diff[1];
		this->diffz=diff[2];
		this->spX=spotDir[0];
		this->spY=spotDir[1];
		this->spZ=spotDir[2];
		this->i = i;
		this->cutoff = cutoff;
		this->exponent=exponent;
	}
};

class Primi{
public:
	string nome;
	string tex;
	
	Primi(const char* nome,const char* tex){
		string aux(nome);
		this->nome.assign(aux);
		string aux2(tex);
		this->tex.assign(aux2);
	}
};

class Acao{
public:
	Transf *t;
	Primi *p;
	Catmull *c;
	Material *m;
	Light *l;
	int tipo; //1-Push -1-Pop 0-vertices 2-translaçao 3-rotaçao 4-escala 5-rotaçao c/tempo 6-transCatmull 7-light 8-Material
	Acao *prox;

	Acao(int t){
		this->tipo=t;
		this->prox = NULL;
		this->t = NULL;
		this->c = NULL;
		this->p = NULL;
		this->l = NULL;
	}
	void catmull(int i,float t,float t2){
		c=new Catmull(i,t,t2);
	}
	void translate(float t1,float t2,float t3){
		t=new Transf(0,t1,t2,t3,0);
	}
	void rotate(float ang,float r1,float r2,float r3,float inc){
		t=new Transf(ang,r1,r2,r3,inc);
	}
	void scale(float s1,float s2,float s3){
		t=new Transf(0,s1,s2,s3,0);
	}
	void light(int i,float *pos,float *spotDir,float *amb,float *diff,float cutoff,float exponent){
		l=new Light(i,pos,spotDir,amb,diff,cutoff,exponent);
	}
	void material(float r,float g,float b,GLenum nome){
		m=new Material(r,g,b,nome);
	}
	void Vfile(const char* name,const char* tex){
		p=new Primi(name,tex);
	}

	Acao* Prox(){ // obtém o próximo No
		return prox;
	}

	void sProx(Acao* p){ // seta o próximo No
		prox = p;
	}
};

int loadTexture(std::string s) {

	unsigned int tw,th,t,textura;
	unsigned char *texData;

	ilGenImages(1,&t);
	glGenTextures(1,&textura);
	ilBindImage(t);
	ilLoadImage((ILstring)s.c_str());
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	texData = ilGetData();
	
	glBindTexture(GL_TEXTURE_2D,textura);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,	GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,	GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, 	GL_LINEAR_MIPMAP_LINEAR);
		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textura;

}


// classe Lista
class Lista
{
private:
	Acao* cabeca;
	Acao* ult;
public:
	Lista()	{
		cabeca = NULL;
		ult=NULL;
	}


	void mostrar(){ // mostra todos os elementos da lista
		Acao* c = cabeca;

		if(vazia())
			cout << "Sem elems\n";
		else
		{
			while(c)
			{
				c = c->Prox();
			}
			cout << endl;
		}
	}


	bool vazia(){ // verifica se a lista está vazia
		return (cabeca == NULL);
	}

	// insere no fim
	Acao *insert(int t){
		Acao* novo = new Acao(t);
		if(vazia())
		{
			cabeca = novo;
			ult=novo;
		}
		else
		{
			ult->sProx(novo);
			ult = novo;
		}
		return novo;
	}

	void drawAll(int light){
		float y[3];
		Acao *c=cabeca;
		int i = 0;
		float k = 1/256.0f;
		//glMaterialfv(GL_FRONT,GL_EMISSION,black);
		glMaterialfv(GL_FRONT,GL_AMBIENT,white);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
		glMaterialfv(GL_FRONT,GL_SPECULAR,white);
		while(c){
			if (c->tipo==1){
				glPushMatrix();
			}
			else if (c->tipo==-1){
				glPopMatrix();
			}
			else if (c->tipo==2){
				glTranslatef(c->t->t1,c->t->t2,c->t->t3);
			}
			else if (c->tipo==3){
				glRotatef(c->t->ang,c->t->t1,c->t->t2,c->t->t3);
				GLUquadricObj *quadric;
				quadric = gluNewQuadric();
				gluQuadricDrawStyle(quadric,GLU_FILL);
				gluSphere(quadric,1,60,30);
				if(timed&&paragem){
					c->t->ang+=c->t->inc;
					if(c->t->ang>360.0f){
						c->t->ang-=360.0f;
					}
				}
			}
			else if (c->tipo==4){
				glScalef(c->t->t1,c->t->t2,c->t->t3);
			}
			else if (c->tipo==6){
				y[0] = yy[c->c->i][0]; y[1] = yy[c->c->i][1]; y[2] = yy[c->c->i][2];
				renderCatmull(c->c->t,y,pontos[c->c->i].size()/3, &pontos[c->c->i].front());
				if(timed&&paragem){
					c->c->t+=c->c->inc;
					if(c->c->t > 1){
						c->c->t--;
					}
				}
				yy[c->c->i].pop_back();
				yy[c->c->i].pop_back();
				yy[c->c->i].pop_back();

				yy[c->c->i].push_back(y[0]); 
				yy[c->c->i].push_back(y[1]); 
				yy[c->c->i].push_back(y[2]);
			}
			else if(c->tipo==7&&light){
				i = c->l->i;
				float pos[4] = {c->l->x,c->l->y,c->l->z,c->l->w};
				float amb[4] = {c->l->ambx,c->l->amby,c->l->ambz,1};
				float diff[4] = {c->l->diffx,c->l->diffy,c->l->diffz,1};
				glLightfv(GL_LIGHT0+i, GL_POSITION, pos);
				glLightfv(GL_LIGHT0+i, GL_AMBIENT, amb);
				glLightfv(GL_LIGHT0+i, GL_DIFFUSE, diff);
				if(c->l->cutoff != 0.0f){
					float spotDir[3] = {c->l->spX,c->l->spY,c->l->spZ};
					glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, spotDir);
					glLightf(GL_LIGHT0+i, GL_SPOT_CUTOFF, c->l->cutoff);
					glLightf(GL_LIGHT0+i, GL_SPOT_EXPONENT, c->l->exponent);
				}
				glEnable(GL_LIGHT0+i);
			}
			else if(c->tipo==8&&light){
				float cor[4] = {c->m->r,c->m->g,c->m->b,1};
				glMaterialfv(GL_FRONT,c->m->pname,cor);
			}
			else if(c->tipo==0){
				if(!light){
					glColor3f(k,k,k);
					texto[k] = c->p->tex;
					k+=5/256.0f;

				}
				int size = normal[c->p->nome].size()/3;

				unsigned int tex = texFile[c->p->tex];

				glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
				glBufferData(GL_ARRAY_BUFFER,size * sizeof(float) * 3,&files[c->p->nome].front(), GL_STATIC_DRAW);
				glVertexPointer(3,GL_FLOAT,0,0);

				glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
				glBufferData(GL_ARRAY_BUFFER,size * sizeof(float) * 3,&normal[c->p->nome].front(), GL_STATIC_DRAW);
				glNormalPointer(GL_FLOAT,0,0);

				glBindBuffer(GL_ARRAY_BUFFER,buffers[2]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 2, &texture[c->p->nome].front(),  GL_STATIC_DRAW);
				glTexCoordPointer(2,GL_FLOAT,0,0);

				glBindTexture(GL_TEXTURE_2D, tex);
				glDrawArrays(GL_TRIANGLES, 0, size);
				glBindTexture(GL_TEXTURE_2D, 0);


				float* teste = &normal[c->p->nome].front();
				for(int o = 0; o < size; o = o + 3){
					glBegin(GL_LINE);
					glVertex3f(teste[o],teste[o+1],teste[o+2]);
					glVertex3f(0,0,0);
					glEnd();
				}
				
				/*glMaterialfv(GL_FRONT,GL_EMISSION,black);
				glMaterialfv(GL_FRONT,GL_AMBIENT,black);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,black);
				glMaterialfv(GL_FRONT,GL_SPECULAR,black);*/

			}
			c=c->Prox();
		}
	}
};


//lista dos vertices
Lista l;

void recur(Lista *l,xml_node group){
	l->insert(1);
	const char *file,*tex;
	ifstream fle;
	Acao *g;	
	xml_node nodo,point,model;
	float r,x,y,z,inc,w,cutoff,exponent,spX,spY,spZ,ambx,amby,ambz,diffx,diffy,diffz;
	GLenum nome;
	int texID,i=0;


	if(group){
		nodo = group.first_child();
	}
		while(nodo){
			if (strcmp(nodo.name(),"models")==0){
				model = nodo.first_child();
				while(model){
					if(model.attribute("file")){
						file = model.attribute("file").value();
						if(files.count(file)<=0){
							fle.open(file);
							float a,b,c,d,e,f,g,h;
							while(fle >> a >> b >> c >> d >> e >> f >> g >> h){
								files[file].push_back(a);
								files[file].push_back(b);
								files[file].push_back(c);
								normal[file].push_back(d);
								normal[file].push_back(e);
								normal[file].push_back(f);
								texture[file].push_back(g);
								texture[file].push_back(h);
							}
						}
						if(model.attribute("diffR")){
							g=l->insert(8);
							x = model.attribute("diffR").as_float();
				 			y = model.attribute("diffG").as_float();
				 			z = model.attribute("diffB").as_float();
				 			nome = GL_DIFFUSE;
				 			g->material(x,y,z,nome);
						}
						if(model.attribute("ambR")){
							g=l->insert(8);
							x = model.attribute("ambR").as_float();
				 			y = model.attribute("ambG").as_float();
				 			z = model.attribute("ambB").as_float();
				 			nome = GL_AMBIENT;
				 			g->material(x,y,z,nome);
						}
						if(model.attribute("emiR")){
							g=l->insert(8);
							x = model.attribute("emiR").as_float();
				 			y = model.attribute("emiG").as_float();
				 			z = model.attribute("emiB").as_float();
				 			nome = GL_EMISSION;
				 			g->material(x,y,z,nome);
						}
						if(model.attribute("spR")){
							g=l->insert(8);
							x = model.attribute("spR").as_float();
				 			y = model.attribute("spG").as_float();
				 			z = model.attribute("spB").as_float();
				 			nome = GL_SPECULAR;
				 			g->material(x,y,z,nome);
						}
						tex = "";
						if(model.attribute("texture")){
							tex = model.attribute("texture").value();
							if(texFile.count(tex)<=0){
								texID = loadTexture(tex);
								texFile[tex] = texID;
							}
						}
						else if (texFile.count("")<=0){
							texFile[tex] = 0;
						}

						g=l->insert(0);
						g-> Vfile(file,tex);
					}
					else if(model.attribute("camera")){
						file = model.attribute("camera").value();
						fle.open(file);
						float a,b,c,d,e,f,g,h;
						vector<float> camera, normais,texturas;
						while(fle >> a >> b >> c >> d >> e >> f >> g >> h){
							camera.push_back(a);
							camera.push_back(b);
							camera.push_back(c);
							normais.push_back(d);
							normais.push_back(e);
							normais.push_back(f);
							texturas.push_back(g);
							texturas.push_back(h);
						}
						if(model.attribute("texture")){
							tex = model.attribute("texture").value();
							texID = loadTexture(tex);
						}
						cam = Camera(camera,normais,texturas,texID);
					}
					model = model.next_sibling();
				}
			}
			if (strcmp(nodo.name(),"lights")==0){
				model = nodo.first_child();
				float spotDir[3];
				while(model){
					x = model.attribute("posX").as_float();
				 	y = model.attribute("posY").as_float();
				 	z = model.attribute("posZ").as_float();
				 	ambx = model.attribute("ambX").as_float();
				 	amby = model.attribute("ambY").as_float();
				 	ambz = model.attribute("ambZ").as_float();
				 	diffx = model.attribute("diffX").as_float();
				 	diffy = model.attribute("diffY").as_float();
				 	diffz = model.attribute("diffZ").as_float();
				 	float amb[4] = {ambx,amby,ambz,1.0};
				 	float diff[4] = {diffx,diffy,diffz,1.0};
					if(strcmp(model.attribute("type").value(),"POINT")==0){
						float pos[4] = {x,y,z,1.0};
						g=l->insert(7);
						g->light(i,pos,spotDir,amb,diff,0.0f,0.0f);
					}
					else if(strcmp(model.attribute("type").value(),"DIRECTIONAL")==0){
						float pos[4] = {x,y,z,0.0f};
						g=l->insert(7);
						g->light(i,pos,spotDir,amb,diff,0.0f,0.0f);
					}
					else if(strcmp(model.attribute("type").value(),"SPOTLIGHT")==0){
						float pos[4] = {x,y,z,1};
						spX = model.attribute("spX").as_float();
				 		spY = model.attribute("spY").as_float();
				 		spZ = model.attribute("spZ").as_float();
				 		cutoff = model.attribute("cutoff").as_float();
				 		exponent = model.attribute("exponent").as_float();
				 		float spotDir[3] = {spX,spY,spZ};
						g=l->insert(7);
						g->light(i,pos,spotDir,amb,diff,cutoff,exponent);
					}
					model = model.next_sibling();
					i++;
				}
			}
			else if (strcmp(nodo.name(),"translate")==0){
				if(nodo.attribute("time")){
					point = nodo.first_child();
					i = nodo.attribute("time").as_float();
					while(point){
						x = point.attribute("X").as_float();
				 		y = point.attribute("Y").as_float();
				 		z = point.attribute("Z").as_float();
				 		pontos[catmull].push_back(x);
						pontos[catmull].push_back(y);
						pontos[catmull].push_back(z);
				 		point=point.next_sibling();
				 	}
				 	g=l->insert(6);
				 	g->catmull(catmull,0,1/(i*fps));
				 	yy[catmull].push_back(0.0f);
				 	yy[catmull].push_back(1.0f);
				 	yy[catmull].push_back(0.0f);
				 	catmull++;

				}
				else{
				 	x = nodo.attribute("X").as_float();
				 	y = nodo.attribute("Y").as_float();
				 	z = nodo.attribute("Z").as_float();
					g=l->insert(2);
					g->translate(x,y,z);
				}
			}
			else if (strcmp(nodo.name(),"rotate")==0){
				if(nodo.attribute("time")){
					r = nodo.attribute("time").as_float();
					inc = 360.0f/(r*fps);
					r = 0.0f;
				}
				else{
					r = nodo.attribute("angle").as_float();
					inc = 0.0f;
				}
				x = nodo.attribute("axisX").as_float();
				y = nodo.attribute("axisY").as_float();
				z = nodo.attribute("axisZ").as_float();
				g=l->insert(3);
				g->rotate(r,x,y,z,inc);
			}
			else if (strcmp(nodo.name(),"scale")==0){
				x = nodo.attribute("X").as_float();
				y = nodo.attribute("Y").as_float();
				z = nodo.attribute("Z").as_float();
				g=l->insert(4);
				g->scale(x,y,z);
			}
			else if (strcmp(nodo.name(),"group")==0){
				recur(l,nodo);
			}
			nodo = nodo.next_sibling();
		}
	l->insert(-1); 
}


void loadScene(Lista *l,const char *XMLfile){

		xml_document doc;
		doc.load_file(XMLfile);
		xml_node scene=doc.first_child();
		xml_node group = scene.first_child();
		glGenBuffers(3, buffers);
		while(group){
			recur(l,group);
			group=group.next_sibling();
		}
}


void changeSize(int ww, int hh) {

	w = ww;
	h = hh;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void renderText() {


	glMaterialfv(GL_FRONT, GL_EMISSION, white);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// set projection so that coordinates match window pixels
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
	glLoadIdentity();
	glRasterPos2d(10, 20);

	for (char *c = text; *c != '\0'; c++) {
	glutBitmapCharacter	(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix(); 
	glEnable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT, GL_EMISSION, black);
}


void fundo(int light){

	glPushMatrix();
	glTranslatef(camX,camY,camZ);

	if(!light){
		glColor3f(0,0,0);
		glMaterialfv(GL_FRONT,GL_EMISSION,black);
		glMaterialfv(GL_FRONT,GL_AMBIENT,black);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,black);
		glMaterialfv(GL_FRONT,GL_SPECULAR,black);
	}
	else{
		glMaterialfv(GL_FRONT,GL_EMISSION,white);
		glMaterialfv(GL_FRONT,GL_AMBIENT,white);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
		glMaterialfv(GL_FRONT,GL_SPECULAR,white);
	}

	unsigned int tex = cam.tex;

	int size = cam.files.size()/3;

	glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
	glBufferData(GL_ARRAY_BUFFER,size * sizeof(float) * 3,&cam.files.front(), GL_STATIC_DRAW);
	glVertexPointer(3,GL_FLOAT,0,0);

	glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
	glBufferData(GL_ARRAY_BUFFER,size * sizeof(float) * 3,&cam.normais.front(), GL_STATIC_DRAW);
	glNormalPointer(GL_FLOAT,0,0);

	glBindBuffer(GL_ARRAY_BUFFER,buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 2, &cam.texturas.front(),  GL_STATIC_DRAW);
	glTexCoordPointer(2,GL_FLOAT,0,0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glDrawArrays(GL_TRIANGLES, 0, size);
	glBindTexture(GL_TEXTURE_2D, 0);

	glMaterialfv(GL_FRONT,GL_EMISSION,black);
	glMaterialfv(GL_FRONT,GL_AMBIENT,black);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,black);
	glMaterialfv(GL_FRONT,GL_SPECULAR,black);

	glPopMatrix();				
}


void nave(int light){
	if(!light){
		glColor3f(0,0,0);
		glMaterialfv(GL_FRONT,GL_EMISSION,black);
		glMaterialfv(GL_FRONT,GL_AMBIENT,black);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,black);
		glMaterialfv(GL_FRONT,GL_SPECULAR,black);
	}
	else{
		glMaterialfv(GL_FRONT,GL_EMISSION,black);
		glMaterialfv(GL_FRONT,GL_AMBIENT,white);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
		glMaterialfv(GL_FRONT,GL_SPECULAR,white);
	}
	glPushMatrix();
	glTranslatef(lx,thirdly,lz);
	glRotatef(alfa2+90,0,1,0);
	glRotatef(beta2,0,0,1);
	glutSolidTeapot(0.3);
	glPopMatrix();

	glMaterialfv(GL_FRONT,GL_EMISSION,black);
	glMaterialfv(GL_FRONT,GL_AMBIENT,black);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,black);
	glMaterialfv(GL_FRONT,GL_SPECULAR,black);
}


void renderScene(void) {
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();

	gluLookAt(camX,camY,camZ,
			   lx , ly , lz ,
			  0.0f,1.0f,0.0f);

	// put drawing instructions here
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	fundo(1);
	l.drawAll(1);
	nave(1);
	renderText();

	glutSwapBuffers();
}


unsigned char  picking(int x, int y) {

	unsigned char res[4];

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	/*int aux = paragem;
	paragem = 0;*/

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(camX, camY, camZ, 
		      lx,ly,lz,
			  0.0f,1.0f,0.0f);


	glDepthFunc(GL_LEQUAL);
	fundo(0);
	l.drawAll(0);
	nave(0);
	glDepthFunc(GL_LESS);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glReadPixels(x,viewport[3]- y,1,1,
				GL_RGBA,GL_UNSIGNED_BYTE, res);
	//paragem = aux;

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	return res[0];
}


void processMouseButtons(int button, int state, int xx, int yy) 
{
	//printf("%d %d\n", xx, yy);
	if (state == GLUT_DOWN)  {
		if (button == GLUT_LEFT_BUTTON){
			int picked = picking(xx,yy);
			float k = picked/256.0f;
			if (picked){
				int len = texto[k].length();
				char *nome = new char[texto[k].length() + 1];
				strcpy(nome, texto[k].c_str());
				nome[len -4] = '\0';
				sprintf(text,"Astro selected: %s\n", nome);
				//printf("Picked Snowman number %d\n", picked);
			}
			else{
				sprintf(text,"Nothing selected!\n");
				//printf("Nothing selected\n");
			}
			glutPostRedisplay();
		}
	}
}

void processKeys(unsigned char c, int xx, int yy) {

	switch (c) {

	case 'p':
		paragem++;
		if(paragem>1){
			paragem = 0;
		}
		break;
	}
	glutPostRedisplay();

}

void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		alfa -= 0.05;		 //radianos
		alfa2 -= 2.86478898f; //graus
		lx = camX - raio * cos(beta) * sin(alfa);
		ly = camY - raio * sin(beta);
		lz = camZ - raio * cos(beta) * cos(alfa);
		break;
	case GLUT_KEY_LEFT:
		alfa += 0.05;
		alfa2 += 2.86478898f;
		lx = camX - raio * cos(beta) * sin(alfa);
		ly = camY - raio * sin(beta);
		lz = camZ - raio * cos(beta) * cos(alfa);
		break;
	case GLUT_KEY_DOWN:
		beta += 0.025f;
		beta2 -= 2.86478898f/2;
		if (beta > _PI_/2){
			beta = _PI_/2;
			beta2 = -90;
		}
		lx = camX - raio * cos(beta) * sin(alfa);
		ly = camY - raio * sin(beta);
		thirdly = ly - 3;
		lz = camZ - raio * cos(beta) * cos(alfa);
		break;
	case GLUT_KEY_UP:
		beta -= 0.025f;
		beta2 += 2.86478898f/2;
		if (beta < -1*_PI_/2){
			beta = -1*_PI_/2;
			beta2 = 90;
		}
		lx = camX - raio * cos(beta) * sin(alfa);
		ly = camY - raio * sin(beta);
		thirdly = ly - 3 ;
		lz = camZ - raio * cos(beta) * cos(alfa);
		break;
	case GLUT_KEY_PAGE_DOWN:
		lx += raio * cos(beta) * sin(alfa);
		ly += raio * sin(beta);
		lz += raio * cos(beta) * cos(alfa);
		camX += raio * cos(beta) * sin(alfa);
		camY += raio * sin(beta);
		camZ += raio * cos(beta) * cos(alfa);
		thirdly = ly - 3;
		break;
	case GLUT_KEY_PAGE_UP:
		lx -= raio * cos(beta) * sin(alfa);
		ly -= raio * sin(beta);
		lz -= raio * cos(beta) * cos(alfa);
		camX -= raio * cos(beta) * sin(alfa);
		camY -= raio * sin(beta);
		camZ -= raio * cos(beta) * cos(alfa);
		thirdly = ly - 3;
		break;
	case GLUT_KEY_END:
		exit(1);
		break;
	}

	glutPostRedisplay();
}

void timing(){
	timed = 0;
	float times=glutGet(GLUT_ELAPSED_TIME);
	if(times>=k){
		k += 1000.0f/fps;
		contador++;
		timed = 1;
		//printf("%d %f\n",contador,times );
		glutPostRedisplay();
	}
	//printf("%d %f\n",contador,times );
}

int main(int argc,char **argv){
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,700);
	glutCreateWindow("Parte4");	
	spherical2Cartesian();
#ifndef __APPLE__
// init GLEW
	glewInit();
#endif
	ilInit();
	loadScene(&l,argv[1]);
	
// Required callback registry
	glutIdleFunc(timing);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutSpecialFunc(processSpecialKeys);
//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_RESCALE_NORMAL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
// enter GLUT's main cycle
	glutMainLoop();
	return 1;
}