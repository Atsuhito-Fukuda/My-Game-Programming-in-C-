#pragma once
#include <string>
#include "Vector.h"
#include "ObjectCore.h"

namespace gpu {
	class Gpu;
	class GLSLIDs;
}
namespace game {
	class GameBase;
}
namespace object {
	class Object;
	class ObjectBase;
}

/***************************************************************************
	satellite
****************************************************************************/

namespace gpu {
	namespace satellite {
		class Game;
	}
}
class gpu::satellite::Game {

	friend class game::GameBase;

	static void Update(void);
	static void Initialize(void);

};

/***************************************************************************

	Gpu

****************************************************************************/

namespace gpu {
	class Gpu;
}
class gpu::Gpu : virtual public object::ObjectCore,
	public satellite::Game{

	friend class object::Object;
	friend class object::ObjectBase;

	GLSLIDs& glslids;

	void Draw(const Matrix4& viewprojmat4);
	
	Gpu& operator =(const Gpu& gpu); //ÉRÉsÅ[ã÷é~
	Gpu(const Gpu& gpu);

	Gpu(const std::string& vertexshader, const std::string& fragmentshader,
		const std::string& vertexfile, const std::string& vertindexfile,
		const std::string& texture, float specpower);

	~Gpu(void);

};