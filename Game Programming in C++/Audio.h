#pragma once
#include <vector>
#include "Vector.h"
#include <string>
#include "ObjectCore.h"

namespace audio {
	class Audio;
	class InstanceManager;
	enum class Playmode : int { //�T�E���h�̍Đ�mode
		Play, Pause, Restart, Stop, FadeoutStop, Volume, Pitch, setParByName
	};
	enum class Busmode : int { //Bus�̍Đ�mode
		BusPause, BusVolume
	};
	enum enumInstanceInfo : unsigned int { //�eobj�̊Ǘ�����T�E���h�̔z��(InstanceManager)�̗v�f�ԍ�
		mainvoice0, mainvoice1
	};
}
namespace game {
	class GameBase;
}
namespace camera {
	class CameraBase;
}
namespace object {
	class Object;
	class ObjectBase;
}

/***************************************************************************
	satellite
****************************************************************************/

namespace audio {
	namespace satellite {
		class Camera;
		class Game;
	}
}
class audio::satellite::Camera {

	friend class camera::CameraBase;

	static void Listener(const Vector3& pos, const Vector3& up, const Vector3& foward);

};
class audio::satellite::Game {

	friend class game::GameBase;

	static void Initialize(void);
	static void Update(void);
	static void Shutdown(void);

};

/***************************************************************************
	
	Audio

****************************************************************************/

class audio::Audio : virtual public object::ObjectCore,
	public satellite::Camera, public satellite::Game {

	friend class object::Object;
	friend class object::ObjectBase;

	InstanceManager& manager;

	void update(void);
	void PlaySound(const std::string& eventname, audio::enumInstanceInfo number);
	
	Audio& operator =(const Audio& audio); //�R�s�[�֎~
	Audio(const Audio& audio);

	Audio(void);
	~Audio(void);

};