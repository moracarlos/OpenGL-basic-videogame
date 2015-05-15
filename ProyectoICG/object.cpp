#include "object.h"
#define PI 3.14159265359


extern CGLSLProgram m_program;
extern glm::mat4 mModelViewMatrix;
extern glm::mat4 mProjMatrix;
extern float lightPos[3];
extern float eyePos[3];
extern float ld[3];
extern float ls[3];
extern float ambient[3];
extern CCamera* myCamera;
extern bool collision;
extern bool collide();

CObject::CObject(){
	numFaces = 0;
	numVertex = 0;
	transFX = 0; transFY = 0; transFZ = 0;
	xMinT = 0; xMaxT = 0; 
	factorX = 1; factorY = 1; factorZ = 1;
	quat[0] = 0; quat[1] = 0; quat[2] = 0; quat[3] = 1;
	vertexN[0] = 0.0f; vertexN[1] = 0.0f; vertexN[2] = 1.0f;
	facesN[0] = 1.0f; facesN[1] = 0.0f; facesN[2] = 0.0f;
	ambientObject[0] = 0.0f; ambientObject[1] = 0.5f; ambientObject[2] = 0.7f;
	diffuseObject[0] = 0.0f; diffuseObject[1] = 1.0f; diffuseObject[2] = 0.0f;
	specularObject[0] = 0.0f; specularObject[1] = 1.0f; specularObject[2] = 1.0f;
	velocidad = aceleracion = alpha = 0;
	xmid = ymid = zmid = 0;
	picked = false;
}

CObject::~CObject(){
	index.~vector<GLuint>();
	bbindex.~vector<GLuint>();
	color.~vector<GLfloat>();
	vertex.~vector<GLfloat>();
	bbvertex.~vector<GLfloat>();
	bbcolor.~vector<GLfloat>();
	faceNormals.~vector<GLfloat>();
	vertexNormals.~vector<GLfloat>();
	//texture.~vector<GLfloat>();
}

void CObject::display(){
	m_program.enable();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vindex);

	glUniformMatrix4fv(m_program.getLocation("mModelView"), 1, GL_FALSE, glm::value_ptr(mModelViewMatrix));
	glUniformMatrix4fv(m_program.getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));
	glUniform3f(m_program.getLocation("scaleFactor"), factorX, factorY, factorZ);
	glUniform3f(m_program.getLocation("translateFactor"), transFX, transFY, transFZ);
	glUniform4f(m_program.getLocation("quat"), quat[0], quat[1], quat[2], quat[3]);
	glUniform3f(m_program.getLocation("centerPosition"), (xmin + xmax) / 2, (ymin + ymax) / 2, (zmin + zmax) / 2);

	glUniform3fv(m_program.getLocation("lightPos"), 1, lightPos);
	glUniform3fv(m_program.getLocation("eyePos"), 1, eyePos);
	glUniform3fv(m_program.getLocation("ld"), 1, ld);
	glUniform3fv(m_program.getLocation("ls"), 1, ls);
	glUniform3fv(m_program.getLocation("ambient"), 1, ambient);

	glUniform3fv(m_program.getLocation("diffuseObject"), 1, diffuseObject);
	glUniform3fv(m_program.getLocation("specularObject"), 1, specularObject);
	glUniform3fv(m_program.getLocation("ambientObject"), 1, ambientObject);

	glBindVertexArray(m_idVAO);
	glDrawElements(GL_TRIANGLES, sizeof(GLuint)*index.size(), GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_program.disable();
}

// Init all data and variables.
int CObject::initialize(){
	//Generate the Vertex Array
	glGenVertexArrays(1, &m_idVAO);
	glBindVertexArray(m_idVAO);

	//Generate the buffers
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vindex);
	glGenBuffers(1, &vbbo);
	glGenBuffers(1, &vbbindex);

	//Bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Allocate space for the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex.size() + sizeof(float)*color.size() + sizeof(float)*vertexNormals.size(), NULL, GL_DYNAMIC_DRAW);
	//Pass the vertex and the color data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*vertex.size(), vertex.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*vertex.size(), sizeof(float)*color.size(), color.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*color.size() + sizeof(float)*vertex.size(), sizeof(float)*vertexNormals.size(), vertexNormals.data());

	glEnableVertexAttribArray(m_program.getLocation("vVertex"));
	glVertexAttribPointer(m_program.getLocation("vVertex"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); //Vertex
	glEnableVertexAttribArray(m_program.getLocation("vColor"));
	glVertexAttribPointer(m_program.getLocation("vColor"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex.size()*sizeof(float))); //Colors
	glEnableVertexAttribArray(m_program.getLocation("vNormal"));
	glVertexAttribPointer(m_program.getLocation("vNormal"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex.size()*sizeof(float)+color.size()*sizeof(float))); //Normals

	//Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vindex);
	//Fill the index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*index.size(), index.data(), GL_STATIC_DRAW);

	//Unbind Vertex Array
	glBindVertexArray(0);

	//Bounding Box
	glBindBuffer(GL_ARRAY_BUFFER, vbbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*bbvertex.size() + sizeof(float)*bbcolor.size(), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*bbvertex.size(), bbvertex.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*bbvertex.size(), sizeof(float)*bbcolor.size(), bbcolor.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbbindex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*bbindex.size(), bbindex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return true;
}

void CObject::displayBB(){
	glBindBuffer(GL_ARRAY_BUFFER, vbbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbbindex);

	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*bbvertex.size(), sizeof(float)*bbcolor.size(), bbcolor.data());

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(sizeof(float)*bbvertex.size()));
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawElements(GL_LINES, sizeof(GLuint)*bbindex.size(), GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CObject::createBB(){
	//v0
	bbvertex.push_back(xmin);
	bbvertex.push_back(ymin);
	bbvertex.push_back(zmin);
	//v1
	bbvertex.push_back(xmax);
	bbvertex.push_back(ymin);
	bbvertex.push_back(zmin);
	//v2
	bbvertex.push_back(xmin);
	bbvertex.push_back(ymax);
	bbvertex.push_back(zmin);
	//v3
	bbvertex.push_back(xmax);
	bbvertex.push_back(ymax);
	bbvertex.push_back(zmin);
	//v4
	bbvertex.push_back(xmax);
	bbvertex.push_back(ymin);
	bbvertex.push_back(zmax);
	//v5
	bbvertex.push_back(xmax);
	bbvertex.push_back(ymax);
	bbvertex.push_back(zmax);
	//v6
	bbvertex.push_back(xmin);
	bbvertex.push_back(ymax);
	bbvertex.push_back(zmax);
	//v7
	bbvertex.push_back(xmin);
	bbvertex.push_back(ymin);
	bbvertex.push_back(zmax);

	//back v0,v1,v2,v3
	bbindex.push_back(0);
	bbindex.push_back(1);
	bbindex.push_back(2);
	bbindex.push_back(0);
	bbindex.push_back(2);
	bbindex.push_back(3);
	bbindex.push_back(3);
	bbindex.push_back(1);

	//up v6,v5,v3,v2
	bbindex.push_back(5);
	bbindex.push_back(3);
	bbindex.push_back(6);
	bbindex.push_back(2);

	//down v7,v4,v1,v0
	bbindex.push_back(4);
	bbindex.push_back(1);
	bbindex.push_back(7);
	bbindex.push_back(0);

	//front v4,v5,v6,v7
	bbindex.push_back(4);
	bbindex.push_back(5);
	bbindex.push_back(5);
	bbindex.push_back(6);
	bbindex.push_back(6);
	bbindex.push_back(7);
	bbindex.push_back(4);
	bbindex.push_back(7);

	//Color
	for (int i = 0; i < bbvertex.size(); i += 3){
		bbcolor.push_back(1);
		bbcolor.push_back(1);
		bbcolor.push_back(1);
	}
}

void CObject::FaceNormals(){
	float alpha = 0.1f;
	int i, j;
	glm::vec3 v1, v2, c, p1, p2, p3, n;
	for (i = 0; i < index.size(); i += 3){
		p1 = glm::vec3(vertex[index[i] * 3], vertex[index[i] * 3 + 1], vertex[index[i] * 3 + 2]);
		p2 = glm::vec3(vertex[index[i + 1] * 3], vertex[index[i + 1] * 3 + 1], vertex[index[i + 1] * 3 + 2]);
		p3 = glm::vec3(vertex[index[i + 2] * 3], vertex[index[i + 2] * 3 + 1], vertex[index[i + 2] * 3 + 2]);
		v1 = glm::normalize(p2 - p1);
		v2 = glm::normalize(p3 - p1);
		c = (p1 + p2 + p3) / 3.f;
		n = glm::normalize(glm::cross(v1, v2)); //Aqui esta el cambio

		faceNormals.push_back(c[0]);
		faceNormals.push_back(c[1]);
		faceNormals.push_back(c[2]);

		faceNormals.push_back(n[0]);
		faceNormals.push_back(n[1]);
		faceNormals.push_back(n[2]);
	}
}

void CObject::VertexNormals(){
	int i, j = 0;
	vertexNormals.clear();
	for (i = 0; i < vertex.size(); i++){
		vertexNormals.push_back(0);
	}

	for (i = 0; i < index.size(); i += 3){
		float x = faceNormals[2 * i + 3], y = faceNormals[2 * i + 4], z = faceNormals[2 * i + 5];

		vertexNormals[index[i] * 3] += x;
		vertexNormals[index[i] * 3 + 1] += y;
		vertexNormals[index[i] * 3 + 2] += z;

		vertexNormals[index[i + 1] * 3] += x;
		vertexNormals[index[i + 1] * 3 + 1] += y;
		vertexNormals[index[i + 1] * 3 + 2] += z;

		vertexNormals[index[i + 2] * 3] += x;
		vertexNormals[index[i + 2] * 3 + 1] += y;
		vertexNormals[index[i + 2] * 3 + 2] += z;
	}

	glm::vec3 n;
	float alpha = 0.1f;
	for (i = 0; i < vertexNormals.size(); i += 3){
		n = glm::vec3(vertexNormals[i], vertexNormals[i + 1], vertexNormals[i + 2]);
		n = glm::normalize(n);

		//vertexNormals[i] = vertex[i] + n[0] * alpha;
		//vertexNormals[i + 1] = vertex[i + 1] + n[1] * alpha;
		//vertexNormals[i + 2] = vertex[i + 2] + n[2] * alpha;

		vertexNormals[i] = n[0];
		vertexNormals[i + 1] = n[1];
		vertexNormals[i + 2] = n[2];
	}
}

void CObject::displayFacesNormals(){
	int i;
	glColor3fv(facesN);
	for (i = 0; i < faceNormals.size(); i += 6){
		glBegin(GL_LINES);
		glVertex3f(faceNormals[i], faceNormals[i + 1], faceNormals[i + 2]);
		glVertex3f(faceNormals[i + 3] * 0.1f + faceNormals[i],
			faceNormals[i + 4] * 0.1f + faceNormals[i + 1],
			faceNormals[i + 5] * 0.1f + faceNormals[i + 2]);
		glEnd();
	}
}

void CObject::displayVertexNormals(){
	int i;
	glColor3fv(vertexN);
	for (i = 0; i < vertexNormals.size(); i += 3){
		glBegin(GL_LINES);
		glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
		glVertex3f(vertexNormals[i] * 0.1f + vertex[i],
			vertexNormals[i + 1] * 0.1f + vertex[i + 1],
			vertexNormals[i + 2] * 0.1f + vertex[i + 2]);
		glEnd();
	}
}

void CObject::setNumVertex(int num){
	numVertex = num;
}

int CObject::getNumVertex(){
	return numVertex;
}

void CObject::setNumFaces(int num){
	numFaces = num;
}

int CObject::getNumFaces(){
	return numFaces;
}

void CObject::setIndex(GLuint x){
	index.push_back(x);
}

vector <GLuint> CObject::getIndex(){
	return index;
}

void CObject::setIndexBB(GLuint x){
	bbindex.push_back(x);
}

vector <GLuint> CObject::getIndexBB(){
	return bbindex;
}

void CObject::setVertex(GLfloat x){
	vertex.push_back(x);
}

GLfloat CObject::getVertex(int i){
	return vertex[i];
}

void CObject::setColor(GLfloat r, GLfloat g, GLfloat b){
	int i;
	for (i = 0; i < color.size(); i += 3){
		color[i] = r;
		color[i + 1] = g;
		color[i + 2] = b;
	}
}

GLfloat* CObject::getColor(){
	GLfloat colorFilling[3];
	colorFilling[0] = color[0];
	colorFilling[1] = color[1];
	colorFilling[2] = color[2];
	return colorFilling;
}

void CObject::setVertexBB(GLfloat x){
	bbvertex.push_back(x);
}

vector <GLfloat> CObject::getVertexBB(){
	return bbvertex;
}

void CObject::setColorBB(GLfloat r, GLfloat g, GLfloat b){
	int i;
	for (i = 0; i < bbcolor.size(); i += 3){
		bbcolor[i] = r;
		bbcolor[i + 1] = g;
		bbcolor[i + 2] = b;
	}
}

GLfloat* CObject::getColorBB(){
	GLfloat colorBB[3];
	colorBB[0] = bbcolor[0];
	colorBB[1] = bbcolor[1];
	colorBB[2] = bbcolor[2];
	return colorBB;
}

void CObject::setQuat(GLfloat x, GLfloat y, GLfloat z, GLfloat w){
	quat[0] = x;
	quat[1] = y;
	quat[2] = z;
	quat[3] = w;
}

GLfloat* CObject::getQuat(){
	GLfloat rotate[4];
	rotate[0] = quat[0];
	rotate[1] = quat[1];
	rotate[2] = quat[2];
	rotate[3] = quat[3];
	return rotate;
}

void CObject::setXMin(GLfloat x){
	xmin = x;
}

GLfloat CObject::getXMin(){
	return xmin;
}

void CObject::setYMin(GLfloat y){
	ymin = y;
}

GLfloat CObject::getYMin(){
	return ymin;
}

void CObject::setZMin(GLfloat z){
	zmin = z;
}

GLfloat CObject::getZMin(){
	return zmin;
}

void CObject::setXMax(GLfloat x){
	xmax = x;
}

GLfloat CObject::getXMax(){
	return xmax;
}

void CObject::setYMax(GLfloat y){
	ymax = y;
}

GLfloat CObject::getYMax(){
	return ymax;
}

void CObject::setZMax(GLfloat z){
	zmax = z;
}

GLfloat CObject::getZMax(){
	return zmax;
}

void CObject::setXMid(GLfloat x){
	xmid = x;
}

GLfloat CObject::getXMid(){
	return xmid;
}

void CObject::setYMid(GLfloat y){
	ymid = y;
}

GLfloat CObject::getYMid(){
	return ymid;
}

void CObject::setZMid(GLfloat z){
	zmid = z;
}

GLfloat CObject::getZMid(){
	return zmid;
}

void CObject::setTraslateX(GLfloat x){
	transFX = x;
}

GLfloat CObject::getTraslateX(){
	return transFX;
}

void CObject::setTraslateY(GLfloat y){
	transFY = y;
}

GLfloat CObject::getTraslateY(){
	return transFY;
}

void CObject::setTraslateZ(GLfloat z){
	transFZ = z;
}

GLfloat CObject::getTraslateZ(){
	return transFZ;
}

void CObject::setScaleX(GLfloat x){
	factorX = x;
}

GLfloat CObject::getScaleX(){
	return factorX;
}

void CObject::setScaleY(GLfloat y){
	factorY = y;
}

GLfloat CObject::getScaleY(){
	return factorY;
}

void CObject::setScaleZ(GLfloat z){
	factorZ = z;
}

GLfloat CObject::getScaleZ(){
	return factorZ;
}

void CObject::setAmbientObject(float r, float g, float b)
{
	ambientObject[0] = r;
	ambientObject[1] = g;
	ambientObject[2] = b;
}

float* CObject::getAmbientObject()
{
	return ambientObject;
}

void CObject::setDiffuseObject(float r, float g, float b)
{
	diffuseObject[0] = r;
	diffuseObject[1] = g;
	diffuseObject[2] = b;
}

float* CObject::getDiffuseObject()
{
	return diffuseObject;
}

void CObject::setSpecularObject(float r, float g, float b)
{
	specularObject[0] = r;
	specularObject[1] = g;
	specularObject[2] = b;
}

float* CObject::getSpecularObject()
{
	return specularObject;
}

void CObject::speedUp(double delta)
{
	velocidad += delta;
}

void CObject::speedDown(double delta)
{
	velocidad -= delta;
}

void CObject::turnRight(double delta)
{
	if (velocidad != 0 && !collision){
		if (velocidad > 0) alpha += delta; else if (velocidad < 0) alpha -= delta;
		if (alpha > 360 || alpha < -360) alpha = 0;
	}
}

void CObject::turnLeft(double delta)
{
	if (velocidad != 0 && !collision){
		if (velocidad > 0) alpha -= delta; else if (velocidad < 0) alpha += delta;
		if (alpha < -360 || alpha > 360) alpha = 0;
	}
}

double CObject::getAlpha()
{
	return alpha;
}

void CObject::calculatePhysics()
{
	glm::quat myquaternion(glm::vec3(0, (PI / 180)*(alpha + 180), 0));
	quat[2] = myquaternion.x;
	quat[3] = myquaternion.y;
	quat[0] = myquaternion.z;
	quat[1] = myquaternion.w;
	if (velocidad > 0){
		velocidad -= 0.00025;
		if (velocidad < 0) velocidad = 0;
		if (velocidad > MAXVEL) velocidad = MAXVEL;
	}
	else if (velocidad < 0){
		velocidad += 0.00025;
		if (velocidad > 0) velocidad = 0;
		if (velocidad < -0.1) velocidad = -0.1f;
	}
	glm::vec3 oldEye, oldCenter;
	oldEye = myCamera->getEye();
	oldCenter = myCamera->getCenter();
	if (velocidad != 0){
		transFZ += velocidad*sin((PI / 180)*(alpha + 90));
		transFX += velocidad*cos((PI / 180)*(alpha + 90));
		collision = collide();
		if (collision){
			transFZ -= velocidad*sin((PI / 180)*(alpha + 90));
			transFX -= velocidad*cos((PI / 180)*(alpha + 90));
			velocidad = 0;
		}
		myCamera->setCenter(glm::vec3(xmid + transFX, ymid, zmid + transFZ));
	}
}
bool CObject::getPicked(){
	return picked;
}

void CObject::setPicked(bool p){
	picked = p;
}