#pragma once
#include <string>
#include <vector>
#include "IDmap.h"	
#include <unordered_map>
#include "Vector.h"

namespace object {
	class ObjectBank;
}
namespace game {
	class GameBase;
	class GameScript;
	class GameScriptBase;
}
namespace camera {
	enum Mode : unsigned int;
}

/***************************************************************************
	satellite
****************************************************************************/

namespace game {
	namespace satellite {
		class GameScript_s_Game;
	}
}
class game::satellite::GameScript_s_Game {

	friend class game::GameBase;

	static void Update(void);
	static void Initialize(game::GameScript& script);

};

/***************************************************************************

	GameScript (未整備)

****************************************************************************/

class game::GameScript : public satellite::GameScript_s_Game
{ 

	friend class game::GameScriptBase; 

protected:
	GameScript(void);
	virtual ~GameScript(void);

	virtual void Load(void) = 0;		// ゲームの動作よ記述
	virtual void Progress(void) = 0;
	virtual void EndGame(void);

	void Load_ObjectBank(const std::string& file, const std::string& name = std::string(""));
	void Remove_ObjectBank(const std::string& name);
	void Load_Object(const std::string& file, const std::string& name);
	template<typename... Rest>
	void Load_Object(const std::string& file, const std::string& name, const Rest&... rest) {	
		Load_Object(file, name);
		//if (sizeof...(rest) != 0)		
		Load_Object(rest...);
	}
	void Remove_Object(const std::string& name);
	template<typename... Rest>
	void Remove_Object(const std::string& name, const Rest&... rest);
	void Load_Audio(const std::string& file, const std::string& name);
	template<typename... Rest>
	void Load_Audio(const std::string& file, const std::string& name, const Rest&... rest);
	void Remove_Audio(const std::string& name);
	template<typename... Rest>
	void Remove_Audio(const std::string& name, const Rest&... rest);
	void SetFlag(void);
	const GLMatrix& coord(const std::string& name);	

	void Camera(camera::Mode mode, const std::string& targetobjectname = std::string(""));
	static void NextScript(GameScript& script);
	static void ReStart(void);	

private:
	class InstanceInfo {
	public:
		InstanceInfo(void);
		InstanceInfo(unsigned int ID);
		unsigned int ID;
	};
	class BankInfo {		
	public:
		BankInfo(void);
		std::vector<idmap::IDgenerateMap<object::ObjectBank*>::BoardID> table;
	};		
	class InfoList {
	public:
		InfoList(void);
		std::unordered_map<std::string, BankInfo> objectbank;	
		std::unordered_map<std::string, InstanceInfo> object;
	};

	InfoList local;
	static InfoList global;		
	//InfoList ui;		// UI画面をテストしたもの
	static GameScript* script;
	static GameScript* nextscript;
	void localupdate(void);
	void combinetable(std::vector<idmap::IDgenerateMap<object::ObjectBank*>::BoardID>& objtable);
	static bool restart;

	class UIInfo {
	public:
		UIInfo(void) : leftdown(), rightup(){}
		Vector2 leftdown;
		Vector2 rightup;
		//int textureID;
	};
	static std::vector<UIInfo> frameui;

//->Game
	static void Update(void);
	static void Initialize(GameScript& script);

};