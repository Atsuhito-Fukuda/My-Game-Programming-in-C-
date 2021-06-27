#include <SDL.h>	
#include <SDL_image.h>
#include "Game.h"
#include "Error.h"
#include "Gpu.h"
#include "Action.h"
#include "Audio.h"
#include "Data.h"
#include "GameScript.h"
#include "Vector.h"
#include "Object.h"

/*
	[ namespace game ]

	�E�����ł̓A�v���̑S�̗̂����A�A�v���O���Ƃ̓�(�o)�͂������B(�O�����C�u����SDL��p����B)
	�EGameBase�N���X�̓A�v���S�̗̂��������static�֐���p�ӁB

	�Egame::controller�͓��͏����������B
	�EGameScript�̓Q�[�����L�q����class�B(object::Object�Ɠ����\��H)

*/

namespace game {
	class GameBase;
	class GameScript;
}
struct SDL_Window;

class game::GameBase {

	static void Boot(GameScript& gamescript);		
	static void ShutDown(void);	
	static void WaittillNextFrame(void);

	static SDL_Window* window;		
	static void* sdlcontext;
	static unsigned int tickscount;

	static void Initialize(void);

	static const std::vector<std::vector<unsigned int>> screensizetable;
	static void screensizeID(screenIDtable ID);
	static void frameID(frameIDtable ID);

public:

	static void Run(GameScript& gamescript);
	static unsigned int msframe;
	static const std::vector<unsigned int>* screensize;	

};

using namespace game;

/***************************************************************************

	GameBase

****************************************************************************/

/*

	�E�A�v���̃��C���̗���������B(�N���E�X�V�E�I��)

		�E�N�����Ɋestatic�N���X��Initialize�������ōs���B

		�E�X�V���ɂ͊estatic�N���X��Update���s���B

		�E�I�����estatic�N���X��ShutDown���s���B


*/

SDL_Window* GameBase::window(nullptr);	
SDL_GLContext GameBase::sdlcontext(nullptr);
unsigned int GameBase::tickscount(0);

void GameBase::Boot(GameScript& gamescript) {
	// Debug
	error::Error::Debug_RecordPlace(error::Error::Boot);

	// Initialize SDL
	error::Game::SDLErrorCheck(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) == 0);

	// Initialize SDL Image
	error::Game::SDLErrorCheck(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG);

	// Set Attribute SDL Open GL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	GameBase::Initialize();

	// Create SDL Window
	window = SDL_CreateWindow("Game Programming in C++", 100, 20,
		(*screensize)[0], (*screensize)[1], SDL_WINDOW_OPENGL);
	error::Game::SDLErrorCheck(window != nullptr);

	// Create SDL GL Contex
	error::Game::SDLErrorCheck((sdlcontext = SDL_GL_CreateContext(window)) != NULL);

	// Initialize
	gpu::Gpu::Initialize();
	audio::Audio::Initialize();
	GameScript::GameScript_s_Game::Initialize(gamescript); 
	Game::ControllerInitialize();
	action::Action::Initialize();
	object::data::Data::Initialize();
	object::Object::Initialize(); 

}

void GameBase::Run(GameScript& gamescript) {
	// Debug
	error::Error::Debug_RecordPlace(error::Error::Run);

	Boot(gamescript);

	while (true) {

		WaittillNextFrame();

		SDL_GL_SwapWindow(window);
		gpu::Gpu::Update();
		error::Error::Debug_RecordPlace(error::Error::Input);		
		Game::ControllerUpdate();
		GameScript::GameScript_s_Game::Update(); 
		audio::Audio::Update();

	}

}

void GameBase::ShutDown(void) {
	// Debug
	error::Error::Debug_RecordPlace(error::Error::ShutDown);

	audio::Audio::Shutdown();

	SDL_GL_DeleteContext(sdlcontext);
	SDL_DestroyWindow(window);		
	SDL_Quit();

	exit(EXIT_SUCCESS);		

}

void GameBase::WaittillNextFrame(void) {	
	// Debug
	error::Error::Debug_RecordPlace(error::Error::WaittillNextFrame);

	unsigned int temp;
	while (!SDL_TICKS_PASSED((temp = SDL_GetTicks()), tickscount + msframe))
		;
	
	tickscount = temp;

}

void GameBase::Initialize(void) {

	for (const auto& screensize : screensizetable) {
		if (screensize.size() != 2) { error::Game::ScreensizetableError(); break; }
	}

}

unsigned int GameBase::msframe(frameIDtable::ms16);
const std::vector<std::vector<unsigned int>> GameBase::screensizetable({ {1024,768} });
const std::vector<unsigned int>* GameBase::screensize(&screensizetable[0]);

void GameBase::screensizeID(screenIDtable ID) {
	screensize = &screensizetable[ID];
	// SDL�̕����A����
}
void GameBase::frameID(frameIDtable ID) { msframe = ID; }

/***************************************************************************

	satellite

****************************************************************************/

const unsigned int& satellite::Public::msframe(GameBase::msframe);
const std::vector<unsigned int>& satellite::Public::screensize(void) { return *GameBase::screensize; }

void satellite::Public::Run(GameScript& script) { //main�����B��x����B

	static bool first(true); //�Վ���
	if (!first) return;
	first = false;

	GameBase::Run(script);

}