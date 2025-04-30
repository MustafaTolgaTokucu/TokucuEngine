#include "tkcpch.h"
#include "Camera.h"

namespace Tokucu{
	Camera::Camera()
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		cameraSpeed = 0.005f * deltaTime;
		beginFront = Front;
		/*direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));*/
	}
	void Camera::ProcessKeyboard(Camera_Movement direction)
	{
		if (direction == FORWARD)
			Position += Front * cameraSpeed;
		if (direction == BACKWARD)
			Position -= Front * cameraSpeed;
		if (direction==RIGHT)
			Position += Right * cameraSpeed;
		if (direction==LEFT)
			Position -= Right * cameraSpeed;
	}
	glm::mat4 Camera::GetView()
	{
		view = glm::lookAt(Position, Position + Front, Up);
		//TKC_CORE_INFO("{0} {1} {2} ",cameraFront.x ,cameraFront.y, cameraFront.z);
		return view;
	}

	glm::vec3 Camera::GetPosition()
	{
		return Position;
	}

	glm::vec3 Camera::GetFront()
	{
		return Front;
	}

	void Camera::updateCameraVectors(float m_Yaw,float m_Pitch)
	{
		// calculate the new Front vector
		yaw = m_Yaw;
		pitch = m_Pitch;
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		Front = glm::normalize(front);
		glm::float32 angle = glm::dot(Front, beginFront);

		//TKC_CORE_INFO("Angle {0}", angle);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
	
}