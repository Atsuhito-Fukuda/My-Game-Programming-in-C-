#pragma once
#include "GameScript.h"

class testScript : public game::GameScript{

	void Load(void) override;
	void Progress(void) override;

public://->main
	testScript(void);

};