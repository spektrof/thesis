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

/*3D-s es 2D-s kameranezet kotorri valtas*/
void Camera::SwitchCameraView(const glm::vec3& v)
{
	ViewPoint = ViewPoint ? _3D : _2D;
	if (ViewPoint) // _2D
	{
		SetCamera2DValues(v);
	}
	else
	{ //3D
		SetCamera3DValues(v);
	}
}

/*2D-s kameraertekek beallitasa*/
void Camera::SetCamera2DValues(const glm::vec3& v)
{
	zunit = glm::vec3(0, -1, 0);
	hunit = glm::vec3(0, 0, -1);
	vunit = glm::vec3(1, 0, 0);;
	up = glm::vec3(1, 0, 0);
	SetCamera(v);
}

/*3D-s kameraertekek beallitasa*/
void Camera::SetCamera3DValues(const glm::vec3& v)
{
	zunit = Utility::DescartesToPolar(omega, theta, radius);

	eye = v;
	up = glm::vec3(0, 1, 0);
	at = eye + zunit;
	vunit = glm::vec3(0, 1, 0);
	hunit = glm::normalize(glm::cross(up, zunit));
}

/*csak 2D-ben ket adattag beallitasa*/
void Camera::SetCamera(const glm::vec3& v)
{
	eye = v;
	at = glm::vec3(v.x, 0, v.z);
}

/*Noveles, mozgashoz*/
void Camera::Add(const glm::vec3& unit)
{
	if (Is2DView() && eye.y + unit.y < 10) return;

	eye += unit;
	at += unit;
}

/*Csokkentes, mozgashoz*/
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
	
		zunit = Utility::DescartesToPolar(omega, theta, radius);
		hunit = glm::normalize(glm::cross(up, zunit));

		at = eye + zunit;
	}
}

// ---------------------------------------------------------

Light::Light(const int& l, const int& c, const float& r, const int& o, const int& t)
	: cunit(c),radius(r)
{
	lunit = l;
	omega = ((o) % cunit) * (float)((float)1 / (float)cunit);
	theta = ((t) % cunit) * (float)((float)1 / (float)cunit);

	LightDir = Utility::DescartesToPolar(-omega * 2 * (float)M_PI, theta * 2 * (float)M_PI, radius);	//2Pi - 2Pi mert mindket iranyba korbe akarjuk tudni mozgatni a fenyt
}

void Light::AddTo(float* unit)
{
	*unit = float( (int)std::round((*unit) * cunit + lunit) % cunit ) / (float)cunit;	//visszaszamoljuk az egyseget a noveles elott

	LightDir = Utility::DescartesToPolar(-omega * 2 * (float)M_PI, theta * 2 * (float)M_PI, radius);
}
void Light::SubFrom(float* unit)
{
	*unit = float((int)std::round((*unit) * cunit - lunit + cunit) % cunit) / (float)cunit; //visszaszamoljuk az egyseget a csokkentes elott
	
	LightDir = Utility::DescartesToPolar(-omega * 2 * (float)M_PI, theta * 2 * (float)M_PI, radius);
}