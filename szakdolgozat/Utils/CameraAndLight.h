#pragma once

/*	Keszitette: Lukacs Peter

	A kamera- es fenykezeleshez szukseges osztalyok.
	Segitsegukkel mozgathatjuk kamerankat es a fenyforrasunkat.
*/
#include "glm/glm.hpp"
#include "Utility.h"

class Camera
{
public:
	 Camera(const glm::vec3& = glm::vec3(40, 30, 50), const glm::vec3& = glm::vec3(0, 1, 0), const float& = 4.0f, const float& = 2.0f, const float& r = 1.0f);
	~Camera() {  }

	void SetIsLeftPressed(const bool& val) { is_left_pressed = val; }

	void SetCamera(const glm::vec3&);
	glm::vec3 GetEye() const { return eye; }
	glm::vec3 GetUp() const { return up; }
	glm::vec3 GetAt() const { return at; }
	glm::vec3 GetVertUnit() const { return vunit; }
	glm::vec3 GetZoomUnit() const { return zunit; }
	glm::vec3 GetHorUnit() const { return hunit; }
	float GetView() const { return (float)ViewPoint; }
	bool Is2DView() const { return ViewPoint == _2D; }

	void SwitchCameraView(const glm::vec3& = glm::vec3(40, 30, 50));
	void Add(const glm::vec3&);
	void Sub( const glm::vec3&);

	void MouseMove(SDL_MouseMotionEvent&);

protected:
	void SetCamera2DValues(const glm::vec3&);
	void SetCamera3DValues(const glm::vec3&);

	bool is_left_pressed = false;	// camera moving

	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 at;

private:
	float omega, theta;				//forgatashoz, a gomb ket szoge
	glm::vec3 zunit,vunit,hunit;	//kameraegységek
	float radius;					//gomb sugara

	enum ViewPos {
		_3D, 
		_2D
	};
	ViewPos ViewPoint;
};

class Light
{
public:
	Light(const int& = -1, const int& c = 100, const float& r = 20.0f, const int& = 28, const int& = 88);
	~Light() {  }

	glm::vec3 GetLightDir() const { return LightDir; }
	float* GetOmega() { return &omega; }
	float* GetTheta() { return &theta; }

	void AddTo(float* unit);
	void SubFrom(float* unit);

protected:
	glm::vec3 LightDir;	//Origobol a feny pontjaba mutato vektor
	float omega, theta;

private:
	int cunit;	//circle unit
	int lunit;	//light unit
	float radius;
};
