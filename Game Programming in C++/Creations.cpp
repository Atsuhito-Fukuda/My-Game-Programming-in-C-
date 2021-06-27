#include "ARRAY.h"
#include "Vector.h"
#include "Data.h"
#include "Error.h"

/*

	・各関数はDataBaseに渡されたハッシュmapに名前(string)と関数ptrをここで登録する。

*/


namespace object {
	namespace data {
		class CreationsBase;
	}
}
class object::data::CreationsBase {

public:

	static void Register(const std::string& name, void(*func)(ARRAY<float>&, ARRAY<unsigned int>&));
	template<typename... Rest>
	static void Register(const std::string& name, void(*func)(ARRAY<float>&, ARRAY<unsigned int>&), const Rest&... rest);

	static std::unordered_map <std::string, void(*)(ARRAY<float>&, ARRAY<unsigned int>&)>* createmap;

// ----------------------------------------------
	static void Sphere(ARRAY<float>& vertexbuff, ARRAY<unsigned int>& vertindexbuff);
	static void Torus(ARRAY<float>& vertexbuff, ARRAY<unsigned int>& vertindexbuff);

};

using namespace object::data;

std::unordered_map <std::string, void(*)(ARRAY<float>&, ARRAY<unsigned int>&)>* CreationsBase::createmap(nullptr);

void CreationsBase::Register(const std::string& name, void(*func)(ARRAY<float>&, ARRAY<unsigned int>&)) {

	auto iter = createmap->find(name);

	if (iter == createmap->end()) createmap->emplace(name, func); 	// 一応チェック

	else error::Creations::MultipleCreateName(name);	// 報告のみ

}

template<typename... Rest>
void object::data::CreationsBase::Register(const std::string& name, void(*func)(ARRAY<float>&, ARRAY<unsigned int>&), const Rest&... rest) {
	Register(name, func);
	Register(rest...);
}

/***************************************************************************
	satellite
****************************************************************************/

void satellite::selfy::Creations::CreationsInitialize(std::unordered_map<std::string, void(*)(ARRAY<float>&, ARRAY<unsigned int>&)>& createmap) {

	CreationsBase::createmap = &createmap;

	CreationsBase::Register(	
		"Sphere", CreationsBase::Sphere,
		"Torus", CreationsBase::Torus
	);

	CreationsBase::createmap = nullptr; 

}

/***************************************************************************

	Creations (funcs)

****************************************************************************/

/*
	① 頂点、② 法線、 ③ u,v座標 をvertexデータに入力すべし
*/

void CreationsBase::Sphere(ARRAY<float>& vertexbuff, ARRAY<unsigned int>& vertindexbuff) {	// ※ default 精度がないのも面倒？

	/*
		[仕様]
		・原点中心の球
		・preciseはsliceの数と各sliceの点の数とする
		・そのため、textureは緯度方向の円ごとにprecise+1の点をもつ
		・三角は各頂点ごとに2つ作るので6のindexができる

		[iparameter(未使用) ]
		・1st : precision
		・2nd ; radius  
	*/

	unsigned int precision = 48;//取り敢えず

	if (precision == 0)		
		error::Creations::CreateParameterError("Sphere", 1, NULL);	

	ARRAY<float>& vertex(vertexbuff);
	ARRAY<unsigned int>& vertindex(vertindexbuff);

	vertex.ReAllocate((precision + 1) * (precision + 1) * 8);
	vertindex.ReAllocate((precision * precision * 6));

	float sphere_radius(100.f);	// 一応

// vertex 登録 -----------------------------------------------------------

	for (unsigned int slice(0); slice <= precision; ++slice) {	

		float r(sphere_radius * sin(MATH::Pi * slice / precision));
		for (unsigned int i(0); i <= precision; ++i) {

			unsigned int num((slice * (precision + 1) + i) * 8);

			/*
					[幾何学説明]
				・z = r cos(π-θ)
				・(x, y) = r sinθ(cosφ, sinφ)
			*/

			Vector3 vec3(r * cos(2 * MATH::Pi * i / precision), r * sin(2 * MATH::Pi * i / precision),
				-sphere_radius * cos(MATH::Pi * slice / precision));

			// verts
			vertex[num++] = vec3[0];	
			vertex[num++] = vec3[1];	
			vertex[num++] = vec3[2];	
			// normal
			vec3.normalize();	
			vertex[num++] = vec3[0];
			vertex[num++] = vec3[1];
			vertex[num++] = vec3[2];
			// texcoord
			vertex[num++] = (float)i / precision;
			vertex[num++] = 1.0f - (float)slice / precision;	

		}

	}


	// index 登録 ---------------------------------------------------------

	for (unsigned int slice(0); slice < precision; ++slice) {	

		for (unsigned int i(0); i < precision; ++i) {

			unsigned int num(6 * (slice * precision + i));
			unsigned int index(slice * (precision + 1) + i);

			vertindex[num++] = index;
			vertindex[num++] = index + 1;
			vertindex[num++] = index + precision + 1;
			vertindex[num++] = index + 1;
			vertindex[num++] = index + precision + 1 + 1;
			vertindex[num++] = index + precision + 1;

		}	

	}

}

void CreationsBase::Torus(ARRAY<float>& vertexbuff, ARRAY<unsigned int>& vertindexbuff) {	

	/*
		[仕様]
		・z軸周りに精度precisionの円を作って
		・それをx軸方向に平行移動したものを、y軸周りに回す

		[iparameter(未使用)]
		・1st : precision

	*/

	const unsigned int precision = 48;

	if (precision == 0)		
		error::Creations::CreateParameterError("Torus", 1, NULL);	

	ARRAY<float>& vertex(vertexbuff);
	ARRAY<unsigned int>& vertindex(vertindexbuff);

	vertex.ReAllocate((precision + 1) * (precision + 1) * 8);
	vertindex.ReAllocate((precision * precision * 6));

	const float inner_radius(50.0f), slice_radius(20.0f);

// vertices 登録 -----------------------------------------------------------

	const float theta(2.0f * MATH::Pi / precision);
	const Matrix3 yrotate(
		Vector3(cos(theta), 0.0f, -sin(theta)),
		Vector3(0.0f, 1.0f, 0.0f),
		Vector3(sin(theta), 0.0f, cos(theta))
	);
	const Matrix3 zrotate(
		Vector3(cos(theta), sin(theta), 0.0f),
		Vector3(-sin(theta), cos(theta), 0.0f),
		Vector3(0.0f, 0.0f, 1.0f)
	);	

	ARRAY<Vector3> SliceVert(precision + 1);
	ARRAY<Vector2> SliceTex(precision + 1);
	ARRAY<Vector3> Normal(precision + 1);
	Vector3 pos(slice_radius, 0.0f, 0.0f);
	Vector3 stangent(0.0f, 0.0f, -1.0f);
	Vector3 ttangent(0.0f, -1.0f, 0.0f);

	for (unsigned int i(0); i <= precision; ++i) {	

		SliceVert[i] = pos;
		pos = zrotate * pos;
		SliceTex[i] = Vector2(0.0f, ((float)i) / precision);	
		Normal[i] = Vector3::Cross(ttangent, stangent);
		ttangent = zrotate * ttangent;

	}

	for (unsigned int ring(0); ring <= precision; ++ring) {	

		for (unsigned int i(0); i <= precision; ++i) {	

			const unsigned int vertnum(8 * (ring * (precision + 1) + i));

			// pos
			vertex[vertnum] = SliceVert[i][0];
			vertex[vertnum + 1] = SliceVert[i][1];
			vertex[vertnum + 2] = SliceVert[i][2];
			SliceVert[i] = yrotate * SliceVert[i];
			// normal
			vertex[8 * i + 3] = Normal[i][0];
			vertex[8 * i + 4] = Normal[i][1];
			vertex[8 * i + 5] = Normal[i][2];
			Normal[i] = yrotate * Normal[i];
			// texcoord
			vertex[8 * i + 6] = SliceTex[i][0];
			vertex[8 * i + 7] = SliceTex[i][1];
			SliceTex[i][0] += 2.0f / precision;
			if (SliceTex[i][0] > 1.0f)
				SliceTex[i][0] -= 1.0f;

		}

	}

	// indices 登録 ------------------------------------------------------------

	for (unsigned int ring(0); ring < precision; ++ring) {

		for (unsigned int i(0); i < precision; ++i) {

			const unsigned int indnum(6 * (ring * precision + i));

			vertindex[indnum] = ring * (precision + 1) + i;
			vertindex[indnum + 1] = (ring + 1) * (precision + 1) + i;
			vertindex[indnum + 2] = ring * (precision + 1) + i + 1;
			vertindex[indnum + 3] = ring * (precision + 1) + i;
			vertindex[indnum + 4] = (ring + 1) * (precision + 1) + i;
			vertindex[indnum + 5] = (ring + 1) * (precision + 1) + i + 1;

		}

	}


}