#include <SDL.h>
#include "Game.h"
#include "Error.h"
#include "LookingGlass.h"

/*
	[ namespace game::controller ]

	・ここでは入力処理を扱う。(外部ライブラリSDLを用いる。)
	・ControllerBaseクラスは入力を扱うstatic関数を用意。

*/


namespace game {
	namespace controller {
		class ControllerBase;
		class ControlStatus;
		enum class ControlMode : int { 
			NoControl, KeyMouse, GamePad	
		};
		enum AxisRange : int {
			AXISRANGE_DEAD = 4000, AXISRANGE_MAX = 30000
		};
		enum Max : unsigned int { 
			PlayerNumberMax = PlayerNumber::P2 + 1,
			ButMax = But::But14 + 1,
			AxisMax = Axis::Axis2 + 1
		};
	}
}

class game::controller::ControllerBase {

	static const unsigned char* keyboardstate;

	static void InputButState(PlayerNumber player, bool input, But but);		

	static void JoystickPlugCheck(void);
	static void AllocGamePad_test(PlayerNumber num);
	static void CloseGamePad_test(PlayerNumber num);

public:

	static void ProcessInput(void);
	static ARRAY<ControlStatus> controlmap; // ※ 抜き差ししてもデータのeraseはせずそのままとする

};

using namespace game;
using namespace game::controller;

/***************************************************************************

	ControlStatus

****************************************************************************/

/*

	・各コントローラのボタンやaxisの入力状態を保持するクラス。

*/

class game::controller::ControlStatus {	

	friend ControllerBase;
	friend class satellite::Object;
	friend class satellite::selfy::Controller;
	friend class ARRAY<ControlStatus>;

	_SDL_GameController* gpadptr;	
	ControlMode mode;
	//Vector2 preaxes[3];	
	ControlStatus(void);

	ARRAY<enumButState> buttons; 
	ARRAY<Vector2> axes;

};

ControlStatus::ControlStatus(void) : mode(ControlMode::NoControl), buttons(Max::ButMax), axes(Max::AxisMax), gpadptr()//, preaxes{}
{}

/***************************************************************************

	ControllerBase

****************************************************************************/

/*

	・コントローラの処理を扱うクラス。

	・SDLライブラリを通して外部機器からの入力を扱う。

	・マウス・キー入力(一部)に対応。

	・外部(object::Object)に入力情報を提供する。

		・None,Pressed,Released,Heldとボタン状態は用意されていて、ControlBaseで各ボタンこれらを更新する


*/

const Uint8* ControllerBase::keyboardstate(SDL_GetKeyboardState(NULL));
ARRAY<ControlStatus> ControllerBase::controlmap(Max::PlayerNumberMax);

void ControllerBase::ProcessInput(void) {

// Get SDL Event ---------------------------

	SDL_Event event;
	int iwheel_x(0), iwheel_y(0);
	lookingglass::Controller::wheel[0] = &iwheel_x;
	lookingglass::Controller::wheel[1] = &iwheel_y;

	while (SDL_PollEvent(&event)) {	

		switch (event.type) {

		case SDL_QUIT:
			exit(EXIT_SUCCESS);
			//ShutDown();
			break;

		case SDL_MOUSEWHEEL:
			iwheel_x = event.wheel.x;
			iwheel_y = event.wheel.y;
			break;

		default:
			break;
		}

	}


	// Input Controller Signals ----------------------

	for (unsigned int i(0); i < Max::PlayerNumberMax; ++i) {

		float coord0_x(0), coord0_y(0), coord1_x(0), coord1_y(0), coord2_x(0), coord2_y(0);	
		bool tempbut[Max::ButMax] = {};

		// Key Mouse Input ---------------------------------------------------

		if (controlmap[i].mode == ControlMode::KeyMouse) {

			// Keyboard Manipulate ----------------------

			if (keyboardstate[SDL_SCANCODE_ESCAPE])
				exit(EXIT_SUCCESS);
				//ShutDown();	

			if (keyboardstate[SDL_SCANCODE_TAB]) {
				lookingglass::Controller::SwitchWindowMode();
			}

			tempbut[But::Key_Space] |= (bool)keyboardstate[SDL_SCANCODE_SPACE];
			tempbut[But::Key_Return] |= (bool)keyboardstate[SDL_SCANCODE_RETURN];
			tempbut[But::Key_Left] |= (bool)keyboardstate[SDL_SCANCODE_LEFT];
			tempbut[But::Key_Down] |= (bool)keyboardstate[SDL_SCANCODE_DOWN];
			tempbut[But::Key_Right] |= (bool)keyboardstate[SDL_SCANCODE_RIGHT];
			tempbut[But::Key_Up] |= (bool)keyboardstate[SDL_SCANCODE_UP];
			tempbut[But::Key_W] |= (bool)keyboardstate[SDL_SCANCODE_W];
			tempbut[But::Key_A] |= (bool)keyboardstate[SDL_SCANCODE_A];
			tempbut[But::Key_S] |= (bool)keyboardstate[SDL_SCANCODE_S];
			tempbut[But::Key_D] |= (bool)keyboardstate[SDL_SCANCODE_D];


			// Mouse Manipulate ------------------------

			int imouse_x(0), imouse_y(0);
			Uint32 mousebut = lookingglass::Controller::sdlmouse(&imouse_x, &imouse_y);	
			coord1_x = (float)iwheel_x; coord1_y = (float)iwheel_y;		

			coord0_x = (float)imouse_x / (Game::screensize())[0];
			coord0_y = -(float)imouse_y / (Game::screensize())[1];

			tempbut[But::Mouse_Left] |= (bool)(mousebut & SDL_BUTTON(SDL_BUTTON_LEFT));
			tempbut[But::Mouse_Right] |= (bool)(mousebut & SDL_BUTTON(SDL_BUTTON_RIGHT));
			tempbut[But::Mouse_Middle] |= (bool)(mousebut & SDL_BUTTON(SDL_BUTTON_MIDDLE));

		}
		else

			// Game Pad Input ---------------------------------------------------

			if (controlmap[i].mode == ControlMode::GamePad) {

				SDL_GameController* gpadptr = controlmap[i].gpadptr;

				if (gpadptr == nullptr)	
					error::Controller::nosdlgpadpointer();

				else {

					for (unsigned int i(0); i < SDL_CONTROLLER_BUTTON_MAX; ++i) {	
						tempbut[(But)i] = SDL_GameControllerGetButton(gpadptr, SDL_GameControllerButton(i));
					}

					float axis[SDL_CONTROLLER_AXIS_MAX]{};		
					for (unsigned int i(0); i < SDL_CONTROLLER_AXIS_MAX; ++i) {

						int iaxis = SDL_GameControllerGetAxis(gpadptr, SDL_GameControllerAxis(i));

						if (abs(iaxis) < AxisRange::AXISRANGE_DEAD)	
							axis[i] = 0.0f;
						else if (abs(iaxis) > AxisRange::AXISRANGE_MAX)
							axis[i] = iaxis >= 0 ? 1.0f : -1.0f;
						else
							axis[i] = iaxis >= 0 ? ((float)iaxis - AxisRange::AXISRANGE_DEAD) / (AxisRange::AXISRANGE_MAX - AxisRange::AXISRANGE_DEAD) :
							((float)iaxis + AxisRange::AXISRANGE_DEAD) / (AxisRange::AXISRANGE_MAX - AxisRange::AXISRANGE_DEAD);

					}

					coord0_x = axis[SDL_CONTROLLER_AXIS_LEFTX];
					coord0_y = -axis[SDL_CONTROLLER_AXIS_LEFTY];	
					coord1_x = axis[SDL_CONTROLLER_AXIS_RIGHTX];
					coord1_y = -axis[SDL_CONTROLLER_AXIS_RIGHTY];
					coord2_x = axis[SDL_CONTROLLER_AXIS_TRIGGERLEFT];
					coord2_y = axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT];
				
				}
			}
			else
				// No Player (ControlMode::None) --------------------------------
			{
				// 一応いれることに
				goto END;
			}

		controlmap[i].axes[Axis::Axis0] = Vector2(coord0_x, coord0_y);
		controlmap[i].axes[Axis::Axis1] = Vector2(coord1_x, coord1_y);
		controlmap[i].axes[Axis::Axis2] = Vector2(coord2_x, coord2_y);

		for (int n(0); n < Max::ButMax; ++n) {
			InputButState((PlayerNumber)i, tempbut[n], (But)n);
		}

	}


END:

	return;


}

void ControllerBase::JoystickPlugCheck(void) { //はりぼて

	for (int i(0); i < SDL_NumJoysticks(); ++i) {

		if (SDL_IsGameController(i)) {

			SDL_GameControllerOpen(i);

		}

	}

}

void ControllerBase::AllocGamePad_test(PlayerNumber num) {	 //はりぼて

	controlmap[num].mode = ControlMode::GamePad;
	controlmap[num].gpadptr = SDL_GameControllerOpen(0);	

}	
void ControllerBase::CloseGamePad_test(PlayerNumber num) { //はりぼて

	controlmap[num].mode = ControlMode::NoControl;

	SDL_GameControllerClose(controlmap[num].gpadptr);
	controlmap[num].gpadptr = nullptr;		

}

void ControllerBase::InputButState(PlayerNumber player, bool input, But but) {

	switch (controlmap[player].buttons[but]) {	

	case enumButState::None: {
		if (input) controlmap[player].buttons[but] = enumButState::Pressed;
		break;
	}
	case enumButState::Pressed: {
		if (input) controlmap[player].buttons[but] = enumButState::Held;
		else controlmap[player].buttons[but] = enumButState::Released;
		break;
	}
	case enumButState::Released: {
		if (input) controlmap[player].buttons[but] = enumButState::Pressed;
		else controlmap[player].buttons[but] = enumButState::None;
		break;
	}
	case enumButState::Held: {
		if (!input) controlmap[player].buttons[but] = enumButState::Released;
		break;
	}
	default:
		break;

	}

}

/***************************************************************************
	satellite
****************************************************************************/

enumButState satellite::Object::Butstate(game::controller::PlayerNumber n, game::controller::But but) {
	return ControllerBase::controlmap[n].buttons[but];
}
const Vector2& satellite::Object::Axis(game::controller::PlayerNumber n, game::controller::Axis axis) {
	return ControllerBase::controlmap[n].axes[axis];
}

void satellite::selfy::Controller::ControllerInitialize(void) {

	if (Max::ButMax < SDL_CONTROLLER_BUTTON_MAX)
		error::Controller::ButtonsShortage();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	ControllerBase::controlmap[PlayerNumber::P1].mode = ControlMode::KeyMouse;	

}
void satellite::selfy::Controller::ControllerUpdate(void) { ControllerBase::ProcessInput(); }