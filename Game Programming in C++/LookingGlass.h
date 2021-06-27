#pragma once

namespace librarian {
	namespace lookingglass {
		class Controller;
	}
}

namespace game {
	namespace controller {
		class ControllerBase;
	}
}

using namespace librarian;

class lookingglass::Controller {

	friend class game::controller::ControllerBase;

	static int* wheel[2];
	static unsigned int mutemouse(int* x, int* y);
	static unsigned int(*sdlmouse)(int*, int*);
	static void SwitchWindowMode(void);

};


