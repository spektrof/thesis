#include "Utility.h"
#include "CameraAndLight.h"

Camera::Camera(const glm::vec3& e, const glm::vec3& u, const float& o, const float& t, const float& r)
	: eye(e),up(u),omega(o),theta(t),radius(r)
{
	zunit = Utility::DescartesToPolar(omega, theta, radius);
	at = eye + zunit;
	vunit = glm::vec3(0, 1, 0);
	hunit = glm::normalize(glm::cross(up, zunit));

	ViewPoint = _3D;
}

void Camera::SwitchCameraView(const glm::vec3& v)
{
	ViewPoint = ViewPoint ? _3D : _2D;
	if (ViewPoint) // _2D
	{
		SetCamera2DValues(v);
	}
	else
	{ //3D
		SetCamera3DValues();
	}
}
void Camera::SetCamera2DValues(const glm::vec3& v)
{
	zunit = glm::vec3(0, -1, 0);
	hunit = glm::vec3(0, 0, -1);
	vunit = glm::vec3(1, 0, 0);;
	up = glm::vec3(1, 0, 0);
	SetCamera(v);
}
void Camera::SetCamera3DValues()
{
	omega = 4.0f; theta = 2.0f; 
	zunit = Utility::DescartesToPolar(omega, theta, radius);

	eye = glm::vec3(65, 50, 90);
	up = glm::vec3(0, 1, 0);
	at = eye + zunit;
	vunit = glm::vec3(0, 1, 0);
	hunit = glm::normalize(glm::cross(up, zunit));
}

void Camera::SetCamera(const glm::vec3& v)
{
	eye = v;
	at = glm::vec3(v.x, 0, v.z);
}
void Camera::Add(const glm::vec3& unit)
{
	eye += unit;
	at += unit;
}
void Camera::Sub(const glm::vec3& unit)
{
	if (Is2DView() && eye.y - unit.y < 10) return;

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
	
		zunit = Utility::DescartesToPolar(omega, theta, radius);
		hunit = glm::normalize(glm::cross(up, zunit));

		at = eye + zunit;
	}
}

// ---------------------------------------------------------

Light::Light(const int& c, const float& r, const int& o, const int& t)
	: cunit(c),radius(r)
{
	omega = ((o) % cunit) * (float)((float)1 / (float)cunit);
	theta = ((t) % cunit) * (float)((float)1 / (float)cunit);

	FenyIrany = Utility::DescartesToPolar(-omega * 2 * (float)M_PI, theta * 2 * (float)M_PI, radius);

}

void Light::AddTo(float* unit)
{
	*unit = float( (int)std::round((*unit) * cunit + 1) % cunit ) / (float)cunit;	//visszaszamoljuk az egyseget a noveles elott

	FenyIrany = Utility::DescartesToPolar(-omega * 2 * (float)M_PI, theta * 2 * (float)M_PI, radius);
}
void Light::SubFrom(float* unit)
{
	*unit = float((int)std::round((*unit) * cunit - 1 + cunit) % cunit) / (float)cunit; //visszaszamoljuk az egyseget a noveles elott

	FenyIrany = Utility::DescartesToPolar(-omega * 2 * (float)M_PI, theta * 2 * (float)M_PI, radius);
}