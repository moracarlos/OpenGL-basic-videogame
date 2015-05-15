#pragma once

#include "Header/glm/fwd.hpp"
#include "Header/glm/glm.hpp"
#include "Header/glm/mat4x4.hpp"
#include "Header/glm/matrix.hpp"
#include "Header/glm/gtc/type_ptr.hpp"
#include "Header/glm/gtc/quaternion.hpp"
#include "Header/glm/gtc/matrix_transform.hpp"
#include <iostream>

class CCamera{
	private:
		glm::vec3 eye;
		glm::vec3 center;
		glm::vec3 up;
	public:
		CCamera(glm::vec3 e, glm::vec3 c);
		void setEye(glm::vec3 e);
		void setCenter(glm::vec3 e);
		glm::vec3 getEye();
		glm::vec3 getCenter();
};