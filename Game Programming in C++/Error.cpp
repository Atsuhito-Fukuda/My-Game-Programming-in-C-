#include "Error.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Vector.h"
#include <fmod_errors.h>
#include <rapidjson/document.h>
#include <SDL.h>	
#include <GL/glew.h>

using namespace librarian::error;

/*
	[ namespace librarian::error ]

	�E�����ł̓G���[�����������B

	�E�A�v���E�eclass�͂܂�����r���ł���A�~�X��ύX�����������Ƃ�\�����ăG���[�����𒣂菄�点�Ă���B

		�E�S�̂��ł܂��Ă���΂Ȃ�����G���[������H

*/

/***************************************************************************
	
	ErrorBase

****************************************************************************/


void Error::ErrorString(const std::string& string) {
	Error::WarningString(string);
	std::exit(EXIT_SUCCESS);
}

/***************************************************************************
	Array
****************************************************************************/

void error::tARRAY::elementexceed(const std::string& derived) {
	Error::WarningString(std::string("ARRAY") + derived + " Error : elements excessed (operator[]). returned *new.\n");
} // ��template�ɔ��������

/***************************************************************************
	Board
****************************************************************************/

void error::tBoard::Nullregister(void) {
	static bool first(true);
	if (first) {
		Error::WarningString("Board warning : null instance has been tried to be registered. skipped.\n ");
		first = false;
	}
}
void error::tBoard::Nullremove(void) {
	if (NullRemove_flag)
		Error::WarningString("Board warning : null instance has been tried to be removed. skipped.\n ");
}
void error::tBoard::shortenreallocate(void) {
	Error::WarningString("Board warning : tried to shorten board length. skipped.\n ");
}
bool error::tBoard::NullRemove_flag(true);
//��ARRAY��eARRAY���ǂ��Ɏ��̂���邩�悭�킩��Ȃ������̂�
//����őS��flag�͋��L�ƂȂ�̂�?

/***************************************************************************
	BinaryTree
****************************************************************************/

void error::tBinaryTree::treemismatch(void) {
	Error::WarningString(
		"BinaryTree warning : NodeInfo is not used in proper tree. Shall be messed.\n"
	);
}

void error::tBinaryTree::dimensionmismatch(void) {
	Error::WarningString(
		"Base/BinaryTree warning : input code(vector<char>) size does not match tree dimension.\n"
	);
}

void message::tBinaryTree::Nodeinfoleft(void) {	// �Q�[������鎞�ɖ��񃊃}�C���_�[���o���̂���
	static bool check(true);
	if (check) {
		std::cout << "BinaryTree message : Tree is deleted but NodeInfo still register it.\n" <<
			"Deleting before deleting Nodeinfo will cause unauthorized memory access"; // ����
		check = false;
	}
}

/***************************************************************************
	Action
****************************************************************************/

void Action::nonpositivelength(void) {
	Error::WarningString("Action warning : non postive saction length.\n");
}
void Action::rangecheck(const Vector3& min, const Vector3& max) {	// �� ���͂��̕��@��
	static bool first(true);
	if (first)
		for (int i(0); i < 3; ++i) {
			if ((min[i] <= minrange[i]) || (max[i] >= maxrange[i])) {
				Error::WarningString("Action warning : an Objects position.moved out of range.collision boundary will cycle.\n");
				first = false;
				break;
			}
		}// error��������ꍇ�̏������炵�͂���Ȃ�(check==true�Ȃ�A�Ƃ�)
}
void Action::undefiendcollision(unsigned int type) {
	Error::WarningString("Shape warning : undefiened collision (code d" + std::to_string(type) + ")\n");
}	// �ʓ|�Ȃ̂�10�i�@�\��
void Action::undefiendboundary(unsigned int type) {
	Error::WarningString("Shape warning : undefiened collision (code d" + std::to_string(type) + ")\n");
}	// �ʓ|�Ȃ̂�10�i�@�\��
void Action::pointslengtherror(void) {
	Error::WarningString("Shape(constructor) warning : vector size is not equal to pointslength.\n");
}	// �ʓ|�Ȃ̂�10�i�@�\��

Vector3 Action::minrange;
Vector3 Action::maxrange;

/***************************************************************************
	Audio
****************************************************************************/

void Audio::CreateFMODStdSysCheck(FMOD_RESULT result) {// ptr���g���K�v������̂ŁA�����ŃG���[�Ȃ�exit���邩�AAudio�֐��𖳌��ɂ��邩���H
	if (result != FMOD_OK)
		Error::WarningString(std::string("FMOD Error : could not create FMOD system : ") + FMOD_ErrorString(result) + "\n");
}
void Audio::FMODStdSysInitCheck(FMOD_RESULT result) {
	if (result != FMOD_OK)
		Error::WarningString(std::string("FMOD Error : could not Initialize FMOD system : ") + FMOD_ErrorString(result) + "\n");
}
void Audio::FMODBankLoadCheck(FMOD_RESULT loadresult, const std::string& bankname) {
	if (loadresult != FMOD_OK)	Error::WarningString(
		std::string("FMOD Error : could not load Bank ") + bankname +
		" : " + FMOD_ErrorString(loadresult) + "\n"
	);
}
void Audio::AudioBusNotExist(const std::string& busname) {
	Error::WarningString(
		std::string("Audio Error : FMOD Bus ") + busname + " does not exist (in map)\n");
}
void Audio::FMODEventNotExist(const std::string& eventname) {	// ������Ƃ����ƈႤ�������ɂȂ邪�icheck��map�̃A�N�Z�X�����邵�Abool���ߏ�₵�j
	Error::WarningString(
		std::string("FMOD Error : FMOD Event ") + eventname + " does not exist (in map)\n");
}

void Audio::FMODInstanceIDNotExist(void) {
	Error::WarningString("FMOD Error : FMOD Instance ID does not exist (in map)\n");
}
/*
static void InstanceIDnotExist(void) {
	Error::WarningString("Audio Error : Audio Instance ID does not exist (in SOUND map)\n");
}
*/
void Audio::NotExistInstanceManage(void) {
	Error::WarningString("Audio Error : this FMOD Instance Manage does not exist\n");
}
void Audio::MultipleRegister(const std::string& filename) {
	Error::WarningString(
		std::string("Audio Error : do not register same event/bus file name(\"") + filename + "\").\n"
	);
}
void Audio::NoEventInstance(void) {
	if (noeventinstanceflag)
		Error::WarningString(
			std::string("Audio Error : No Event Instance has been tried to be managed.\n")
		);
}
void Audio::NoEventParameterName(const std::string& name) {
	Error::WarningString(
		std::string("Audio Error : event parameter name \"") + name + "\" does not exist.\n"
	);
}
void Audio::CreateEventInstanceFalse(void) {
	Error::WarningString("Audio Error : could not create fmod event instance.\n");
}
bool Audio::noeventinstanceflag(true);

/***************************************************************************
	Controller
****************************************************************************/

void Controller::nosdlgpadpointer(void) {
	Error::WarningString("CONTROLLER Error : no SDL GPad pointer \n");
}
void Controller::ButtonsShortage(void) {
	Error::WarningString("Controller Error : Shortage of buttons. Should be more than SDL uses.\n");
}

/***************************************************************************
	Data
****************************************************************************/

void Data::UnsupportedVertFile(const std::string& filename) {
	Error::WarningString(
		std::string("Gpu Error : \"") + filename + "\" Unsupported Vert file \n");
}

void Data::RapidJSONCheck(const char* filename, const rapidjson::Document& doc, int errornum) {

	//std::string string(std::string("Rapid JSON Error : file \"") + filename + "\" ");	// ���񂱂ꂵ�Ă��珈���H��

	switch (errornum) {	// �� �f�[�^�̍쐬���[�g���Œ�ł����""�������邩�̃`�F�b�N�͂����̂���

	case JSON_IsJSONvalid:
		if (!doc.IsObject()) Error::WarningString(
			std::string("Rapid JSON Error : file \"") + filename + "\" not valid.\n"
		);
		break;
	case JSON_IsVersion_1:

		if (doc.FindMember("version") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"version\".\n"
			);

		else if (doc["version"].GetInt() != 1)
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" not vesion 1.\n"
			);

		break;
	case JSON_LoadTextures:
		if (doc.FindMember("textures") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"textures\".\n"
			);

		else
			if (false/*(!doc["textures"].IsArray()) || doc["textures"].Size() < 1*/)
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" has no texture data.\n"
				);

		break;
	case JSON_LoadVertexfile:
		if (doc.FindMember("vertexfilename") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"vertexfilename\".\n"
			);

		else
			if (0) {

			}
		break;
	case JSON_LoadVertIndexfile:
		if (doc.FindMember("vertindexfilename") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"vertindexfilename\".\n"
			);

		else
			if (0) {

			}
		break;
	case JSON_LoadVertex:
		if (doc.FindMember("vertices") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"vertices\".\n"
			);

		else
			if ((!doc["vertices"].IsArray()) || doc["vertices"].Size() < 1)
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" has no vertex data.\n"
				);

		break;
	case JSON_LoadVertexIndex:
		if (doc.FindMember("indices") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"indices\".\n"
			);

		else
			if ((!doc["indices"].IsArray()) || doc["indices"].Size() < 1)
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" has no vertex index data.\n"
				);

		break;
	case JSON_IsVertexData:
		if (doc.FindMember("DataType") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"DataType\".\n"
			);

		else
			if (std::string(doc["DataType"].GetString()) != "JSONVertex")
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" is no vertex JSON data.\n"
				);

		break;
	case JSON_IsVertIndexData:
		if (doc.FindMember("DataType") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"DataType\".\n"
			);

		else
			if (std::string(doc["DataType"].GetString()) != "JSONVertIndex")
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" is no vertex index JSON data.\n"
				);

		break;
	case JSON_Vertsdatatype:
		if (doc.FindMember("vertsdatatype") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"vertsdatatype\".\n"
			);

		else {
			const std::string type(doc["vertsdatatype"].GetString());
			if ((type != "Vertsdata") && (type != "Create")) {
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" has no valid vertsdatatype(\"Vertsdata\"/\"Create\")\n"
				);
			}
		}
		break;
	case JSON_Createname:
		if (doc.FindMember("createname") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"createname\".\n"
			);
		break;
	case JSON_Createtype:
		if (doc.FindMember("createtype") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"createtype\".\n"
			);

		else {
			//�������Ń`�F�b�N���́H���̂Ƃ���null�����Ȃ�����
			const std::string type(doc["createtype"].GetString());
			if (type != "null") {
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" has no valid vertsdatatype(\"null\" �Ƃ������G���[���������ׂ�����)\n"
				);
			}
		}
		break;
	case JSON_ShaderLoad:
		if (doc.FindMember("shader") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"shader\".\n"
			);

		else
			if ((!doc["shader"].IsArray()) || (doc["shader"].Size() != 2))
				Error::WarningString(
					std::string("Rapid JSON Error : file \"") + filename + "\" doesnt have 2 \"shader\" elements.\n"
				);	// �g���q�𒲂ׂ���̂��l�����邵���s�����\�ɂ��鏈�����c
		break;
	case JSON_SpecularPower:
		if (doc.FindMember("specularpower") == doc.MemberEnd())
			Error::WarningString(
				std::string("Rapid JSON Error : file \"") + filename + "\" has no \"specularpower\".\n"
			);

		else
			if (0) {

			}
		break;
	default:
		break;

	}
}

void Data::JSONVertexFormatCheck(const char* filename, const rapidjson::Value& verts, int rownum) {
	// �d���Ȃ������č����
	if ((!verts.IsArray()) || (verts.Size() != rownum))
		Error::WarningString(
			std::string("Rapid JSON Error : file \"") + filename + "\" vertex format error.\n"
		);

}

void Data::JSONVertIndexFormatCheck(const char* filename, const rapidjson::Value& vertindexs) {

	if ((!vertindexs.IsArray()) || (vertindexs.Size() != 3))
		Error::WarningString(
			std::string("Rapid JSON Error : file \"") + filename + "\" vertex index format error.\n"
		);

}

void Data::InversedToggle(void) {
	Error::WarningString(
		"DATA::Creation Error : Inverted verts loading.(func:LoadVertIndex) \n"
	);
}

void Data::NoSuchCreate(const std::string& name) {
	Error::WarningString(
		std::string("DATA::Creation Error : No such Create name : \"") + name + "\".\n"
	);
}

void Data::UnregisteredManipulationName(const std::string& name) {
	Error::WarningString(
		std::string("data::Manipulations Error :  function \"") + name + "\" is not registered.\n");
}

void Data::MakeNullptr(void) {
	Error::WarningString("data::Manipulations Error : Make(function) returned nullptr.\n");
}

void Creations::MultipleCreateName(const std::string& name) {
	Error::WarningString("DATA Error :  Tried to register same Create name.\n");
}

void Creations::CreateNULL(void) {
	Error::WarningString("DATA::Creation Error : Tried to Create NULL.\n");
}

void Creations::CreateParameterError(const std::string& createname, int iparameternum/* = NULL*/, int fparameternum/* = NULL*/) {
	//DATA::Parameter::iParameterNum iparameternum, DATA::Parameter::fParameterNum fparameternum) {

	/*
		��̑��ݎQ�Ƃ̂���DATA��enum�̗��p�͍���E�E�E
		���̂����ł��܂����ȃR�[�h����
	*/

	// �� ��place��2�d�ɏo���Ă��悢�Ƃ���

	if (iparameternum != NULL)		// �֐����ĂԂƂ���NULL�����ĂȂ��Ȃ炱������̂����Ďd�l
		Error::WarningString(
			std::string("DATA::Creation Error : \"") + createname + "\" number " +
			std::to_string(iparameternum) + " int error parameter. \n"
		);
	if (fparameternum != NULL)
		Error::WarningString(
			std::string("DATA::Creation Error : \"") + createname + "\" number " +
			std::to_string(fparameternum) + " float error parameter. \n"
		);

	// �� ���Ȃ��Ƃ�DATA�̖{�R�[�h�ł͓��͂����ԍ��͎��ۂ�parameter�ԍ��Ɉ�v���邩�H�i�߂����j
	// �Ȃ�΁A���̔ԍ����Ȃ��Ȃ�A�s���A�N�Z�X�ɂȂ�H�H


	/*switch (iparameternum) { // ��ʖ��O�ɂ��Ă��Ƃ�string�̖��O�ł��Ă��Ƃ�

	case DATA::Parameter::iParameterNum::first:
		SDL_Log("DATA::Creation Error : %s 1st int error parameter \n", createname.c_str());
		break;
	case DATA::Parameter::iParameterNum::second:
		SDL_Log("DATA::Creation Error : %s 2nd int error parameter \n", createname.c_str());
		break;
	case DATA::Parameter::iParameterNum::Null:
		break;
	default:
		break;
	}	// �X���[�^�C�v��

	switch (fparameternum) { // ��ʖ��O�ɂ��Ă��Ƃ�string�̖��O�ł��Ă��Ƃ�

	case DATA::Parameter::fParameterNum::first:
		SDL_Log("DATA::Creation Error : %s 1st float error parameter \n", createname.c_str());
		break;
	case DATA::Parameter::fParameterNum::Null:
		break;
	default:
		break;
	}	// �X���[�^�C�v��
	*/

}	// NULL�ȊO���G���[�͂��蓾�邩

void Manipulations::MultipleMovementFuncName(const std::string& name) {
	Error::WarningString("data::Manipulations Error :  Tried to register same Movement func name.\n");
}

/***************************************************************************
	Game
****************************************************************************/
	
void Game::SDLErrorCheck(bool errorcheck) {
	if (!errorcheck)
		Error::WarningString(std::string(SDL_GetError()) + "\n");
}
void Game::ScreensizetableError(void) {
	Error::WarningString("Game Option error : Screen size table is inappropriate.\n");
}

/***************************************************************************
	GameScript
****************************************************************************/

void GameScript::banknewerror(const std::string& bankname) {
	Error::WarningString("GameScript warning : not the proper identification (input name \"" + bankname + "\")\n");
}
void GameScript::multipleobjectname(const std::string& name) {
	Error::WarningString("GameScript warning : Tried to Register same Object name (input name \"" + name + "\"). skipped load.\n");
}
void GameScript::noobjectname(const std::string& name) {
	Error::WarningString("GameScript warning : Tried to interact non Registered Object name (input name \"" + name + "\"). skipped action.\n");
}
void GameScript::noobjectbankname(const std::string& name) {
	Error::WarningString("GameScript warning : Tried to interact non Registered Object bank name (input name \"" + name + "\"). skipped action.\n");
}

/***************************************************************************
	Gpu
****************************************************************************/

class Gputreehouse {

	friend class Gpu;

	static const int GLError_SetuVar = 2;
	static void GLErrorPlace(int errorplace);

};

void Gputreehouse::GLErrorPlace(int errorplace) {

	switch (errorplace) {

	case  0:
		std::cout << "[GLError Place : ]\n";
		break;
	case  GLError_SetuVar:
		std::cout << "[GLError Place : Set uniform variable]\n";
		break;
	default:
		break;
	}

}

void Gpu::OpenGLErrorCheck(int errorplace) {

	GLenum error(0);

	while ((error = glGetError()) != GL_NO_ERROR) {
		Error::WarningString(
			std::string("OpenGL Error : code ") + std::to_string(error) + "\n"
		);
		Gputreehouse::GLErrorPlace(errorplace);
	}

}

void Gpu::ShaderWrongExtension(const std::string& filename) {
	Error::WarningString(
		"GPU warning : file " + filename + " has wrong extension. skipped loading\n"
	);
}

void Gpu::GLEWErrorCheck(GLenum check) {

	if (check != GLEW_OK) // �� unsigned char�ŏo�͂����̂���
		Error::WarningString(std::string((const char*)glewGetErrorString(check)) + "\n");

	// �����Ɋւ��Ă̓L���[�����邩�s���������̂�
}
void Gpu::ShaderCompileCheck(GLuint shader, const char* filename) {	// frag vert�𓯎��ɃR���p�C�����Ă���C�ɃG���[��Ԃ��Ȃ�

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &status);
		GLchar* buf = new char[status];
		glGetShaderInfoLog(shader, status - 1, nullptr, buf);

		Error::WarningString(
			std::string("Shader Compile Error : \"") + filename + "\"\n" + buf
		);

		delete[] buf;

	}

}
void Gpu::ShaderLinkCheck(GLuint shaderprogramID) {

	GLint status;
	glGetProgramiv(shaderprogramID, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {

		GLchar* buf = new char[512];		// �悭�킩���̂�
		glGetProgramInfoLog(shaderprogramID, 512 - 1, nullptr, buf);

		Error::WarningString(std::string("Shader Link Error : \n") + buf);

		delete[] buf;

	}

}
void Gpu::ShaderGetUniformLocCheck(GLint check, const char* uniformname) {
	if (check == -1) Error::WarningString(
		std::string("Shader Error : uniform \"") + uniformname + "\" does not exist or is gl_ variable or associated with an atomic counter or a named uniform block.\n"
	);
}
void Gpu::SOIL_LoadTexCheck(const unsigned char* ptr) {
	if (!ptr) Error::WarningString("SOIL Error : failed to load texture. \n");
}
void Gpu::vertbuffempty(void) {
	Error::WarningString("Gpu Error : input Vertex Buf is Empty \n");
}

/***************************************************************************
	IDcountHashMap
****************************************************************************/

void error::tIDcountHashMap::AlreadyRegistered(const std::string& mapname, const std::string& key) {
	Error::WarningString(
		"IDcountHashMap(" + mapname + ") warning : \"" + key + "\" already registered. skiped writing.(maybe fmod event file name?)\n"
	);
}
void error::tIDcountHashMap::NoRegistertoDelete(const std::string& mapname, const std::string& key) {
	Error::WarningString(
		"IDcountHashMap(" + mapname + ") warning : \"" + key + "\" not registered. skiped deletion.\n"
	);
}
void error::tIDcountHashMap::IDmultipled(const std::string& mapname, const std::string& key) {
	Error::WarningString(
		"IDcountHashMap(" + mapname + ") warning : \"" + key + "\" has multipled ID. skipped register.\n"
	);
}
void error::tIDcountHashMap::NoIDtoDelete(const std::string& mapname) {
	Error::WarningString(
		"IDcountHashMap(" + mapname + ") warning : tried to delete unregistered ID. skipped deletion.\n"
	);
}

/***************************************************************************
	IDgenerateMap
****************************************************************************/

void error::tIDgenerateMap::BoardSize0(const std::string& mapname) {
	Error::WarningString("IDgenerateMap(" + mapname + ") warning : map or board size is 0. map shall crash\n ");
}	// �� �{����warning��ok?(�~�܂��Ă����Ȃ��Ƃ����̃G���[���e�͌�����H)
void error::tIDgenerateMap::nonexistBoard(const std::string& mapname) {
		Error::WarningString("IDgenerateMap(" + mapname + ") warning : ID points a non existing board. Null returned\n");
	}
void error::tIDgenerateMap::nonexistReadBoard(const std::string& mapname) {
	Error::WarningString("IDgenerateMap(" + mapname + ") warning : table num points a non existing board. board 0 returned\n");
}
void error::tIDgenerateMap::nullinstance(const std::string& mapname) {
	Error::WarningString("IDgenerateMap(" + mapname + ") warning : null instance has been read.(and, sure to say null instance an error?)\n");
}
void error::tIDgenerateMap::idzeroremove(const std::string& mapname) {
	Error::WarningString("IDgenerateMap(" + mapname + ") warning : id = 0 has been tried to be removed.\n");
}
void error::tIDgenerateMap::boardidzeroremove(const std::string& mapname) {
	Error::WarningString("IDgenerateMap(" + mapname + ") warning : board id = 0 has been tried to be removed.\n");
}

void error::tIDgenerateMap::settBoardNullRemoveflag(bool flag) {
	tBoard::NullRemove_flag = flag;
}

/***************************************************************************
	Object
****************************************************************************/

void Object::ReadNullptr(void) {
	Error::WarningString("Object ID map warning : Read Null ptr. skipped action.\n");
}

void Object::ObjectCoreDefaultConstructorUse(void) {
	Error::WarningString("ObjectCore error : Default constructor is only for virtual inheritance. Do not use.\n");
}

/***************************************************************************
	Tools
****************************************************************************/

void error::Tool::ExtensionUnExist(const std::string& filename) {	// Extension����̂��̂ɂ�����݌v�Ȃ̂łȂ�������out���Ă��Ƃ�
	Error::WarningString(std::string("Error : File \"") + filename + "\" has no extension.\n");
}

/***************************************************************************
	Vector
****************************************************************************/

void error::Vector::zerodividenormalize(void) {
	Error::WarningString("Vector Error : zero Divide when normalizing.\n");
}
void error::Vector::vectoroverlimit(void) {
	Error::WarningString("Vector Error : Vector element over limit.\n");
}


/***************************************************************************

	Error

****************************************************************************/

void Error::FileOpenCheck(const std::ifstream& stream, const std::string& filename) {
	if (!stream.is_open())
		Error::WarningString(std::string("File \"") + filename + "\" cannot be opened.\n");

} //�������Ŏg�����̂�Error��

void Error::WarningString(const std::string& string) {
	std::cout << string << "\n";
	Error::Debug_RecordPlace(Error::recordplace::getValue);
}

void Error::Debug_RecordPlace(recordplace inplaceID) {

	static int placeID(0);	// ERROR.cpp�������static�����������Ȃ��̂ł���������

	if (inplaceID != getValue)	placeID = inplaceID;

	else {

		std::string string;

		switch (placeID) {
		case Boot:
			string = "[Boot]\n";
			break;
		case Run:
			string = "[Run]\n";
			break;
		case ReGame:
			string = "[ReGame]\n";
			break;
		case ShutDown:
			string = "[ShutDown]\n";
			break;
		case Input:
			string = "[Input]\n";
			break;
		case WaittillNextFrame:
			string = "[WaittillNextFrame]\n";
			break;
		case ScreenOutput:
			string = "[ScreenOutput]\n";
			break;
		case ModeChange:
			string = "[ModeChange]\n";
			break;
		case Load:
			string = "[Load]\n";
			break;
		case InitializeGame:
			string = "[InitializeGame]\n";
			break;
		case UpdateGame:
			string = "[UpdateGame]\n";
			break;
		case Manipulate:
			string = "[Manipulate]\n";
			break;
		case EndMode:
			string = "[EndMode]\n";
			break;
		default:
			break;
		}

		std::cout << string;

	}

}