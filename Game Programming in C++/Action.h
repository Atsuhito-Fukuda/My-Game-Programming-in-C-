#pragma once
#include "ObjectCore.h"
#include <vector>
#include "BinaryTree.h"
#include "Vector.h"
#include <string>

namespace action {
	class Action;
	class Shape;
}
namespace game {
	class GameBase;
}
namespace object {
	using ObjectID = unsigned int;
	class Object;
	class ObjectBase;
}

/***************************************************************************
  satellite
****************************************************************************/

namespace action{
	namespace satellite {
		class Game;
	}
}
class action::satellite::Game {

	friend class game::GameBase;

	static void Initialize(void);

};

/***************************************************************************

	Action 

****************************************************************************/

class action::Action :virtual public object::ObjectCore,
	public satellite::Game { 

	friend class object::Object;
	friend class object::ObjectBase;

	Shape& shape; 

	void FirstAct(void);
	void SecondAct(void);

	Action& operator =(const Action& action);//ÉRÉsÅ[ã÷é~
	Action(const Action& action);

	Action(const std::string& collisiontype, const std::vector<Vector3>& collidepoints);
	~Action(void);

	static bool collision(Action& action0, Action& action1);	
	static BinaryTree<object::ObjectID>& actiontree;

};