#include <vector>
#include "Data.h"
#include <rapidjson/document.h>
#include <fstream>
#include <sstream>
#include "Tools.h"
#include "Error.h"
#include <string>
#include "Action.h"

/*
	[ namespace object::data ]

	・ここではファイル読み込みや頂点データ・テクスチャの用意をする。
	・DataBaseクラスは上記の処理を扱うstatic関数を用意。
	
	・Creationsには頂点データを動的に生成する関数を並べている。
	・Manipulationsには各アクターの派生クラスを並べている。

*/

namespace object {
	namespace data {
		class DataBase;
	}
}

class object::data::DataBase {

public:

	static void loadjson(const std::string& jsonfile, rapidjson::Document& doc);
	static void Create(const std::string& createname);

	static ARRAY<float> vertexbuff;
	static ARRAY<unsigned int> vertindexbuff;
	static bool begincreate;
	
	static std::unordered_map<std::string, void(*)(ARRAY<float>&, ARRAY<unsigned int>&)> createmap;
	static std::unordered_map<std::string, object::Object& (*)(object::ObjectBase&)> manipulationsmap;

//->Game
	static void Initialize(void);

};

using namespace object::data;

/***************************************************************************

	DataBase

****************************************************************************/

/*
	
	・各objの指定するファイルを読み込んでobjの各クラスがロードするべきファイル名を渡す。

	・Createは指定された名前の動的頂点データを生成して渡す。

	・MakeFuncobject::Objectの派生クラスを生成する関数を返す。

*/

std::unordered_map<std::string, void(*)(ARRAY<float>&, ARRAY<unsigned int>&)> DataBase::createmap;
std::unordered_map<std::string, object::Object& (*)(object::ObjectBase&)> DataBase::manipulationsmap;

void DataBase::Initialize(void) {

	Data::ManipulationsInitialize(manipulationsmap);
	Data::CreationsInitialize(createmap);

}

void DataBase::loadjson(const std::string& jsonfile, rapidjson::Document& doc) {

	std::ifstream ifs(jsonfile);
	error::Error::FileOpenCheck(ifs, jsonfile.c_str());

	// 全テキストを１つの文字列に読み込む ------------------
	std::stringstream filestream;
	filestream << ifs.rdbuf();
	std::string contents = filestream.str();
	rapidjson::StringStream JSONstr(contents.c_str());
	doc.ParseStream(JSONstr);	

	error::Data::RapidJSONCheck(jsonfile.c_str(), doc, error::Data::JSON_IsJSONvalid);	// こういうのは各々の（ここならJSONの)部署/関数でやるべきではとも思うが

}

void DataBase::Create(const std::string& createname) {

	auto iter = createmap.find(createname);

	if (iter != createmap.end()) (*iter->second)(vertexbuff, vertindexbuff);

	else error::Data::NoSuchCreate(createname);

}

ARRAY<float> DataBase::vertexbuff;
ARRAY<unsigned int> DataBase::vertindexbuff;
bool DataBase::begincreate(false);

/***************************************************************************
	satellite
****************************************************************************/

const ARRAY<float>& satellite::Gpu::LoadVertex(const std::string& vertexfile) {
	
	std::string name(Tools::GetFileNameOnly(vertexfile));
	std::string datatype(Tools::DetachFileExtension(name));

	if (datatype == "gpmesh") {

		rapidjson::Document doc;
		DataBase::loadjson(vertexfile, doc);

		error::Data::RapidJSONCheck(vertexfile.c_str(), doc, error::Data::JSON_IsVertexData);// 一応変なのが入らぬよう// この前のJSONではファイル名チェックをするだけで、このチェックコードがあるかは全部読み込んでから見ることになる
		error::Data::RapidJSONCheck(vertexfile.c_str(), doc, error::Data::JSON_LoadVertex);

		const rapidjson::Value& vertsJSON = doc["vertices"];
		const int rownum(8);// doc["rownum"].GetInt();

		DataBase::vertexbuff.ReAllocate(vertsJSON.Size() * rownum);

		unsigned int j(0);
		for (unsigned int i(0); i < vertsJSON.Size(); ++i) {

			const rapidjson::Value& vert = vertsJSON[i];

			for (unsigned int k(0); k < vert.Size(); ++k) {
				DataBase::vertexbuff[j++] = (float)vert[k].GetDouble();	
			}

		}
	}

	else if (datatype == "create") { 

		if (DataBase::begincreate ^= true) { //toggle.

			std::string verttype(Tools::DetachFileExtension(name));
			std::string createtype(Tools::DetachFileExtension(name)); 
			
			if (createtype == "null") {}

			DataBase::Create(name);

		}

	}

	else error::Data::UnsupportedVertFile(vertexfile);  
	

	return DataBase::vertexbuff;

}
const ARRAY<unsigned int>& satellite::Gpu::LoadVertIndex(const std::string& vertindexfile) {
	
	std::string name(Tools::GetFileNameOnly(vertindexfile));
	std::string datatype(Tools::DetachFileExtension(name));

	if (datatype == "gpmesh") {

		rapidjson::Document doc;
		DataBase::loadjson(vertindexfile, doc);

		error::Data::RapidJSONCheck(vertindexfile.c_str(), doc, error::Data::JSON_IsVertIndexData);
		error::Data::RapidJSONCheck(vertindexfile.c_str(), doc, error::Data::JSON_LoadVertexIndex);

		const rapidjson::Value& indexJSON = doc["indices"];
		const int rownum(3);//doc["rownum"].GetInt();	

		DataBase::vertindexbuff.ReAllocate(indexJSON.Size() * rownum);

		unsigned int j(0);
		for (unsigned int i(0); i < indexJSON.Size(); ++i) {

			const rapidjson::Value& ind = indexJSON[i];

			for (unsigned int k(0); k < ind.Size(); ++k) {
				DataBase::vertindexbuff[j++] = ind[k].GetUint();	
			}

		}

	}

	else if (datatype == "create") { 

		if (DataBase::begincreate ^= true) error::Data::InversedToggle(); //toggle.
		
	}
	else error::Data::UnsupportedVertFile(vertindexfile);  

	return DataBase::vertindexbuff;

}

void satellite::Object::LoadDatatext(
	const std::string& loaddir,
	std::string& outvertexfile, std::string& outvertindexfile,
	std::string& outvertshaderfile, std::string& outfragshaderfile,
	std::string& outtexturefile, float& specpower,
	std::string& collisiontype, std::vector<Vector3>& collidepoints,
	std::string& outmanipulation
)
{

	const std::string mainfile(loaddir + "/main.gpmesh");	rapidjson::Document doc;
	DataBase::loadjson(mainfile, doc);
	error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_IsVersion_1);

	error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_Vertsdatatype);

	if (std::string(doc["vertsdatatype"].GetString()) == "Vertsdata") {	
		error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_LoadVertexfile);
		outvertexfile = loaddir + "/" + doc["vertexfilename"].GetString();
		error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_LoadVertIndexfile);
		outvertindexfile = loaddir + "/" + doc["vertindexfilename"].GetString();
	}

	else {
		error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_Createname);
		const std::string createname(doc["createname"].GetString());
		error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_Createtype);
		const std::string createtype(doc["createtype"].GetString());
		outvertexfile = loaddir + "/" + createname + "." + createtype + ".vertex.create";
		outvertindexfile = loaddir + "/" + createname + "." + createtype + ".vertindex.create";
	}

	error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_ShaderLoad);
	//std::string shadername(doc["shader"].GetString());
	outvertshaderfile = "Shader/Phong.vert";	outfragshaderfile = "Shader/Phong.frag";

	error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_LoadTextures);
	outtexturefile = loaddir + "/" + doc["textures"].GetString();;

	error::Data::RapidJSONCheck(mainfile.c_str(), doc, error::Data::JSON_SpecularPower);
	specpower = (float)doc["specularpower"].GetDouble();


// 仮(errorなし) ----------------------------------------------------- 
	collisiontype = doc["collision"].GetString();

	collidepoints.emplace_back(Vector3());	
	collidepoints.emplace_back(Vector3());
	collidepoints.emplace_back(Vector3());
	collidepoints.emplace_back(Vector3(100.0f, 0.0f, 0.0f));

	outmanipulation = doc["manipulate"].GetString();

}

object::Object& (*satellite::Object::MakeFunc(const std::string& name))(ObjectBase&){
	
	object::Object&(*make)(ObjectBase&)(nullptr);

	auto iter = DataBase::manipulationsmap.find(name);

	if (iter != DataBase::manipulationsmap.end())
		make = DataBase::manipulationsmap[name];
	else
		error::Data::UnregisteredManipulationName(name);

	if (!make) error::Data::MakeNullptr();

	return make;

}

void satellite::Game::Initialize(void) { DataBase::Initialize(); }