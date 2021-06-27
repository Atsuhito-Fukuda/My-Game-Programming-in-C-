#pragma once
#include "Vector.h"
#include <vector>
#include <string>

namespace object {
	class Object;
	class ObjectBase;
	using ObjectID = unsigned int; 
}
namespace action {
	class Action;
}
namespace audio {
	enum enumInstanceInfo : unsigned int;
	enum class Playmode : int;	
	enum class Busmode : int;
}
namespace camera {
	class CameraBase;
}
namespace game {
	class GameBase;
	class GameScript;
	namespace controller { 
		enum PlayerNumber : unsigned int;
		enum But : unsigned int;
		enum Axis : unsigned int;
	}
}

/***************************************************************************
	satellite
****************************************************************************/

namespace object {
	namespace satellite {
		class Camera;
		class GameScript;
		class Game;
	}
}
class object::satellite::Camera {

	friend class camera::CameraBase;

	static const GLMatrix& coord(object::ObjectID ID);

};
class object::satellite::GameScript {

	friend class game::GameScript;

	static object::ObjectID MakeObject(const std::string& loaddir, std::vector<unsigned int>& table);
	static unsigned int Remove(object::ObjectID ID);
	static void Remove(std::vector<unsigned int>& table);
	static const GLMatrix& coord(object::ObjectID ID);
	static void Update(const std::vector<unsigned int>& table, const Matrix4& viewprojmat4);

};

class object::satellite::Game {

	friend class game::GameBase;

	static void Initialize(void);


};
/***************************************************************************
	
	Object

****************************************************************************/

class object::Object : 
	public satellite::Camera, public satellite::GameScript,
	public satellite::Game {

protected:

	void print(const std::string& string);
	void PlaySound(const std::string& eventname, audio::enumInstanceInfo number);
	void SoundManage(audio::enumInstanceInfo num, audio::Playmode mode, float var = 0.0f, const std::string& name = "");
	void BusManage(audio::enumInstanceInfo num, const std::string& busname, audio::Busmode mode, float var = 0.0f);
	Vector3& Pos(void);
	Matrix3& Unitary(void); 

	bool IsNone(game::controller::PlayerNumber n, game::controller::But but);
	bool IsPressed(game::controller::PlayerNumber n, game::controller::But but);
	bool IsReleased(game::controller::PlayerNumber n, game::controller::But but);
	bool IsHeld(game::controller::PlayerNumber n, game::controller::But but);
	const Vector2& axis(game::controller::PlayerNumber n, game::controller::Axis axis);

	Object(ObjectBase& objbase);	

public:

	ObjectBase& objbase;
	Object(void); 

	virtual ~Object(void);

	virtual void FirstAct(void) = 0;
	virtual void SecondAct(void) = 0;

};