#pragma once
#include <vector>
#include "Vector.h"
#include "ARRAY.h"

struct _SDL_GameController;
namespace object {
	class Object;
}
namespace camera {
	class CameraBase;
}
namespace game {
	class Game;
	class GameBase;
	enum frameIDtable : unsigned int {
		ms16 = 16, ms33 = 33
	};
	enum screenIDtable : int {
		size1024x768
	};

	namespace controller { //コントローラ情報に使用
		enum PlayerNumber : unsigned int {	
			P1, P2
		};
		enum class enumButState : int {
			None, Pressed, Released, Held
		};
		enum But : unsigned int {

			But0, But1, But2, But3, But4, But5, But6, But7,
			But8, But9, But10, But11, But12, But13, But14,

			Key_Space = But0, Key_Return = But1, Key_Left = But13, Key_Down = But12, Key_Right = But14, Key_Up = But11,
			Key_W = But11, Key_A = But13, Key_S = But12, Key_D = But14,
			Mouse_Left = But0, Mouse_Right = But1, Mouse_Middle = But2,

			Pad_A = But0, Pad_B = But1, Pad_X = But2, Pad_Y = But3, Pad_Back = But4, Pad_Guide = But5, Pad_Start = But6,
			Pad_Push_Left = But7, Pad_Push_Right = But8, Pad_Shoulder_Left = But9, Pad_Shoulder_Right = But10,
			Pad_DPad_Up = But11, Pad_DPad_Down = But12, Pad_DPad_Left = But13, Pad_DPad_Right = But14

		};	
		enum Axis : unsigned int {

			Axis0, Axis1, Axis2,
			Left = Axis0, Right = Axis1, Trigger = Axis2,
			Mousepos = Axis0, Mousewheel = Axis1

		};
	}
}

/***************************************************************************
	satellite
****************************************************************************/

namespace game {
	namespace satellite {
		class Public;
		class Object;
		namespace selfy {
			class Controller;
		}
	}
}
class game::satellite::Public {

public: //取り敢えず
	static const unsigned int& msframe;
	static const std::vector<unsigned int>& screensize(void); 

//- 臨時策で ----------------------
	static void Run(GameScript& gamescript); 

};
class game::satellite::Object {

	friend class object::Object;
	friend class camera::CameraBase; //一時的にか

	static game::controller::enumButState Butstate(game::controller::PlayerNumber n, game::controller::But but);
	static const Vector2& Axis(game::controller::PlayerNumber n, game::controller::Axis axis);

};
class game::satellite::selfy::Controller {

	friend class game::GameBase;

	static void ControllerInitialize(void);
	static void ControllerUpdate(void);

};

/***************************************************************************

	Game

****************************************************************************/

class game::Game : 
	public satellite::Public,
	public satellite::Object, public satellite::selfy::Controller
{};