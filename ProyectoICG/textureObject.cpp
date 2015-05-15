#include "textureObject.h"
using namespace std;

extern CGLSLProgram m_texture_program;
extern glm::mat4 mModelViewMatrix;
extern glm::mat4 mProjMatrix;
extern CTexturedObject* menuTexture;

CTexturedObject::CTexturedObject(string path, string texturePath, string textureCoordsPath)
{
	numFaces = 0;
	numVertex = 0;
	transFX = 0; transFY = 0; transFZ = 0;
	factorX = 1; factorY = 1; factorZ = 1;
	quat[0] = 0; quat[1] = 0; quat[2] = 0; quat[3] = 1;
	vertexN[0] = 0.0f; vertexN[1] = 0.0f; vertexN[2] = 1.0f;
	facesN[0] = 1.0f; facesN[1] = 0.0f; facesN[2] = 0.0f;
	ambientObject[0] = 1.0f; ambientObject[1] = 1.0f; ambientObject[2] = 1.0f;
	diffuseObject[0] = 1.0f; diffuseObject[1] = 1.0f; diffuseObject[2] = 1.0f;
	specularObject[0] = 1.0f; specularObject[1] = 1.0f; specularObject[2] = 1.0f;
	velocidad = aceleracion = alpha = 0;
	xmid = ymid = zmid = 0;

	//Load file
	std::fstream file(path, std::fstream::in | std::fstream::out);

	//
	int i, j = 0, indexMinMax = 0;
	GLfloat point; //read vertex
	vector <GLuint> fc; //store faces
	GLuint pt; //read faces
	string type, subtype, line;
	//OBJ
	fc.clear();
	while (!file.eof()){
		subtype = type.substr(0, 1);
		if (subtype == "#"){ //Comments
			std::getline(file, line);
		}
		if (type == "vt"){ //Texture Vertex
			std::getline(file, line);
		}
		if (type == "vn"){ //Normal Vertexx
			std::getline(file, line);
		}
		if (type == "v"){ //Vertex
			for (i = 0; i < 3; i++){
				file >> point;
				setVertex(point);
			}
			setColor(0, 1, 0);

			setNumVertex(getNumVertex() + 1);
			//
			calculateMinMax(indexMinMax);
			indexMinMax++;
			//
		}
		if (type == "f"){ //faces v/vt/vn
			std::getline(file, line);
			char * tok = strtok(&line[0u], " ");
			while (tok != NULL){
				string num = tok;
				size_t pos = num.find("/");
				num = num.substr(0, pos);
				pt = stoi(num);
				fc.push_back(pt);
				tok = strtok(NULL, " ");
			}
			setNumFaces(getNumFaces() + 1);
			for (j = 1; j < fc.size() - 1; j++){
				setIndex(fc[0] - 1);
				setIndex(fc[j] - 1);
				setIndex(fc[j + 1] - 1);
			}
			fc.clear();
		}
		file >> type;
	}

	setXMid((getXMin() + getXMax()) / 2);
	setYMid((getYMin() + getYMax()) / 2);
	setZMid((getZMin() + getZMax()) / 2);


	FaceNormals();
	VertexNormals();

	const char* f = texturePath.c_str();
	texture = loadTexture(f);

	loadTextureCoords(textureCoordsPath);
	if (!initialize())
		exit(EXIT_FAILURE);

	file.close();
}

CTexturedObject::~CTexturedObject(){
	index.~vector<GLuint>();
	bbindex.~vector<GLuint>();
	color.~vector<GLfloat>();
	vertex.~vector<GLfloat>();
	bbvertex.~vector<GLfloat>();
	bbcolor.~vector<GLfloat>();
	faceNormals.~vector<GLfloat>();
	vertexNormals.~vector<GLfloat>();
}

void CTexturedObject::calculateMinMax(int i){
	//std::cout << i << std::endl;
	if (i == 0){
		setXMin(getVertex(0));
		setYMin(getVertex(1));
		setZMin(getVertex(2));
		//
		setXMax(getVertex(0));
		setYMax(getVertex(1));
		setZMax(getVertex(2));
	}
	else{
		if (getVertex(i * 3) < getXMin()){
			setXMin(getVertex(i * 3));
		}
		if (getVertex(i * 3 + 1) < getYMin()){
			setYMin(getVertex(i * 3 + 1));
		}
		if (getVertex(i * 3 + 2) < getZMin()){
			setZMin(getVertex(i * 3 + 2));
		}
		//
		if (getVertex(i * 3) > getXMax()){
			setXMax(getVertex(i * 3));
		}
		if (getVertex(i * 3 + 1) > getYMax()){
			setYMax(getVertex(i * 3 + 1));
		}
		if (getVertex(i * 3 + 2) > getZMax()){
			setZMax(getVertex(i * 3 + 2));
		}
	}
}

void CTexturedObject::display(){
	m_texture_program.enable();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vindex);
	glUniformMatrix4fv(m_texture_program.getLocation("mModelView"), 1, GL_FALSE, glm::value_ptr(mModelViewMatrix));
	glUniformMatrix4fv(m_texture_program.getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));
	glBindVertexArray(m_idVAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, sizeof(GLuint)*index.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_texture_program.disable();

}

int CTexturedObject::initialize(){
	//Generate the Vertex Array
	glGenVertexArrays(1, &m_idVAO);
	glBindVertexArray(m_idVAO);

	//Generate the buffers
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vindex);

	//Bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Allocate space for the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex.size() + sizeof(float)*color.size() + sizeof(float)*vertexNormals.size()+sizeof(float)*textureCoord.size(), NULL, GL_DYNAMIC_DRAW);
	//Pass the vertex and the color data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*vertex.size(), vertex.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*vertex.size(), sizeof(float)*color.size(), color.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*color.size() + sizeof(float)*vertex.size(), sizeof(float)*vertexNormals.size(), vertexNormals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*vertex.size() + sizeof(float)*color.size()+sizeof(float)*vertexNormals.size(), sizeof(float)*textureCoord.size(), textureCoord.data());


	glEnableVertexAttribArray(m_texture_program.getLocation("vVertex"));
	glVertexAttribPointer(m_texture_program.getLocation("vVertex"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); //Vertex
	glEnableVertexAttribArray(m_texture_program.getLocation("vColor"));
	glVertexAttribPointer(m_texture_program.getLocation("vColor"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex.size()*sizeof(float))); //Colors
	glEnableVertexAttribArray(m_texture_program.getLocation("vNormal"));
	glVertexAttribPointer(m_texture_program.getLocation("vNormal"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex.size()*sizeof(float) + color.size()*sizeof(float))); //Normals
	glEnableVertexAttribArray(m_texture_program.getLocation("vTexture"));
	glVertexAttribPointer(m_texture_program.getLocation("vTexture"), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex.size()*sizeof(float) + color.size()*sizeof(float)+vertexNormals.size()*sizeof(float)));


	//Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vindex);
	//Fill the index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*index.size(), index.data(), GL_STATIC_DRAW);

	//Unbind Vertex Array
	glBindVertexArray(0);
	return true;
}

void CTexturedObject::FaceNormals(){
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

void CTexturedObject::VertexNormals(){
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

unsigned int CTexturedObject::loadTexture(const char* filename) {
	ilInit();
	ILuint imageID;
	GLuint textureID;
	ILboolean success;
	ILenum error;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	success = ilLoadImage(filename);
	if (success)
	{
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		if (!success){
			error = ilGetError();
			cout << "Image conversion fails" << endl;
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			ilGetInteger(IL_IMAGE_FORMAT),
			ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT),
			0,
			ilGetInteger(IL_IMAGE_FORMAT),
			GL_UNSIGNED_BYTE,
			ilGetData()
			);
		imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
		imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		imageFormat = ilGetInteger(IL_IMAGE_FORMAT);
	}

	ilDeleteImages(1, &imageID);
	std::cout << "Textura creada exitosamente" << endl;
	return textureID;
}

void CTexturedObject::loadTextureCoords(string textureCoordPath)
{
	std::fstream file(textureCoordPath, std::fstream::in | std::fstream::out);
	if (!file.is_open())
		exit(0);
	float tc;
	cout << vertex.size() / 3 << endl;
	for (int i = 0; i < vertex.size()/3 * 2; i++){
		file >> tc;
		textureCoord.push_back(tc);
	}
}

void CTexturedObject::setNumVertex(int num){
	numVertex = num;
}

int CTexturedObject::getNumVertex(){
	return numVertex;
}

void CTexturedObject::setNumFaces(int num){
	numFaces = num;
}

int CTexturedObject::getNumFaces(){
	return numFaces;
}

void CTexturedObject::setIndex(GLuint x){
	index.push_back(x);
}

vector <GLuint> CTexturedObject::getIndex(){
	return index;
}

void CTexturedObject::setIndexBB(GLuint x){
	bbindex.push_back(x);
}

vector <GLuint> CTexturedObject::getIndexBB(){
	return bbindex;
}

void CTexturedObject::setVertex(GLfloat x){
	vertex.push_back(x);
}

GLfloat CTexturedObject::getVertex(int i){
	return vertex[i];
}

void CTexturedObject::setColor(GLfloat r, GLfloat g, GLfloat b){
	int i;
	for (i = 0; i < color.size(); i += 3){
		color[i] = r;
		color[i + 1] = g;
		color[i + 2] = b;
	}
}

GLfloat* CTexturedObject::getColor(){
	GLfloat colorFilling[3];
	colorFilling[0] = color[0];
	colorFilling[1] = color[1];
	colorFilling[2] = color[2];
	return colorFilling;
}

void CTexturedObject::setVertexBB(GLfloat x){
	bbvertex.push_back(x);
}

vector <GLfloat> CTexturedObject::getVertexBB(){
	return bbvertex;
}

void CTexturedObject::setColorBB(GLfloat r, GLfloat g, GLfloat b){
	int i;
	for (i = 0; i < bbcolor.size(); i += 3){
		bbcolor[i] = r;
		bbcolor[i + 1] = g;
		bbcolor[i + 2] = b;
	}
}

GLfloat* CTexturedObject::getColorBB(){
	GLfloat colorBB[3];
	colorBB[0] = bbcolor[0];
	colorBB[1] = bbcolor[1];
	colorBB[2] = bbcolor[2];
	return colorBB;
}

void CTexturedObject::setQuat(GLfloat x, GLfloat y, GLfloat z, GLfloat w){
	quat[0] = x;
	quat[1] = y;
	quat[2] = z;
	quat[3] = w;
}

GLfloat* CTexturedObject::getQuat(){
	GLfloat rotate[4];
	rotate[0] = quat[0];
	rotate[1] = quat[1];
	rotate[2] = quat[2];
	rotate[3] = quat[3];
	return rotate;
}

void CTexturedObject::setXMin(GLfloat x){
	xmin = x;
}

GLfloat CTexturedObject::getXMin(){
	return xmin;
}

void CTexturedObject::setYMin(GLfloat y){
	ymin = y;
}

GLfloat CTexturedObject::getYMin(){
	return ymin;
}

void CTexturedObject::setZMin(GLfloat z){
	zmin = z;
}

GLfloat CTexturedObject::getZMin(){
	return zmin;
}

void CTexturedObject::setXMax(GLfloat x){
	xmax = x;
}

GLfloat CTexturedObject::getXMax(){
	return xmax;
}

void CTexturedObject::setYMax(GLfloat y){
	ymax = y;
}

GLfloat CTexturedObject::getYMax(){
	return ymax;
}

void CTexturedObject::setZMax(GLfloat z){
	zmax = z;
}

GLfloat CTexturedObject::getZMax(){
	return zmax;
}

void CTexturedObject::setXMid(GLfloat x){
	xmid = x;
}

GLfloat CTexturedObject::getXMid(){
	return xmid;
}

void CTexturedObject::setYMid(GLfloat y){
	ymid = y;
}

GLfloat CTexturedObject::getYMid(){
	return ymid;
}

void CTexturedObject::setZMid(GLfloat z){
	zmid = z;
}

GLfloat CTexturedObject::getZMid(){
	return zmid;
}

void CTexturedObject::setTraslateX(GLfloat x){
	transFX = x;
}

GLfloat CTexturedObject::getTraslateX(){
	return transFX;
}

void CTexturedObject::setTraslateY(GLfloat y){
	transFY = y;
}

GLfloat CTexturedObject::getTraslateY(){
	return transFY;
}

void CTexturedObject::setTraslateZ(GLfloat z){
	transFZ = z;
}

GLfloat CTexturedObject::getTraslateZ(){
	return transFZ;
}

void CTexturedObject::setScaleX(GLfloat x){
	factorX = x;
}

GLfloat CTexturedObject::getScaleX(){
	return factorX;
}

void CTexturedObject::setScaleY(GLfloat y){
	factorY = y;
}

GLfloat CTexturedObject::getScaleY(){
	return factorY;
}

void CTexturedObject::setScaleZ(GLfloat z){
	factorZ = z;
}

GLfloat CTexturedObject::getScaleZ(){
	return factorZ;
}

void CTexturedObject::setAmbientObject(float r, float g, float b)
{
	ambientObject[0] = r;
	ambientObject[1] = g;
	ambientObject[2] = b;
}

float* CTexturedObject::getAmbientObject()
{
	return ambientObject;
}

void CTexturedObject::setDiffuseObject(float r, float g, float b)
{
	diffuseObject[0] = r;
	diffuseObject[1] = g;
	diffuseObject[2] = b;
}

float* CTexturedObject::getDiffuseObject()
{
	return diffuseObject;
}

void CTexturedObject::setSpecularObject(float r, float g, float b)
{
	specularObject[0] = r;
	specularObject[1] = g;
	specularObject[2] = b;
}

float* CTexturedObject::getSpecularObject()
{
	return specularObject;
}