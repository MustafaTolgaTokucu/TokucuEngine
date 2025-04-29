#pragma once

#include "GLFW/glfw3.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Tokucu {
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	

	class Camera {

	public:
		Camera();

		static std::shared_ptr<Camera> GetInstance() {
			static std::shared_ptr<Camera> instance = std::make_shared<Camera>();
			return instance;
		}

		glm::mat4 view;
		float cameraSpeed;
		glm::vec3 Position = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 beginFront;
		void ProcessKeyboard(Camera_Movement direction);
		glm::mat4 GetView();
		glm::vec3 GetPosition();
		glm::vec3 GetFront();
		void updateCameraVectors(float m_Yaw, float m_Pitch);
	
	private:

		float deltaTime = 0.0f;	// Time between current frame and last frame
		float lastFrame = 0.0f; // Time of last frame
		float currentFrame;


		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraDirection = glm::normalize(Position - cameraTarget);

		glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 Right = glm::normalize(glm::cross(WorldUp, cameraDirection));
		glm::vec3 Up = glm::cross(cameraDirection, Right);



		const float radius = 10.0f;
		

		

		// euler Angles
		float yaw=-90;
		float pitch ;
		float MouseSensitivity= 0.000001f;
		bool firstMouse = true;
	};

}
