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
 
	・各ゲーム動作を記述しようと用意したクラス。

	・生成したGameScriptをstaticクラスに渡すことでそのGameScriptを走らせる。

	・Load時やゲームの更新時の関数をvirtual継承で記述することを考えていた。

	・obj(個体または群体)のLoadや消去を扱う。

	・ゲームのリセットも扱う。

	・生成されたobjはidmap::IDgenerateMap(ObjectBase持つ)に収納されている。
		
		・これはobjのptrを並べる配列を幾つも並べたものである。
		・各配列はobjを50個とかまで収納する。
		・各配列には「テーブル番号(ID)」が割り振られていてobjをロード・または更新(collision,描画…)
		　する際にはそのテーブル番号を指定する。
		・各GameScriptも自身の配列(番号)を持っていて、GameScriptを走らせればそのテーブル番号のに含まれる
		　objの更新を行う。
		 
		(・各テーブルのサイズは固定であるのはobjのID番号も定めているため。
			・ObjectID = テーブルサイズ×テーブル番号 + 配列要素番号
		)

	・objはstringで固有名をつけることができ、各GameScriptはその対応を記録したInfoListクラスを持つ。
	　また全GameScriptに共通の"global"なInfoList(static)も用意している
	


	・object::Objectクラスと統合できないか考えている。
  
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

	if (end == std::string("global")) { //入力された名前名前が"name.global"のような形なら
		infolist = &global;			
		end = Tools::DetachEnd(tempname);
	}

	bool newflag(tempname == std::string("new"));	

}//はりぼて

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

	if (end == std::string("global")) { //入力された名前が"name.global"のような形なら
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

	・onjと固有名を対応させるための規格。

*/

GameScript::InstanceInfo::InstanceInfo(void) : ID() {}
GameScript::InstanceInfo::InstanceInfo(unsigned int ID) : ID(ID) {}

/***************************************************************************

	BankInfo

****************************************************************************/

/*

	・objを群体でロードする場合にその群体に固有名をつけ対応させるための規格。

		・idmap::IDgenerateMapのテーブルIDを要素にする配列を持つ。

*/

GameScript::BankInfo::BankInfo(void) : table(){}

/***************************************************************************

	InfoList

****************************************************************************/

/*
	
	・上記のInstanceInfo, BankInfoをメンバに持つクラス

	・各GameSciptはこれをメンバにもつが、staticな"global"なものもある。

*/

GameScript::InfoList::InfoList(void) : objectbank(), object(){
	objectbank[""] = BankInfo();	
	object[""] = InstanceInfo();	
}

/***************************************************************************

	GameScriptBase

****************************************************************************/

/*

	・実装ファイルに"Base"(staticクラス)を持たせる構造にソースコードを統一するために急遽用意した。

	・これ含め、GameBaseクラスは未整備。

*/

namespace game {
	class GameScriptBase;
}

class game::GameScriptBase { //応急措置

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

using namespace game::satellite; //応急措置

void GameScript_s_Game::Update(void) { game::GameScriptBase::Update(); }
void GameScript_s_Game::Initialize(GameScript& script) { game::GameScriptBase::Initialize(script); }