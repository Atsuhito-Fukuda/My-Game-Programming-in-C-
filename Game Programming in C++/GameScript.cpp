#pragma once
#include "GameScript.h"
#include "Tools.h"
#include "Camera.h"
#include "Error.h"
#include "Object.h"

using namespace game;

/***************************************************************************

	GameScript

****************************************************************************/

/*
 
	�E�e�Q�[��������L�q���悤�Ɨp�ӂ����N���X�B

	�E��������GameScript��static�N���X�ɓn�����Ƃł���GameScript�𑖂点��B

	�ELoad����Q�[���̍X�V���̊֐���virtual�p���ŋL�q���邱�Ƃ��l���Ă����B

	�Eobj(�̂܂��͌Q��)��Load������������B

	�E�Q�[���̃��Z�b�g�������B

	�E�������ꂽobj��idmap::IDgenerateMap(ObjectBase����)�Ɏ��[����Ă���B
		
		�E�����obj��ptr����ׂ�z���������ׂ����̂ł���B
		�E�e�z���obj��50�Ƃ��܂Ŏ��[����B
		�E�e�z��ɂ́u�e�[�u���ԍ�(ID)�v������U���Ă���obj�����[�h�E�܂��͍X�V(collision,�`��c)
		�@����ۂɂ͂��̃e�[�u���ԍ����w�肷��B
		�E�eGameScript�����g�̔z��(�ԍ�)�������Ă��āAGameScript�𑖂点��΂��̃e�[�u���ԍ��̂Ɋ܂܂��
		�@obj�̍X�V���s���B
		 
		(�E�e�e�[�u���̃T�C�Y�͌Œ�ł���̂�obj��ID�ԍ�����߂Ă��邽�߁B
			�EObjectID = �e�[�u���T�C�Y�~�e�[�u���ԍ� + �z��v�f�ԍ�
		)

	�Eobj��string�ŌŗL�������邱�Ƃ��ł��A�eGameScript�͂��̑Ή����L�^����InfoList�N���X�����B
	�@�܂��SGameScript�ɋ��ʂ�"global"��InfoList(static)���p�ӂ��Ă���
	


	�Eobject::Object�N���X�Ɠ����ł��Ȃ����l���Ă���B
  
*/

GameScript::InfoList GameScript::global;
GameScript* GameScript::script(nullptr);
GameScript* GameScript::nextscript(nullptr);
bool GameScript::restart(true); 

GameScript::GameScript(void) : local() {

}

GameScript::~GameScript(void) {

	for (auto iter : local.objectbank) {
		object::Object::Remove(iter.second.table);
	}

}

void GameScript::EndGame(void) {
	std::exit(EXIT_SUCCESS);
}

void GameScript::Load_ObjectBank(const std::string& file, const std::string& name/*=std::string("")*/) {

	std::string tempname(name);
	std::string end(Tools::DetachEnd(tempname));
	
	InfoList* infolist(&local);

	if (end == std::string("global")) { //���͂��ꂽ���O���O��"name.global"�̂悤�Ȍ`�Ȃ�
		infolist = &global;			
		end = Tools::DetachEnd(tempname);
	}

	bool newflag(tempname == std::string("new"));	

}//�͂�ڂ�

void GameScript::Remove_ObjectBank(const std::string& name) {
	
	std::string tempname(name);
	std::string end(Tools::DetachEnd(tempname)); 

	InfoList* infolist(&local);

	if (end == std::string("global")) {
		infolist = &global;			
		end = Tools::DetachEnd(tempname);
	}

	auto iter(infolist->objectbank.find(end));	
	if ((iter != infolist->objectbank.end()) && (end != std::string(""))) {	
			object::Object::Remove(iter->second.table);
			infolist->objectbank.erase(iter);
	}
	else
		error::GameScript::noobjectbankname(name);

}

void GameScript::Load_Object(const std::string& file, const std::string& name) {

	std::string tempname(name);
	std::string end(Tools::DetachEnd(tempname));

	InfoList* infolist(&local);

	if (end == std::string("global")) { //���͂��ꂽ���O��"name.global"�̂悤�Ȍ`�Ȃ�
		infolist = &global;
		end = Tools::DetachEnd(tempname);
	}
	/*if (end == std::string("ui")) {	
		infolist = &ui;
		end = Tools::DetachEnd(tempname);
	}*/

	auto iter(infolist->object.find(end));		
	if (iter == infolist->object.end()) {	

		infolist->object[end] =
			InstanceInfo(object::Object::MakeObject(file, infolist->objectbank[""].table));
	}
	else
		error::GameScript::multipleobjectname(name);

}

void GameScript::Remove_Object(const std::string& name) {

	std::string tempname(name);
	std::string end(Tools::DetachEnd(tempname));

	InfoList* infolist(&local);

	if (end == std::string("global")) {
		infolist = &global;
		end = Tools::DetachEnd(tempname);
	}

	auto iter(infolist->object.find(end));		
	if ((iter != infolist->object.end()) && (end != std::string(""))) {

		unsigned int deltable(object::Object::Remove(iter->second.ID));
		if (deltable) {

			std::vector<idmap::IDgenerateMap<object::ObjectBank*>::BoardID>& table(infolist->objectbank[""].table);
			for (auto iter = table.begin(); iter != table.end(); ++iter) {
				if (*iter == deltable) {
					table.erase(iter);
					break;
				}
			}
		}
		infolist->object.erase(end);
	}
	else
		error::GameScript::noobjectname(name);

}

template<typename... Rest>
void GameScript::Remove_Object(const std::string& name, const Rest&... rest) {

	Remove_Object(name);

	if (sizeof...(rest) != 0)
		Remove_Object(rest...);

}

const GLMatrix& GameScript::coord(const std::string& name) {

	std::string tempname(name);
	std::string end(Tools::DetachEnd(tempname));

	InfoList* infolist(&local);

	if (end == std::string("global")) {
		infolist = &global;
		end = Tools::DetachEnd(tempname);
	}
	/*if (end == std::string("ui")) {	
		infolist = &ui;
		end = Tools::DetachEnd(tempname);
	}*/

	const GLMatrix* val(nullptr);
	auto iter(infolist->object.find(end));	
	if ((iter != infolist->object.end()) && (end != std::string(""))) {
		val = &object::Object::GameScript::coord(infolist->object[end].ID);
	}
	else {
		error::GameScript::noobjectname(name);
		val = &object::Object::GameScript::coord(0);	
	}

	return *val;
}

void GameScript::NextScript(GameScript& script) {
	nextscript = &script;
	ReStart();
}

void GameScript::ReStart(void) { restart = true; }

void GameScript::Camera(camera::Mode mode, const std::string& targetobjectname/* = std::string("")*/) {

	if (targetobjectname != std::string("")) {

		std::string tempname(targetobjectname);
		std::string end(Tools::DetachEnd(tempname));

		InfoList* infolist(&local);

		if (end == std::string("global")) {
			infolist = &global;
			end = Tools::DetachEnd(tempname);
		}

		auto iter(infolist->object.find(end));		
		if ((iter != infolist->object.end()) && (end != std::string(""))) {
			camera::Camera::SetTarget(infolist->object[end].ID);
		}
		else
			error::GameScript::noobjectname(targetobjectname);

	}

	camera::Camera::SetMode(mode);


}


void GameScript::Initialize(GameScript& script) {
	GameScript::nextscript = &script;
	global.objectbank[""].table.push_back(0);		
}

void GameScript::localupdate(void) {	

	std::vector<idmap::IDgenerateMap<object::ObjectBank*>::BoardID> objtable;		
	combinetable(objtable);

	object::Object::Update(objtable, camera::Camera::Update());
	//object::Object::Update(ui.objectbank[""].table, Matrix4::Identity); 

}

void GameScript::combinetable(std::vector<idmap::IDgenerateMap<object::ObjectBank*>::BoardID>& objtable) {

	for (const auto iter : global.objectbank) {	
		objtable.insert(objtable.end(), iter.second.table.begin(), iter.second.table.end());
	}
	for (const auto iter : local.objectbank) {
		objtable.insert(objtable.end(), iter.second.table.begin(), iter.second.table.end());
	}

}

void GameScript::Update(void) {	

	if (restart) {

		if (nextscript) script = nextscript;	
		nextscript = nullptr;
		script->Load();
		restart = false;
	}

	script->Progress();
	script->localupdate();

}

/***************************************************************************

	InstanceInfo

****************************************************************************/

/*

	�Eonj�ƌŗL����Ή������邽�߂̋K�i�B

*/

GameScript::InstanceInfo::InstanceInfo(void) : ID() {}
GameScript::InstanceInfo::InstanceInfo(unsigned int ID) : ID(ID) {}

/***************************************************************************

	BankInfo

****************************************************************************/

/*

	�Eobj���Q�̂Ń��[�h����ꍇ�ɂ��̌Q�̂ɌŗL�������Ή������邽�߂̋K�i�B

		�Eidmap::IDgenerateMap�̃e�[�u��ID��v�f�ɂ���z������B

*/

GameScript::BankInfo::BankInfo(void) : table(){}

/***************************************************************************

	InfoList

****************************************************************************/

/*
	
	�E��L��InstanceInfo, BankInfo�������o�Ɏ��N���X

	�E�eGameScipt�͂���������o�ɂ����Astatic��"global"�Ȃ��̂�����B

*/

GameScript::InfoList::InfoList(void) : objectbank(), object(){
	objectbank[""] = BankInfo();	
	object[""] = InstanceInfo();	
}

/***************************************************************************

	GameScriptBase

****************************************************************************/

/*

	�E�����t�@�C����"Base"(static�N���X)����������\���Ƀ\�[�X�R�[�h�𓝈ꂷ�邽�߂ɋ}篗p�ӂ����B

	�E����܂߁AGameBase�N���X�͖������B

*/

namespace game {
	class GameScriptBase;
}

class game::GameScriptBase { //���}�[�u

public:

//->Game 
	static void Update(void);
	static void Initialize(GameScript& script);
};

void game::GameScriptBase::Update(void) { GameScript::Update(); }
void game::GameScriptBase::Initialize(GameScript& script) {	GameScript::Initialize(script); }

/***************************************************************************

	satellite

****************************************************************************/

using namespace game::satellite; //���}�[�u

void GameScript_s_Game::Update(void) { game::GameScriptBase::Update(); }
void GameScript_s_Game::Initialize(GameScript& script) { game::GameScriptBase::Initialize(script); }