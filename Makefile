GLUT=-lGLU -lGL -lglut -lGLEW -lIL -std=gnu++11

all:motor gerador

motor:catmull.cpp pugixml.cpp motor.cpp
	g++ -g -o motor catmull.cpp pugixml.cpp motor.cpp $(GLUT)

gerador:catmull.cpp gerador.cpp
	g++ -o gerar catmull.cpp gerador.cpp $(GLUT)
