#pragma once
#include <string>
#include <rapidjson/fwd.h>

namespace librarian {
	namespace error {
		class Error;
		class tARRAY;
		class tBoard;
		class tBinaryTree;
		class Action;
		class Audio;
		class Controller;
		class Data;
		class Creations;
		class Manipulations;
		class Game;
		class GameScript;
		class Gpu;
		class tIDcountHashMap;
		class tIDgenerateMap;
		class Object;
		class Tool;	
		class Vector;
	}
	namespace message {
		class tBinaryTree;
	}
}
class Vector2;
class Vector3;
class Vector4;
template <typename T> class ARRAY;
template <typename Template> class StdBoard;
template <typename Template> class Board;
template <typename Instance> class BaseBinaryTree;
template <typename Instance> class BinaryTree;
namespace action {
	class Action;
	class ActionBase;
	class Shape;
	namespace satellite {
		class Game;
	}
}
enum FMOD_RESULT : int;
namespace audio {
	class AudioBase;
	class InstanceInfo;
	namespace satellite {
		class Game;
	}
}
namespace game {
	class GameBase;
	class GameScript;
	namespace controller {
		class ControllerBase;
	}
	namespace satellite {
		namespace selfy {
			class Controller;
		}
	}		
}
namespace object {
	class ObjectCore;
	class ObjectBase;
	namespace data {
		class DataBase;
		class CreationsBase;
		class ManipulationsBase;
		namespace satellite {
			class Object;
			class Gpu;
			namespace selfy {
				class Manipulations;
			}
		}
	}
}
namespace gpu {
	using GLuint = unsigned int;
	using GLint = int;
	using GLenum =  unsigned int;
	class Gpu;
	class GpuBase;
	namespace satellite {
		class Game;
	}
}
namespace idmap {
	template <typename Key, typename Instance>
	class IDcountHashMap;
	template <typename Instance> class IDgenerateMap;
}
class Tools;

using namespace librarian;

class error::tARRAY {

	template <typename T> friend class ARRAY;

	static void elementexceed(const std::string& derived);

};

class error::tBoard {

	template <typename Template> friend class StdBoard;
	template <typename Template> friend class Board;
	friend class tIDgenerateMap;

	static void Nullregister(void);
	static void Nullremove(void);
	static void shortenreallocate(void);
	static bool NullRemove_flag;

};

class error::tBinaryTree {

	template <typename Instance> friend class BaseBinaryTree;
	template <typename Instance> friend class BinaryTree;

	static void treemismatch(void);
	static void dimensionmismatch(void);

};

class message::tBinaryTree {

	template <typename Instance> friend class BinaryTree;

	static void Nodeinfoleft(void);

};

class error::Action {

	friend class action::Shape;
	friend class action::Action;
	friend class action::ActionBase;
	friend class action::satellite::Game;

	static void nonpositivelength(void);
	static void rangecheck(const Vector3& min, const Vector3& max);
	static Vector3 minrange, maxrange;

	static void undefiendcollision(unsigned int type);
	static void undefiendboundary(unsigned int type);
	static void pointslengtherror(void);

};

class error::Audio {

	friend class audio::AudioBase;
	friend class audio::InstanceInfo;
	friend class audio::satellite::Game;

	static void CreateFMODStdSysCheck(FMOD_RESULT result);
	static void FMODStdSysInitCheck(FMOD_RESULT result);
	static void FMODBankLoadCheck(FMOD_RESULT loadresult, const std::string& bankname);
	static void AudioBusNotExist(const std::string& busname);
	static void FMODEventNotExist(const std::string& eventname);
	static void FMODInstanceIDNotExist(void);
	static void NotExistInstanceManage(void);
	static void MultipleRegister(const std::string& filename);
	static bool noeventinstanceflag;
	static void NoEventInstance(void);
	static void NoEventParameterName(const std::string& name);
	static void CreateEventInstanceFalse(void);

};

class error::Controller {

	friend class game::controller::ControllerBase;
	friend class game::satellite::selfy::Controller;

	static void nosdlgpadpointer(void);
	static void ButtonsShortage(void);

};

class error::Data {

	friend class object::data::DataBase;
	friend class object::data::satellite::Object;
	friend class object::data::satellite::Gpu;

	static void UnsupportedVertFile(const std::string& filename);
	// JSON ------------------------------------------------------------------
	enum jsonerror {
		JSON_IsJSONvalid, JSON_IsVersion_1, JSON_LoadTextures,
		JSON_LoadVertex,//JSON_VertexDataFormat ,
		JSON_LoadVertexIndex, JSON_IsVertexData, JSON_IsVertIndexData,
		/*JSON_Directory,*/ JSON_ShaderLoad, JSON_SpecularPower,
		JSON_LoadVertexfile, JSON_LoadVertIndexfile,
		JSON_Vertsdatatype, JSON_Createname, JSON_Createtype
	};
	static void RapidJSONCheck(const char* filename, const rapidjson::Document& doc, int errornum);
	static void JSONVertexFormatCheck(const char* filename, const rapidjson::Value& verts, int rownum);
	static void JSONVertIndexFormatCheck(const char* filename, const rapidjson::Value& vertindexs);
	static void InversedToggle(void);
	static void NoSuchCreate(const std::string& name);
	static void UnregisteredManipulationName(const std::string& name);
	static void MakeNullptr(void);

};

class error::Creations {

	friend class object::data::CreationsBase;

	static void MultipleCreateName(const std::string& name);
	static void CreateNULL(void);
	static void CreateParameterError(const std::string& createname, int iparameternum = NULL, int fparameternum = NULL);
		
};

class error::Manipulations {

	friend class object::data::ManipulationsBase;

	static void MultipleMovementFuncName(const std::string& name);

};

class error::Game {

	friend class game::GameBase;

	static void SDLErrorCheck(bool errorcheck);
	static void ScreensizetableError(void);

};

class error::GameScript {

	friend class game::GameScript;

	static void banknewerror(const std::string& bankname);
	static void multipleobjectname(const std::string& name);
	static void noobjectname(const std::string& name);
	static void noobjectbankname(const std::string& name);

};

class error::Gpu {

	friend class gpu::GpuBase;
	friend class gpu::satellite::Game;

	static void OpenGLErrorCheck(int errorplace);
	static void ShaderWrongExtension(const std::string& filename);
	static void GLEWErrorCheck(gpu::GLenum check);
	static void ShaderCompileCheck(gpu::GLuint shader, const char* filename);
	static void ShaderLinkCheck(gpu::GLuint shaderprogramID);
	static void ShaderGetUniformLocCheck(gpu::GLint check, const char* uniformname);
	static void SOIL_LoadTexCheck(const unsigned char* ptr);
	static void vertbuffempty(void);

};

class error::tIDcountHashMap {

	template <typename Key, typename Instance>
	friend class idmap::IDcountHashMap;

	static void AlreadyRegistered(const std::string& mapname, const std::string& key);
	static void NoRegistertoDelete(const std::string& mapname, const std::string& key);
	static void IDmultipled(const std::string& mapname, const std::string& key);
	static void NoIDtoDelete(const std::string& mapname);

};

class error::tIDgenerateMap {

	template <typename Instance>
	friend class idmap::IDgenerateMap;

	static void BoardSize0(const std::string& mapname);
	static void nonexistBoard(const std::string& mapname);
	static void nonexistReadBoard(const std::string& mapname);
	static void nullinstance(const std::string& mapname);
	static void idzeroremove(const std::string& mapname);
	static void boardidzeroremove(const std::string& mapname);

	static void settBoardNullRemoveflag(bool flag);

};

class error::Object {

	friend class object::ObjectCore;
	friend class object::ObjectBase;

	static void ReadNullptr(void);
	static void ObjectCoreDefaultConstructorUse(void);

};

class error::Tool {

	friend class Tools;

	static void ExtensionUnExist(const std::string& filename);

};

class error::Vector {

	friend class Vector2;
	friend class Vector3;
	friend class Vector4;

	static void zerodividenormalize(void);
	static void vectoroverlimit(void);

};

/***************************************************************************

	Error

****************************************************************************/

class error::Error :
public tARRAY, public tBoard, public tBinaryTree,
public Action, public Audio, public Controller, public Data,
public Creations, public Manipulations, public Game, public GameScript,
public Gpu, public tIDcountHashMap, public tIDgenerateMap, public Object,
public Tool, public Vector, public message::tBinaryTree
{

public: 


	enum recordplace : int {
		getValue = -1,
		Boot, Run, ReGame, ShutDown, Input, WaittillNextFrame, ScreenOutput, ModeChange,
		Load = 10,
		InitializeGame, UpdateGame, Manipulate, EndMode,
	};

	static void FileOpenCheck(const std::ifstream& stream, const std::string& filename);
	static void Debug_RecordPlace(recordplace inplaceID);
	static void WarningString(const std::string& string);
	static void ErrorString(const std::string& string);

};