#pragma once
#include "Vector.h"

namespace object {
	using ObjectID = unsigned int;
	class Object;
	class ObjectCore;
	class ObjectBase;
}
namespace action {
	class Action;
	class ActionBase;
}

class object::ObjectCore {

	friend class Object;
	friend class ObjectBase; 
	friend class action::Action; 
	friend class action::ActionBase;

	Object& object; 
	GLMatrix& Mat; 

protected:

	const ObjectID& id;
	const GLMatrix& mat;

	ObjectCore(const ObjectID& id, GLMatrix& mat, Object& object);
	ObjectCore(void); 

};