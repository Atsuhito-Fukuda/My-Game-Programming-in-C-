#pragma once
#include "ARRAY.h"
#include "Vector.h"
#include <vector>
#include <unordered_map>

namespace object {
	class Object;
	class ObjectBase;
	namespace data {
		class Data;
		class DataBase;
	}
}
namespace gpu {
	class GpuBase;
}
namespace action {
	enum Type : unsigned int;
}
namespace game {
	class GameBase;
}

/***************************************************************************
	satellite
****************************************************************************/

namespace object {
	namespace data {
		class Manipulations;
		namespace satellite {
			class Gpu;
			class Object;
			namespace selfy {
				class Creations;
				class Manipulations;
			}
			class Game;
		}
	}
}

class object::data::satellite::Gpu {

	friend class gpu::GpuBase;

	static const ARRAY<float>& LoadVertex(const std::string& vertexfile);
	static const ARRAY<unsigned int>& LoadVertIndex(const std::string& vertindexfile);

};
class object::data::satellite::Object {

	friend class object::ObjectBase;

	static void LoadDatatext(
		const std::string& loaddir,
		std::string& outvertexfile, std::string& outvertindexfile,
		std::string& outvertshaderfile, std::string& outfragshaderfile,
		std::string& outtexturefile, float& specpower,
		std::string& collisiontype, std::vector<Vector3>& collidepoints,
		std::string& outmanipulation
	);

	static object::Object& (*MakeFunc(const std::string& name))(ObjectBase&);

};
class object::data::satellite::selfy::Creations {

	friend class object::data::DataBase;

	static void CreationsInitialize(std::unordered_map<std::string, void(*)(ARRAY<float>&, ARRAY<unsigned int>&)>& createmap);

};
class object::data::satellite::selfy::Manipulations {

	friend class object::data::DataBase;

	static void ManipulationsInitialize(std::unordered_map<std::string, object::Object& (*)(object::ObjectBase&)>& manipulationsmap);

};
class object::data::satellite::Game {

	friend class game::GameBase;

	static void Initialize(void);

};

/***************************************************************************
	
	Data 

****************************************************************************/

class object::data::Data :
	public satellite::Gpu, public satellite::Object, 
	public satellite::selfy::Creations, public satellite::Game,
	public satellite::selfy::Manipulations

{};