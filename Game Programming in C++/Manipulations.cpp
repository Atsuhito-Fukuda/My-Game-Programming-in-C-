#include "Object.h"
#include "Game.h" 
#include "Audio.h"
#include "Data.h"

/*

	ÅEäeä÷êîÇÕDataBaseÇ…ìnÇ≥ÇÍÇΩÉnÉbÉVÉÖmapÇ…ñºëO(string)Ç∆ê∂ê¨ä÷êîÇÃptrÇÇ±Ç±Ç≈ìoò^Ç∑ÇÈÅB

*/


namespace object {
	namespace data {
		class DataBase;
		class ManipulationsBase;
	}
}
class object::data::ManipulationsBase {

public:

	static void Register(const std::string& name, object::Object& (*func)(ObjectBase&));
	template<typename... Rest>
	static void Register(const std::string& name, object::Object& (*func)(ObjectBase&), const Rest&... rest);

	static std::unordered_map<std::string, object::Object& (*)(object::ObjectBase&)>* manipulationsmap;


	template <typename T> 
	static object::Object& Make(object::ObjectBase& objbase) { return *new T(objbase); }

};

using namespace object::data;


std::unordered_map<std::string, object::Object& (*)(object::ObjectBase&)>* ManipulationsBase::manipulationsmap(nullptr);

void ManipulationsBase::Register(const std::string& name, object::Object& (*func)(ObjectBase&)) {

	auto iter = manipulationsmap->find(name); 
	if (iter == manipulationsmap->end()) manipulationsmap->emplace(name, func);
	else error::Manipulations::MultipleMovementFuncName(name);

}

template<typename... Rest>
void ManipulationsBase::Register(const std::string& name, object::Object& (*func)(ObjectBase&), const Rest&... rest) {

	Register(name, func);
	Register(rest...);

}

/***************************************************************************
	
	Object åpè≥'s

****************************************************************************/

/***************************************************************************
	null
****************************************************************************/
class null : public object::Object {

	void FirstAct(void){}
	void SecondAct(void) {}

public:
	null(object::ObjectBase& objbase) : Object(objbase) {}

};

/***************************************************************************
	test1
****************************************************************************/
class test1 : public object::Object {

	void FirstAct(void);
	void SecondAct(void){}

public:
	test1(object::ObjectBase& objbase) : Object(objbase) {}

};
void test1::FirstAct(void) {

	if (IsPressed(game::controller::P1,game::controller::But0))
		PlaySound("event:/Explosion2D", audio::mainvoice0);

	if (IsPressed(game::controller::P1, game::controller::Key_Return))
		PlaySound("event:/Music", audio::mainvoice1);

	int side(0);	
	if (IsPressed(game::controller::P1, game::controller::Key_Right) ||
		IsHeld(game::controller::P1, game::controller::Key_Right))
		++side;
	if (IsPressed(game::controller::P1, game::controller::Key_Left) ||
		IsHeld(game::controller::P1, game::controller::Key_Left))
		--side;

	int vert(0);
	if (IsPressed(game::controller::P1, game::controller::Key_Up) ||
		IsHeld(game::controller::P1, game::controller::Key_Up))
		++vert;
	if (IsPressed(game::controller::P1, game::controller::Key_Down) ||
		IsHeld(game::controller::P1, game::controller::Key_Down))
		--vert;

	Pos() += Vector3(0.5f * side, 10.0f * vert, 0);

}
/***************************************************************************
	test_globerotate
****************************************************************************/
class test_globerotate : public object::Object {

	void FirstAct(void) { Unitary().zunitrotate(0.005f); }
	void SecondAct(void) { print("collision detected.\n"); }

public:
	test_globerotate(object::ObjectBase& objbase) : Object(objbase) {}

};
/***************************************************************************
	collisiondetected
****************************************************************************/
class collisiondetected : public object::Object {

	void FirstAct(void) {}
	void SecondAct(void) { print("collision detected.\n"); }

public:
	collisiondetected(object::ObjectBase& objbase) : Object(objbase) {}

};

/***************************************************************************
	satellite 
****************************************************************************/

void satellite::selfy::Manipulations::ManipulationsInitialize(std::unordered_map<std::string, object::Object& (*)(object::ObjectBase&)>& manipulationsmap) {

	ManipulationsBase::manipulationsmap = &manipulationsmap;

	ManipulationsBase::Register(
		"null", ManipulationsBase::Make<null>,
		"test1", ManipulationsBase::Make<test1>,
		"test_globerotate", ManipulationsBase::Make<test_globerotate>,
		"collisiondetected", ManipulationsBase::Make<collisiondetected>
	);

	ManipulationsBase::manipulationsmap = nullptr; 

}