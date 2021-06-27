#include "Camera.h"
#include <cmath>
#include "Vector.h"
#include "Audio.h"
#include "Game.h"
#include "Object.h"
#include "Error.h"

/*
	[ namespace camera ]

	・ここではカメラを扱う。
	・CameraBaseクラスはカメラを扱うstatic関数を用意。
	
	・object::Object(アクターの動作を扱う)クラスに統合すべきでは？と最近思う。

*/

namespace camera {
	class CameraBase;
}
namespace object {
	using ObjectID = unsigned int;
}

class camera::CameraBase { 

	Vector3 pos;		
	Vector3 target;		
	Vector3 up;
	Vector3 prev_pos;

	Matrix4 CalcInverse_ViewProjectMat4(void);
	Matrix4 createviewmat4(void);
	Matrix4 createprojectmat4(void);
	Matrix4 inverse_createviewmat4(void);
	Matrix4 inverse_createprojectmat4(void);

	float near_zpos;	
	float far_zpos;

	object::ObjectID targetID;
	Matrix4 viewprojmat4_2D;	

	void(CameraBase::* mode)(void);		
	void FPS(void);
	void Race(void);
	const float dhorz = 4.0f, dvert = 4.0f, dtarg = 2.0f, spring = 180.0f;	
	void Orbit(void);
	const float offset = 70.0f;

	CameraBase(void);

public:

// -> Script
	Matrix4 Update(void);	
	void SetCam_zrange(float near, float far);
	void SetTarget(object::ObjectID ID);
	void SetMode(camera::Mode mode);
	void BackProjection(const Vector2& glcoord, Vector3& start, Vector3& direction);
	
	static CameraBase camera;	

};

using namespace camera;

/***************************************************************************

	CameraBase

****************************************************************************/

/*

	・カメラ位置から向いてる対象からビュー行列、射影行列の計算を扱う。

	・幾つかのカメラモードも定義している。
	
		(object::Objectクラスのようにvirtual継承を用いて記述できるようにしたい。)

*/

CameraBase CameraBase::camera;

CameraBase::CameraBase(void) : pos(), target(0,1,0), up(0,0,1), prev_pos(), near_zpos(25.0f), far_zpos(10000.0f)
, viewprojmat4_2D(), targetID(), mode(&CameraBase::FPS)
{}	

Matrix4 CameraBase::Update(void) {
	const Vector3 temp(pos);
	(this->*mode)();
	audio::Audio::Listener(pos, up, target - pos);
	prev_pos = temp;
	return createprojectmat4() * createviewmat4();
}

Matrix4 CameraBase::CalcInverse_ViewProjectMat4(void) {
	return inverse_createviewmat4() * inverse_createprojectmat4();
}

void CameraBase::SetCam_zrange(float near, float far) {
	near_zpos = near;	far_zpos = far;
}

void CameraBase::SetTarget(object::ObjectID ID) { targetID = ID; }

void CameraBase::SetMode(camera::Mode mode) {

	const GLMatrix& glmat(object::Object::Camera::coord(targetID));

	switch (mode) {

	case camera::Mode::FPS: {
		this->mode = &CameraBase::FPS;
		break;
	}
	case camera::Mode::Race: {
		this->mode = &CameraBase::Race;
		pos = glmat.pos - glmat.unitary.Read_Yunit() * dhorz + glmat.unitary.Read_Zunit() * dvert;
		break;
	}
	case camera::Mode::Orbit: {
		this->mode = &CameraBase::Orbit;
		up = glmat.unitary.Read_Zunit();;
		target = glmat.pos;
		pos = target - offset * glmat.unitary.Read_Yunit();
		break;
	}
	default:
		break;
	}

	prev_pos = pos;	

}

void CameraBase::BackProjection(const Vector2& glcoord, Vector3& start, Vector3& direction) {	

	Vector4 temp(CalcInverse_ViewProjectMat4() * Vector4(glcoord[0], glcoord[1], -1, 1)); 
	start = Vector3(temp[0], temp[1], temp[2]) / temp[3];
	temp = CalcInverse_ViewProjectMat4() * Vector4(glcoord[0], glcoord[1], 0, 1);	
	direction = (Vector3(temp[0], temp[1], temp[2]) / temp[3] - start).normalize();

}	

Matrix4 CameraBase::createviewmat4(void) {

	Vector3 k(target - pos);
	k.normalize();

	Vector3 i(Vector3::Cross(up, k));	
	i.normalize();

	Vector3 j(Vector3::Cross(k, i));
	j.normalize();


	float mat4[4][4] = {
		{i[0],i[1],i[2],-i * pos},
		{j[0],j[1],j[2],-j * pos},
		{k[0],k[1],k[2],-k * pos},
		{0,0,0,1}
	};

	return Matrix4(mat4);
}

const int screenpart(256);	

Matrix4 CameraBase::createprojectmat4(void) {

	float mat[4][4] = {
		{-2 * near_zpos / (game::Game::screensize()[0] / screenpart),0,0,0},
		{0,2 * near_zpos / (game::Game::screensize()[1] / screenpart),0,0},
		{0,0,(far_zpos + near_zpos) / (far_zpos - near_zpos),-2 * far_zpos * near_zpos / (far_zpos - near_zpos)},
		{0,0,1,0}
	};
	
	return Matrix4(mat);
}

Matrix4 CameraBase::inverse_createviewmat4(void) {

	Vector3 k(target - pos);	
	k.normalize();

	Vector3 i(Vector3::Cross(up, k));
	i.normalize();

	Vector3 j(Vector3::Cross(k, i));
	j.normalize();		


	float mat4[4][4] = {
		{i[0],j[0],k[0],pos[0]},
		{i[1],j[1],k[1],pos[1]},
		{i[2],j[2],k[2],pos[2]},
		{0,0,0,1}
	};

	return Matrix4(mat4);
}

Matrix4 CameraBase::inverse_createprojectmat4(void) {

	float mat[4][4] = {
		{(game::Game::screensize()[0] / screenpart) / (-2 * near_zpos) ,0,0,0},
		{0,(game::Game::screensize()[1] / screenpart) / (2 * near_zpos),0,0},
		{0,0,0,1},
		{0,0,(far_zpos - near_zpos) / (-2 * far_zpos * near_zpos), (far_zpos + near_zpos) / (2 * far_zpos * near_zpos) }
	};

	return Matrix4(mat);
}

void CameraBase::FPS(void) {
	const GLMatrix& glmat(object::Object::Camera::coord(targetID));
	pos = glmat.pos;
	target = pos + glmat.unitary.Read_Yunit() * 2.0f;
	up = glmat.unitary.Read_Zunit();	
}

void CameraBase::Race(void) {	
	
	const float dt(game::Game::msframe / 1000.0f);
	
	const GLMatrix& glmat(object::Object::Camera::coord(targetID));
	const Vector3 ideal(glmat.pos - glmat.unitary.Read_Yunit() * dhorz + glmat.unitary.Read_Zunit() * dvert);
	pos += (pos - prev_pos) * (1 - dt * 2 * sqrt(spring)) - spring * (pos - ideal) * (dt * dt);

	target = pos + glmat.unitary.Read_Yunit() * dtarg;
	up = glmat.unitary.Read_Zunit();	

}	

void CameraBase::Orbit(void) {	

	const GLMatrix& glmat(object::Object::Camera::coord(targetID));
	float side(0), vert(0);		

	const game::controller::enumButState state =
		game::Game::Butstate(game::controller::PlayerNumber::P1, game::controller::But::Mouse_Right);

	if ((state == game::controller::enumButState::Pressed) ||
		(state == game::controller::enumButState::Held)) {	

		const Vector2 axis =
			game::Game::Axis(game::controller::PlayerNumber::P1, game::controller::Axis::Axis0);
		
		side = axis[0]; vert = axis[1];

	}

	const Vector3 offset(pos - target);	const float length(offset.length());
	target = glmat.pos;
	up = glmat.unitary.Read_Zunit();
	pos = target + cos(side) * offset + sin(side) * Vector3::Cross(up, offset).normalize() * length;
	const Vector3 offset2(pos - target);
	pos = target + cos(vert * 3) * offset2 + sin(vert * 3) *
		Vector3::Cross(offset2, Vector3::Cross(up, offset2)).normalize() * length;

}

/***************************************************************************
	satellite
****************************************************************************/

Matrix4 satellite::GameScript::Update(void) {
	return CameraBase::camera.Update();
}
void satellite::GameScript::SetCam_zrange(float near, float far) {
	CameraBase::camera.SetCam_zrange(near, far);
}
void satellite::GameScript::SetTarget(object::ObjectID ID) {
	CameraBase::camera.SetTarget(ID);
}
void satellite::GameScript::SetMode(camera::Mode mode) {
	CameraBase::camera.SetMode(mode);
}
void satellite::GameScript::BackProjection(const Vector2& glcoord, Vector3& start, Vector3& direction) {
	CameraBase::camera.BackProjection(glcoord, start, direction);
}