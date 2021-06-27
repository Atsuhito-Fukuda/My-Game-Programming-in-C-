#include "Gpu.h"
#include <GL/glew.h>
#include <SOIL.h>
#include <fstream>
#include <sstream>
#include "Error.h"
#include "IDmap.h"
#include "Data.h"

/*
	[ namespace gpu ]

	・ここではグラフィックを扱う。(外部ライブラリOpenGLを用いる。)
	・GpuBaseクラスはグラフィックを扱うstatic関数を用意。
	・GLSLIDsクラスは各objの管理するグラフィック情報を持つ。Gpuクラスの一部で外部には宣言不明。(pimplイディオム)
	・Gpuクラスの非static部分はobject::ObjectBaseクラスが継承する。

*/

namespace gpu {
	using GLuint = unsigned int;
	using GLenum = unsigned int;
	using GLint = int;
	using GLsizei = int;
	class GpuBase;
}
class gpu::GpuBase { 

public:

	using VboIDinfo = idmap::IDinfo_i;
	using TextureIDinfo = idmap::IDinfo;
	using ShaderIDinfo = idmap::IDinfo;

// OpenGL Vertex - - - - - - - - - -  - - - - - - - - - - -  - - - - - 

	enum class verttype { vertex, vertindex };
	enum class shadertype { vert, fragment };

	static GLuint LoadVertex(const ARRAY<float>& inarray);
	static GLuint LoadVertIndex(const ARRAY<unsigned int>& inarray);
	static GLuint LoadVert(const std::string& filename, verttype type, GLsizei& indexnum);
	static GLuint Loadf8VertArray(GLuint indexID, GLuint vertexID);
	static void UnloadVertArray(GLuint vertarrayID);

	// Load Texture -------------------------------
	static GLuint LoadTexture(const std::string& filename);
	static void UnloadTexture(GLuint textureID);

// Open GL Shader - - - - - - - - - - - - - - - - - - - - - - - - - - 
	static GLuint LoadShader(const std::string& filename, shadertype type);
	static GLuint CompileShader(const std::string& filename, GLenum shadertype);
	static GLuint LoadShaderProgram(const std::string& vertshaderfile, const std::string& fragshaderfile);
	static void UnloadShaderProgram(GLuint shaderprogramID); 

	static GLint DownloadGLSLuID(GLuint shaderprogramID, const std::string& name);

	static void ClearScreen(void);

private:

// CountMaps - - - - - - - - - - - - - - - - - - - - - - - - - - 
	static idmap::IDcountHashMap<std::string, VboIDinfo> vertfilemap;
	static idmap::IDcountHashMap<std::string, TextureIDinfo> texturemap;
	static idmap::IDcountHashMap<std::string, ShaderIDinfo> shadermap;

};

using namespace gpu;

/***************************************************************************

	GLSLIDs

****************************************************************************/

/*

	・各objのもつshader,頂点データ,textureなどの情報を持つ。

	・コンストラクタより各ファイルが渡されロードされる

*/

class gpu::GLSLIDs {

	friend class Gpu;

	GLuint mShaderProgramID;
	GLuint mVertexArrayID;
	GLuint mTextureID;
	GLsizei IndexNums;

	// shader uIDs
	GLuint uWorldTransformID;
	GLuint uViewProjID;
	GLuint uAmbientLightID;
	GLuint uSpecPowerID;
	GLuint uDirectionalLight_mDirectionID;
	GLuint uDirectionalLight_mDiffuseColorID;
	GLuint uDirectionalLight_mSpecColorID;
	float SpecPower;

	GLSLIDs( 
		const std::string& vertexshader, const std::string& fragmentshader,
		const std::string& vertexfile, const std::string& vertindexfile,
		const std::string& texture, float specpower
	);

	~GLSLIDs(void);

	// debug
	void show(void);

}; 

GLSLIDs::GLSLIDs(
	const std::string& vertexshader, const std::string& fragmentshader,
	const std::string& vertexfile, const std::string& vertindexfile,
	const std::string& texture, float specpower
) :
	mVertexArrayID(), mShaderProgramID(), mTextureID(), IndexNums(),
	uWorldTransformID(-1), uViewProjID(-1), uAmbientLightID(-1), uSpecPowerID(-1),
	uDirectionalLight_mDirectionID(-1), uDirectionalLight_mDiffuseColorID(-1), uDirectionalLight_mSpecColorID(-1),
	SpecPower(10.0f)
{

	mVertexArrayID = GpuBase::Loadf8VertArray(
		GpuBase::LoadVert(vertindexfile, GpuBase::verttype::vertindex, IndexNums),
		GpuBase::LoadVert(vertexfile, GpuBase::verttype::vertex, IndexNums)
	);	

	mShaderProgramID = GpuBase::LoadShaderProgram(vertexshader, fragmentshader); 

	uWorldTransformID = GpuBase::DownloadGLSLuID(mShaderProgramID, "uWorldTransform");
	uViewProjID = GpuBase::DownloadGLSLuID(mShaderProgramID, "uViewProj");
	uAmbientLightID = GpuBase::DownloadGLSLuID(mShaderProgramID, "uAmbientLight");		
	uSpecPowerID = GpuBase::DownloadGLSLuID(mShaderProgramID, "uSpecPower");
	uDirectionalLight_mDirectionID = GpuBase::DownloadGLSLuID(mShaderProgramID, "uDirLight.mDirection");
	uDirectionalLight_mDiffuseColorID = GpuBase::DownloadGLSLuID(mShaderProgramID, "uDirLight.mDiffuseColor");
	uDirectionalLight_mSpecColorID = GpuBase::DownloadGLSLuID(mShaderProgramID, "uDirLight.mSpecColor");

	mTextureID = GpuBase::LoadTexture(texture);

	SpecPower = specpower;

}

GLSLIDs::~GLSLIDs(void) {

	GpuBase::UnloadVertArray(mVertexArrayID);
	GpuBase::UnloadShaderProgram(mShaderProgramID);
	GpuBase::UnloadTexture(mTextureID);

}

void GLSLIDs::show(void) {

	std::cout <<
		"show GLSLIDs [mVertexArrayID:" << mVertexArrayID <<
		" mShaderProgramID:" << mShaderProgramID <<
		" mTextureID:" << mTextureID <<
		" IndexNums:" << IndexNums <<
		" uWorldTransformID:" << uWorldTransformID <<
		" uViewProjID:" << uViewProjID <<
		" uAmbientLightID:" << uAmbientLightID <<
		" uSpecPowerID:" << uSpecPowerID <<
		" uDirectionalLight_mDirectionID:" << uDirectionalLight_mDirectionID <<
		" uDirectionalLight_mDiffuseColorID:" << uDirectionalLight_mDiffuseColorID <<
		" uDirectionalLight_mSpecColorID:" << uDirectionalLight_mSpecColorID <<
		" SpecPower:" << SpecPower <<
		"]\n";

}

/***************************************************************************

	GpuBase

****************************************************************************/

/*

	・Shader, 頂点データ, Textureの読み込みなどを行う。

	・ファイルからデータを配列に読み込む・動的に頂点データ(配列)を生成するのはobject::data::Dataクラスの仕事。
		

*/

namespace gpu {
	enum mapsize : unsigned int;
}
enum gpu::mapsize : unsigned int {	
	vertfilemap = 500, texturemap = 1000, shadermap = 50
};

idmap::IDcountHashMap<std::string, GpuBase::VboIDinfo> GpuBase::vertfilemap("vertfilemap", mapsize::vertfilemap);
idmap::IDcountHashMap<std::string, GpuBase::TextureIDinfo> GpuBase::texturemap("texturemap", mapsize::texturemap);
idmap::IDcountHashMap<std::string, GpuBase::ShaderIDinfo> GpuBase::shadermap("shadermap", mapsize::shadermap);

void GpuBase::ClearScreen(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint GpuBase::LoadVertex(const ARRAY<float>& inarray) {

	GLuint vertexID;

	glGenBuffers(1, &vertexID);	
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);

	if (inarray.getlength())
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * inarray.getlength(), &(inarray[0]), GL_STATIC_DRAW);
	else
		error::Gpu::vertbuffempty();

	return vertexID;

}

GLuint GpuBase::LoadVertIndex(const ARRAY<unsigned int>& inarray) {	

	GLuint indexID;

	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);

	if (inarray.getlength())
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * inarray.getlength(), &(inarray[0]), GL_STATIC_DRAW);
	else
		error::Gpu::vertbuffempty();

	return indexID;

}

GLuint GpuBase::LoadVert(const std::string& filename, verttype type, GLsizei& indexnum) {

	VboIDinfo vboinfo;

	if (!vertfilemap.Read(filename, vboinfo)) {

		if (type == verttype::vertex) vboinfo.ID = LoadVertex(object::data::Data::LoadVertex(filename));

		else {

			const ARRAY<unsigned int>& vertindex(object::data::Data::LoadVertIndex(filename));

			vboinfo.ID = LoadVertIndex(vertindex);
			vboinfo.ivar = (int)vertindex.getlength();
			
		}

		vertfilemap.Register(filename, vboinfo);

	}

	indexnum = vboinfo.ivar;
	return (GLuint)vboinfo.ID;

}

void GpuBase::UnloadVertArray(GLuint vertarrayID) {

	GLint vertid(0), indid(0);

	glBindVertexArray(vertarrayID);

	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vertid);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &indid);

	GLuint vertexID((GLuint)vertid), vertindexID((GLuint)indid);

	if (vertfilemap.Remove(vertexID)) glDeleteBuffers(1, &vertexID);

	if (vertfilemap.Remove(vertindexID)) glDeleteBuffers(1, &vertindexID);

	glDeleteVertexArrays(1, &vertarrayID);

}

GLuint GpuBase::Loadf8VertArray(GLuint indexID, GLuint vertexID) {
	
	GLuint vertarrayID;

	glGenVertexArrays(1, &vertarrayID);		
	glBindVertexArray(vertarrayID);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);	


	glEnableVertexAttribArray(0);		
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);

	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	glBindVertexArray(NULL);

	return vertarrayID;

}

GLuint GpuBase::LoadTexture(const std::string& filename) {

	TextureIDinfo textureinfo;

	if (!texturemap.Read(filename, textureinfo)) {

		GLuint texid(0);  int channels(0), width(0), height(0);

		unsigned char* imagebuf = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
		error::Gpu::SOIL_LoadTexCheck(imagebuf);
		
		int format(GL_RGB);
		if (channels == 4) format = GL_RGBA;

		glGenTextures(1, &texid);		
		glBindTexture(GL_TEXTURE_2D, texid);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imagebuf);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(imagebuf);
		

		textureinfo.ID = texid;
		texturemap.Register(filename, textureinfo);

	}

	return (GLuint)textureinfo.ID;

}

void GpuBase::UnloadTexture(GLuint textureID) { 
	if (texturemap.Remove(textureID)) glDeleteTextures(1, &textureID);
}

// Shader - - - - - - - - - - - - - - - - - - - - - - - - - -

GLuint GpuBase::CompileShader(const std::string& filename, GLenum shadertype) {

	std::ifstream shaderfile(filename);	
	error::Error::FileOpenCheck(shaderfile, filename.c_str());

	// 全テキストを１つの文字列に読み込む ------------------
	std::stringstream sstream;	
	sstream << shaderfile.rdbuf();
	std::string contents = sstream.str();
	const char* contentsChar = contents.c_str();
	// ---------------------------------------------------

	// Shader 作成 
	GLuint outshaderID = glCreateShader(shadertype);
	glShaderSource(outshaderID, 1, &(contentsChar), nullptr);
	glCompileShader(outshaderID);

	error::Gpu::ShaderCompileCheck(outshaderID, filename.c_str());

	return outshaderID;

}

GLuint GpuBase::LoadShader(const std::string& filename, shadertype type) {

	ShaderIDinfo shaderinfo;

	if (!shadermap.Read(filename, shaderinfo)) {

		if (type == shadertype::vert) shaderinfo.ID = CompileShader(filename, GL_VERTEX_SHADER);

		else shaderinfo.ID = CompileShader(filename, GL_FRAGMENT_SHADER); 

		shadermap.Register(filename, shaderinfo);	

	}

	return (GLuint)shaderinfo.ID;

}

GLuint GpuBase::LoadShaderProgram(const std::string& vertshaderfile, const std::string& fragshaderfile) {

	GLuint shaderprogramID(glCreateProgram());

	GLuint vertshaderID(LoadShader(vertshaderfile, shadertype::vert));
	GLuint fragshaderID(LoadShader(fragshaderfile, shadertype::fragment));

	glAttachShader(shaderprogramID, vertshaderID);
	glAttachShader(shaderprogramID, fragshaderID);

	glLinkProgram(shaderprogramID);
	error::Gpu::ShaderLinkCheck(shaderprogramID);

	return shaderprogramID;

}

void GpuBase::UnloadShaderProgram(GLuint shaderprogramID) {

	const int maxputoutnum(2); GLsizei count(0);

	ARRAY<GLuint> shaders;
	shaders.ReAllocate((unsigned int)maxputoutnum);
	glGetAttachedShaders(shaderprogramID, maxputoutnum, &count, &shaders[0]); 

	if (count != maxputoutnum) shaders.ReAllocate((unsigned int)count); 

	for (unsigned int i(0); i < shaders.getlength(); ++i) { 

		if (shadermap.Remove(shaders[i])) glDeleteShader(shaders[i]);

	}

	glDeleteProgram(shaderprogramID);

}

// Set GLSL var ---------------------------------------------------

GLint GpuBase::DownloadGLSLuID(GLuint shaderprogramID, const std::string& name) {

	GLuint ulocationID;
	error::Gpu::ShaderGetUniformLocCheck((ulocationID = glGetUniformLocation(shaderprogramID, name.c_str())),name.c_str());

	return ulocationID;

}

/***************************************************************************
	satellite
****************************************************************************/

void satellite::Game::Update(void) { GpuBase::ClearScreen();; }

void satellite::Game::Initialize(void) {

// Initialize GLEW
	glewExperimental = GL_TRUE;		
	error::Gpu::GLEWErrorCheck(glewInit());
	glGetError();	

// - - - - - - - -  - - - - - - - - - 
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);	

}

/***************************************************************************

	Gpu

****************************************************************************/

Gpu::Gpu(
	const std::string& vertexshader, const std::string& fragmentshader,
	const std::string& vertexfile, const std::string& vertindexfile,
	const std::string& texture, float specpower
) :
	ObjectCore(),
	glslids(*new GLSLIDs(
		vertexshader, fragmentshader, vertexfile, vertindexfile, texture, specpower
	))
{}

Gpu::~Gpu(void){ delete &glslids; }

void Gpu::Draw(const Matrix4& viewprojmat4) {

	glBindVertexArray(glslids.mVertexArrayID);
	glBindTexture(GL_TEXTURE_2D, glslids.mTextureID);
	glUseProgram(glslids.mShaderProgramID);

	glUniformMatrix4fv(glslids.uViewProjID, 1, GL_TRUE, viewprojmat4[0]);
	glUniformMatrix4fv(glslids.uWorldTransformID, 1, GL_TRUE, mat.MakeMat4()[0]);

	glUniform1fv(glslids.uSpecPowerID, 1, &(glslids.SpecPower));

	glUniform3fv(glslids.uAmbientLightID, 1, &(Vector3(1.0f, 1.0f, 1.0f)[0]));
	glUniform3fv(glslids.uDirectionalLight_mDirectionID, 1, &(Vector3(0.7f, 0.7f, -0.7f)[0]));
	glUniform3fv(glslids.uDirectionalLight_mDiffuseColorID, 1, &(Vector3(0.0f, 1.0f, 0.0f)[0]));
	glUniform3fv(glslids.uDirectionalLight_mSpecColorID, 1, &(Vector3(0.5f, 1.0f, 0.5f)[0]));

	glDrawElements(GL_TRIANGLES, glslids.IndexNums, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(NULL);

}