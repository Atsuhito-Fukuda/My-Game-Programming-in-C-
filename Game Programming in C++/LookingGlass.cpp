#include "LookingGlass.h"
#include <SDL.h>	

using namespace lookingglass;


/*

	[ namespace librarian::lookingglass ]

	�E�eclass�̃f�o�b�O���s�����߂ɂƍl���ėp�ӂ����B

	�E���̂Ƃ���A�v���E�B���h�E�O�ł̃}�E�X�����on/off�؂�ւ��݂̂ɗp���Ă���B

*/

int* Controller::wheel[2]{};
unsigned int(*Controller::sdlmouse)(int*, int*)(SDL_GetRelativeMouseState);
unsigned int Controller::mutemouse(int* x, int* y) {
	*y = *x = 0;
	*(wheel[1]) = *(wheel[0]) = 0;
	return 0;
}
void Controller::SwitchWindowMode(void) {

	static unsigned int ticks(0);	unsigned int temp(SDL_GetTicks());	
	if (!SDL_TICKS_PASSED((temp = SDL_GetTicks()), ticks + 500))	
		return;

	ticks = temp;

	static int toggle(0);

	switch ((toggle ^= 1)) {

	case 0:
		SDL_SetRelativeMouseMode(SDL_TRUE);
		sdlmouse = SDL_GetRelativeMouseState;
		break;
	case 1:
		SDL_SetRelativeMouseMode(SDL_FALSE);	
		SDL_ShowCursor(SDL_TRUE);
		sdlmouse = mutemouse;
		break;
	default:
		break;
	}


}
