#pragma once

/*	Keszitette: Lukacs Peter

	A kamera- es fenykezeleshez szukseges osztalyok.
	Segitsegukkel mozgathatjuk kamerankat es a fenyforrasunkat.
*/
#include "glm/glm.hpp"

class Camera
{
public:
	 Camera(const glm::vec3& = glm::vec3(65, 50, 90), const glm::vec3& = glm::vec3(0, 1, 0), const float& = 4.0f, const float& = 2.0f);
	~Camera() {  }

	void SetIsLeftPressed(const bool& val) { is_left_pressed = val; }

	void SetCamera(const glm::vec3&);
	glm::vec3 GetEye() const { return eye; }
	glm::vec3 GetUp() const { return up; }
	glm::vec3 GetAt() const { return at; }
	glm::vec3 GetVertUnit() const { return cunit; }
	glm::vec3 GetZoomUnit() const { return Is2DView() ? glm::vec3(0, 1, 0) : cunit;	}
	float GetView() const { return (float)ViewPoint; }
	bool Is2DView() const { return ViewPoint == _2D; }

	glm::vec3 GetCameraUnit() const { return Is2DView() ? glm::vec3(0, 0, -1) : glm::normalize(glm::cross(up, cunit)); }

	void SwitchCameraView(const glm::vec3& = glm::vec3(1,2,1));
	void Add(const glm::vec3&);
	void Sub( const glm::vec3&);

	void MouseMove(SDL_MouseMotionEvent&);

protected:

	void SetCamera2DValues(const glm::vec3&);
	void SetCamera3DValues();

	bool is_left_pressed = false;	// camera moving

	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 at;


private:
	float omega, theta;
	glm::vec3 cunit;	//kameraegység

	enum ViewPos {
		_3D, 
		_2D
	};
	ViewPos ViewPoint;
};

class Light
{
public:
	Light(const glm::vec3& = glm::vec3(1, -1, 1), const float& = 4.0f, const float& = 2.0f);
	~Light() {  }

	glm::vec3 GetLightDir() const { return FenyIrany; }
	glm::vec3 GetLightUnit(const glm::vec3& up) const { return glm::normalize(glm::cross(up, lunit));}
	glm::vec3 GetVertUnit() const { return lunit; }

	void Add(const glm::vec3&);
	void Sub(const glm::vec3&);

protected:
	glm::vec3 FenyIrany;
private:

	float omega, theta;
	glm::vec3 lunit; //fényegység
};
