#include "Camera.h"

CCamera::CCamera(glm::vec3 e, glm::vec3 c)
{
	eye = glm::vec3(e[0], e[1], e[2]);
	center = glm::vec3(c[0], c[1], c[2]);
	up = glm::vec3(0, 1, 0);
}

void CCamera::setEye(glm::vec3 e)
{
	eye[0] = e[0];
	eye[1] = e[1];
	eye[2] = e[2];
}
void CCamera::setCenter(glm::vec3 c)
{
	center[0] = c[0];
	center[1] = c[1];
	center[2] = c[2];
}
glm::vec3 CCamera::getEye()
{
	return eye;
}
glm::vec3 CCamera::getCenter()
{
	return center;
}