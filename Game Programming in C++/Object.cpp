#include "Object.h"
#include "Error.h"
#include <unordered_map>
#include <string>
#include "IDmap.h"	
#include "Game.h"
#include "ObjectCore.h"
#include "Action.h"
#include "Audio.h"
#include "Gpu.h"
#include "Data.h"

/*
	[ namespace object ]

	・ここでは各objを扱う。

		・Action,Audio,GpuとObject(アクター操作)の各情報をObjectBase(非static)クラスに束ねる。

	・ObjectクラスはObjectBaseの参照を持つが外部には宣言不明。(pimplイディオム)

	・ObjectBaseクラスは各objのを扱うstatic関数を用意。

*/

namespace object {
	class ObjectBase;
}
class object::ObjectBase : virtual public ObjectCore,
	public gpu::Gpu, public audio::Audio, public action::Action {

	friend class Board<ObjectBase>;

public:

	static ObjectID MakeObject(const std::string& loaddir, std::vector<unsigned int>& table);
	static const GLMatrix& coord(ObjectID ID);
	static void Update(const std::vector<unsigned int>& table, const Matrix4& viewprojmat4);

	static idmap::IDgenerateMap<ObjectBase> IDmap;
	
private:

	static void Act(const std::vector<unsigned int>& table);
	static void Collision(void);

	static ObjectID* tempid;

// obj --------------------------------

	ObjectBase(
		const std::string& vertexfile, const std::string& vertindexfile,
		const std::string& vertshaderfile, const std::string& fragshaderfile,
		const std::string& texturefile, float specpower,
		const std::string& collisiontype, const std::vector<Vector3>& collidepoints,
		const std::string& manipulation
	);

	~ObjectBase(void);

};

using namespace object;

/***************************************************************************

	ObjectBase

****************************************************************************/

/*

	・ObjectBaseはobjをidmap::IDgenerateMapで配列に収納して保持している。
	
	・それらの生成・削除・更新(衝突処理・描画)はここで行っている。

*/

ObjectID* ObjectBase::tempid(0);

idmap::IDgenerateMap<ObjectBase> ObjectBase::IDmap("ObjectIDmap", 20, 50);

ObjectBase::ObjectBase(
	const std::string& vertexfile, const std::string& vertindexfile,
	const std::string& vertshaderfile, const std::string& fragshaderfile,
	const std::string& texturefile, float specpower,
	const std::string& collisiontype, const std::vector<Vector3>& collidepoints,
	const std::string& manipulation
) : 
	gpu::Gpu(vertshaderfile,fragshaderfile,vertexfile,vertindexfile,texturefile,specpower),
	audio::Audio(),
	action::Action(collisiontype, collidepoints),
	ObjectCore(*(tempid = new ObjectID), *(new GLMatrix), data::Data::MakeFunc(manipulation)(*this))

{}

ObjectBase::~ObjectBase(void) {

	delete &id; delete &mat; delete &object;

}

ObjectID ObjectBase::MakeObject(const std::string& loaddir, std::vector<unsigned int>& table) {

	std::string vertexfile, vertindexfile, vertshaderfile, fragshaderfile, texturefile, manipulation;
	float specpower; 
	std::string collisiontype;
	std::vector<Vector3> collidepoints;

	data::Data::LoadDatatext(
		loaddir,
		vertexfile, vertindexfile, vertshaderfile, fragshaderfile, texturefile,
		specpower, collisiontype, collidepoints, manipulation
	);

	ObjectBase* obj = new ObjectBase(
		vertexfile, vertindexfile, vertshaderfile, fragshaderfile, texturefile,
		specpower, collisiontype, collidepoints, manipulation
	);

	*tempid = IDmap.Register(obj, table); 

	return obj->id;		

}

const GLMatrix& ObjectBase::coord(ObjectID ID) {

	ObjectBase* obj(IDmap.Read(ID));
	if (obj) return obj->mat;
	else {
		error::Object::ReadNullptr();		
		return *(new GLMatrix);
	}
}

void ObjectBase::Update(const std::vector<unsigned int>& table, const Matrix4& viewprojmat4) {
	
	Act(table); 

	for (auto boardnum : table) {

		const Board<ObjectBase>& board(IDmap.ReadBoard(boardnum));

		for (unsigned int i(0); i < board.getlength(); ++i) {
			if (board[i]) {
				board[i]->Audio::update();
				board[i]->Gpu::Draw(viewprojmat4);	
			} 
		}

	}

}

void ObjectBase::Act(const std::vector<unsigned int>& table) {

	for (auto boardnum : table) {

		const Board<ObjectBase>& board(IDmap.ReadBoard(boardnum));

		for (unsigned int i(0); i < board.getlength(); ++i) {
			if (board[i]) board[i]->Action::FirstAct();
		}

	}

	Collision();

}

void ObjectBase::Collision(void) {

	if (!Action::actiontree.Update()) return;	

	std::vector<const std::vector<ObjectID>*> list;
	list.reserve(9); 
	std::unordered_map<ObjectBase*, bool> secondprocess;	

	do {

		const std::vector<ObjectID>& mainnode(Action::actiontree.present());	

		do { if (Action::actiontree.present().size()) list.emplace_back(&Action::actiontree.present()); }
		while (Action::actiontree.NextParent());

		for (unsigned int i(0); i < mainnode.size(); ++i) {

			unsigned int j(i + 1);	bool i_collision(false);	
			ObjectBase* obj1(IDmap.Read(mainnode[i]));

			if (obj1) 
				for (auto othernode : list) {

					for (; j < othernode->size(); ++j) {

						ObjectBase* obj2(IDmap.Read((*othernode)[j]));

						if (obj2 && action::Action::collision(*obj1, *obj2)) { 
							i_collision = true;
							secondprocess[obj2] = true;
						} 

					}
					j = 0;
				}

			if(i_collision) secondprocess[obj1] = true;

		}

		list.clear();	

	} while (Action::actiontree.NextNode());


// Second Process - - - - - - - - - - - - - 

	for (auto iter = secondprocess.begin(); iter != secondprocess.end(); ++iter) {
		iter->first->Action::SecondAct();	
	}

}

/***************************************************************************
	ObjectCore
****************************************************************************/

ObjectID iddummy(0); GLMatrix gldummy;

ObjectCore::ObjectCore(void) : ObjectCore(iddummy, gldummy, *(Object*)nullptr){ 
	error::Object::ObjectCoreDefaultConstructorUse();
} 

ObjectCore::ObjectCore(const ObjectID& id, GLMatrix& mat, Object& object) : 
	Mat(mat), id(id), mat(mat), object(object) {}

/***************************************************************************
	satellite
****************************************************************************/

const GLMatrix& satellite::Camera::coord(ObjectID ID) { return ObjectBase::coord(ID); }
ObjectID satellite::GameScript::MakeObject(const std::string& loaddir, std::vector<unsigned int>& table) {
	return ObjectBase::MakeObject(loaddir, table); 
}
unsigned int satellite::GameScript::Remove(ObjectID ID) { return ObjectBase::IDmap.Remove(ID);; }
void satellite::GameScript::Remove(std::vector<unsigned int>& table) { ObjectBase::IDmap.Remove(table); }
const GLMatrix& satellite::GameScript::coord(ObjectID ID) { return ObjectBase::coord(ID); }
void satellite::GameScript::Update(const std::vector<unsigned int>& table, const Matrix4& viewprojmat4) {
	ObjectBase::Update(table, viewprojmat4);
}
void satellite::Game::Initialize(void) {

	std::vector<unsigned int> dummytable;
	ObjectBase::MakeObject("Assets/3D/invisible", dummytable); 

}

/***************************************************************************

	Object

****************************************************************************/

Object::Object(ObjectBase& objbase) : objbase(objbase) {}
Object::~Object(void){}

void Object::print(const std::string& string) { std::cout << string; }
Vector3& Object::Pos(void) { return objbase.Mat.pos; }
Matrix3& Object::Unitary(void) { return objbase.Mat.unitary; }

void Object::PlaySound(const std::string& eventname, audio::enumInstanceInfo number) {
	objbase.Audio::PlaySound(eventname, number);
}

bool Object::IsNone(game::controller::PlayerNumber n, game::controller::But but) {
	bool none(false);
	if (game::Game::Butstate(n, but) == game::controller::enumButState::None) none = true;
	return none;
}
bool Object::IsPressed(game::controller::PlayerNumber n, game::controller::But but) {
	bool pressed(false);
	if (game::Game::Butstate(n, but) == game::controller::enumButState::Pressed) pressed = true;
	return pressed;
}
bool Object::IsReleased(game::controller::PlayerNumber n, game::controller::But but) {
	bool released(false);
	if (game::Game::Butstate(n, but) == game::controller::enumButState::Released) released = true;
	return released;
}
bool Object::IsHeld(game::controller::PlayerNumber n, game::controller::But but) {
	bool held(false);
	if (game::Game::Butstate(n, but) == game::controller::enumButState::Held) held = true;
	return held;
}
const Vector2& Object::axis(game::controller::PlayerNumber n, game::controller::Axis axis) {
	return game::Game::Axis(n, axis);
}