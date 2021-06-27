#include "ARRAY.h"
#include "Vector.h"
#include "Data.h"
#include "Error.h"

/*

	�E�e�֐���DataBase�ɓn���ꂽ�n�b�V��map�ɖ��O(string)�Ɗ֐�ptr�������œo�^����B

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

	if (iter == createmap->end()) createmap->emplace(name, func); 	// �ꉞ�`�F�b�N

	else error::Creations::MultipleCreateName(name);	// �񍐂̂�

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
	�@ ���_�A�A �@���A �B u,v���W ��vertex�f�[�^�ɓ��͂��ׂ�
*/

void CreationsBase::Sphere(ARRAY<float>& vertexbuff, ARRAY<unsigned int>& vertindexbuff) {	// �� default ���x���Ȃ��̂��ʓ|�H

	/*
		[�d�l]
		�E���_���S�̋�
		�Eprecise��slice�̐��Ɗeslice�̓_�̐��Ƃ���
		�E���̂��߁Atexture�͈ܓx�����̉~���Ƃ�precise+1�̓_������
		�E�O�p�͊e���_���Ƃ�2���̂�6��index���ł���

		[iparameter(���g�p) ]
		�E1st : precision
		�E2nd ; radius  
	*/

	unsigned int precision = 48;//��芸����

	if (precision == 0)		
		error::Creations::CreateParameterError("Sphere", 1, NULL);	

	ARRAY<float>& vertex(vertexbuff);
	ARRAY<unsigned int>& vertindex(vertindexbuff);

	vertex.ReAllocate((precision + 1) * (precision + 1) * 8);
	vertindex.ReAllocate((precision * precision * 6));

	float sphere_radius(100.f);	// �ꉞ

// vertex �o�^ -----------------------------------------------------------

	for (unsigned int slice(0); slice <= precision; ++slice) {	

		float r(sphere_radius * sin(MATH::Pi * slice / precision));
		for (unsigned int i(0); i <= precision; ++i) {

			unsigned int num((slice * (precision + 1) + i) * 8);

			/*
					[�􉽊w����]
				�Ez = r cos(��-��)
				�E(x, y) = r sin��(cos��, sin��)
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


	// index �o�^ ---------------------------------------------------------

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
		[�d�l]
		�Ez������ɐ��xprecision�̉~�������
		�E�����x�������ɕ��s�ړ��������̂��Ay������ɉ�

		[iparameter(���g�p)]
		�E1st : precision

	*/

	const unsigned int precision = 48;

	if (precision == 0)		
		error::Creations::CreateParameterError("Torus", 1, NULL);	

	ARRAY<float>& vertex(vertexbuff);
	ARRAY<unsigned int>& vertindex(vertindexbuff);

	vertex.ReAllocate((precision + 1) * (precision + 1) * 8);
	vertindex.ReAllocate((precision * precision * 6));

	const float inner_radius(50.0f), slice_radius(20.0f);

// vertices �o�^ -----------------------------------------------------------

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

	// indices �o�^ ------------------------------------------------------------

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