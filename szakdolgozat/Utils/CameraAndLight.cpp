#include "CameraAndLight.h"

Camera::Camera(const glm::vec3& e, const glm::vec3& u, const float& o, const float& t)
	: eye(e),up(u),omega(o),theta(t)
{
	cunit = Utility::DescartesToPolar(omega, theta);
	at = eye + cunit;

	ViewPoint = _3D;
}

void Camera::SwitchCameraView()
{
	ViewPoint = ViewPoint ? _3D : _2D;
	if (ViewPoint) // _2D
	{
		SetLight2DValues();
	}
	else
	{ //3D
		SetLight3DInitialValues();
	}
}
void Camera::SetLight2DValues()
{
	cunit = glm::vec3(1, 0, 0);
	eye = glm::vec3(0, 50, 0);
	up = glm::vec3(1, 0, 0);
	at = glm::vec3(0, 0, 0);
}
void Camera::SetLight3DInitialValues()
{
	omega = 4.0f; theta = 2.0f; 
	cunit = Utility::DescartesToPolar(omega, theta);
	//eye = glm::vec3(50, 35, 50);
	eye = glm::vec3(65, 50, 90);
	up = glm::vec3(0, 1, 0);
	at = eye + cunit;
}

void Camera::Add(const glm::vec3& unit)
{
	eye += unit;
	at += unit;
}
void Camera::Sub(const glm::vec3& unit)
{
	eye -= unit;
	at -= unit;
}

void Camera::MouseMove(SDL_MouseMotionEvent& mouse)
{
	if (ViewPoint == _2D) return;

	if (is_left_pressed) {
		omega += mouse.xrel / 100.0f;
		theta += mouse.yrel / 100.0f;

		theta = glm::clamp(theta, 0.1f, 3.0f);

		cunit = Utility::DescartesToPolar(omega, theta);
		at = eye + cunit;
	}
}

// ---------------------------------------------------------

Light::Light(const glm::vec3& fi, const float& o, const float& t)
	: FenyIrany(fi),omega(o),theta(t)
{
	lunit = Utility::DescartesToPolar(-omega, theta);
	FenyIrany += lunit;
}

void Light::Add(const glm::vec3& unit)
{
	FenyIrany += unit;
}
void Light::Sub(const glm::vec3& unit)
{
	FenyIrany -= unit;
}