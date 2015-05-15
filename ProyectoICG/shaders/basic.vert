#version 330

uniform mat4 mProjection, mModelView;
uniform vec3 scaleFactor, translateFactor, centerPosition;
uniform vec4 quat;
uniform vec3 lightPos, eyePos, ld, ls, ambient;
uniform vec3 diffuseObject, specularObject, ambientObject;

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;

out vec3 vVertexColor;
out vec3 fNormal;
out vec3 fPosition;
out vec3 lightPosFrag;
out vec3 eyePosFrag;
out vec3 ldFrag;
out vec3 lsFrag;
out vec3 ambientFrag;
out vec3 diffuseObjectFrag;
out vec3 specularObjectFrag;
out vec3 ambientObjectFrag;

void main()
{
    //Calcular la matriz normal
    //Transformaciones
    vVertexColor = clamp(vNormal,0,1);
    
    mat4 returnFig = mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(centerPosition[0],centerPosition[1],centerPosition[2],1));
    
    mat4 centerFig = mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(-centerPosition[0],-centerPosition[1],-centerPosition[2],1));
    
    mat4 translateMatrix=mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(translateFactor[0], translateFactor[1], translateFactor[2],1));

    mat4 scaleMatrix = mat4(vec4(scaleFactor[0],0,0,0), vec4(0,scaleFactor[1],0,0), vec4(0,0,scaleFactor[2],0), vec4(0,0,0,1));
    
    mat4 rotateMatrix;

    rotateMatrix[0][0] = 1.0f - 2.0f * (quat[1] * quat[1] + quat[2] * quat[2]);
    rotateMatrix[0][1] = 2.0f * (quat[0] * quat[1] + quat[2] * quat[3]);
    rotateMatrix[0][2] = 2.0f * (quat[0] * quat[2] - quat[1] * quat[3]);
    rotateMatrix[0][3] = 0.0f;
    rotateMatrix[1][0] = 2.0f * (quat[0] * quat[1] - quat[2] * quat[3]);
    rotateMatrix[1][1] = 1.0f - 2.0f * (quat[0] * quat[0] + quat[2] * quat[2]);
    rotateMatrix[1][2] = 2.0f * (quat[1] * quat[2] + quat[0] * quat[3]);
    rotateMatrix[1][3] = 0.0f;
    rotateMatrix[2][0] = 2.0f * (quat[0] * quat[2] + quat[1] * quat[3]);
    rotateMatrix[2][1] = 2.0f * (quat[1] * quat[2] - quat[0] * quat[3]);
    rotateMatrix[2][2] = 1.0f - 2.0f * (quat[0] * quat[0] + quat[1] * quat[1]);
    rotateMatrix[2][3] = 0.0f;
    rotateMatrix[3][0] = 0.0f;
    rotateMatrix[3][1] = 0.0f;
    rotateMatrix[3][2] = 0.0f;
    rotateMatrix[3][3] = 1.0f;
    
	mat4 MV = translateMatrix* returnFig*rotateMatrix*scaleMatrix*centerFig*mModelView;
	mat3 normalMatrix;
	mat4 invTransp = inverse(transpose(MV));
	normalMatrix = mat3(vec3(invTransp[0][0], invTransp[0][1], invTransp[0][2]), vec3(invTransp[1][0], invTransp[1][1], invTransp[1][2]), vec3(invTransp[2][0], invTransp[2][1], invTransp[2][2]));
	

	fNormal = normalize(normalMatrix * vNormal);
	vec4 pos = MV * vec4(vVertex, 1.0);
	fPosition = pos.xyz;
	gl_Position = mProjection * pos;
    lightPosFrag = lightPos;
    eyePosFrag = eyePos;
    ldFrag = ld;
    lsFrag = ls;
    ambientFrag = ambient;
    
    diffuseObjectFrag = diffuseObject;
    specularObjectFrag = specularObject;
    ambientObjectFrag = ambientObject;

    //gl_Position = mProjection *MV*vec4(vVertex, 1.0);
}