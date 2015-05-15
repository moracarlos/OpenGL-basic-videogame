#pragma once

#include "main.h"
#define MAXVEL 0.2

class CObject{
	private:
		int numVertex, numFaces;
		GLuint vbo, vindex, vbbo, vbbindex;
		vector <GLuint> index, bbindex;
		vector <GLfloat> vertex, color, bbvertex, bbcolor;
		vector <GLfloat> faceNormals, vertexNormals, texture;
		GLfloat quat[4];
		GLfloat xmin, ymin, zmin, xmax, ymax, zmax, xmid, ymid, zmid; //Bounding Box and Center
		GLfloat xMinT, xMaxT;
		GLfloat transFX, transFY, transFZ; //Translation Factor
		GLfloat factorX, factorY, factorZ; //Scale Factor
		float vertexN[3], facesN[3]; //Faces and vertex normals color
		GLuint m_idVAO;
		float ambientObject[3];
		float diffuseObject[3];
		float specularObject[3];
		double velocidad;
		double aceleracion;
		double alpha;
		bool picked;

	public:
		CObject();
		~CObject();
		void display();
		int initialize();
		void displayBB();
		void createBB();
		void FaceNormals();
		void VertexNormals();
		void displayFacesNormals();
		void displayVertexNormals();
		//Set and Get
		void setNumVertex(int num);
		int getNumVertex();
		void setNumFaces(int num);
		int getNumFaces();
		void setIndex(GLuint x);
		vector <GLuint> getIndex();
		void setIndexBB(GLuint x);
		vector <GLuint> getIndexBB();
		void setVertex(GLfloat x);
		GLfloat getVertex(int i);
		void setColor(GLfloat r, GLfloat g, GLfloat b);
		GLfloat* getColor();
		void setVertexBB(GLfloat x);
		vector <GLfloat> getVertexBB();
		void setColorBB(GLfloat r, GLfloat g, GLfloat b);
		GLfloat* getColorBB();
		void setQuat(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		GLfloat* getQuat();
		void setXMin(GLfloat x);
		GLfloat getXMin();
		void setYMin(GLfloat y);
		GLfloat getYMin();
		void setZMin(GLfloat z);
		GLfloat getZMin();
		void setXMax(GLfloat x);
		GLfloat getXMax();
		void setYMax(GLfloat y);
		GLfloat getYMax();
		void setZMax(GLfloat z);
		GLfloat getZMax();
		void setXMid(GLfloat x);
		GLfloat getXMid();
		void setYMid(GLfloat y);
		GLfloat getYMid();
		void setZMid(GLfloat z);
		GLfloat getZMid();
		void setTraslateX(GLfloat x);
		GLfloat getTraslateX();
		void setTraslateY(GLfloat y);
		GLfloat getTraslateY();
		void setTraslateZ(GLfloat z);
		GLfloat getTraslateZ();
		void setScaleX(GLfloat x);
		GLfloat getScaleX();
		void setScaleY(GLfloat y);
		GLfloat getScaleY();
		void setScaleZ(GLfloat z);
		GLfloat getScaleZ();
		void setAmbientObject(float r, float g, float b);
		float* getAmbientObject();
		void setDiffuseObject(float r, float g, float b);
		float* getDiffuseObject();
		void setSpecularObject(float r, float g, float b);
		float* getSpecularObject();
		void speedUp(double delta);
		void speedDown(double delta);
		void turnRight(double delta);
		void turnLeft(double delta);
		void calculatePhysics();
		double getAlpha();
		void setPicked(bool p);
		bool getPicked();
};