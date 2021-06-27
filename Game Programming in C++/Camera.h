#pragma once
#include "Vector.h"

namespace camera {
	class Camera;
	enum Mode : unsigned int {
		FPS, Race, Orbit
	};
}
namespace game {
	class GameScript;
}
namespace object {
	using ObjectID = unsigned int;
}

/***************************************************************************
	satellite
****************************************************************************/

namespace camera {
	namespace satellite {
		class GameScript;
	}
}
class camera::satellite::GameScript {
	
	friend class game::GameScript;
	
	static Matrix4 Update(void);
	static void SetCam_zrange(float near, float far);
	static void SetTarget(object::ObjectID ID);
	static void SetMode(camera::Mode mode);
	static void BackProjection(const Vector2& glcoord, Vector3& start, Vector3& direction);

};

/***************************************************************************
	
	Camera

****************************************************************************/

class camera::Camera : public satellite::GameScript
{};