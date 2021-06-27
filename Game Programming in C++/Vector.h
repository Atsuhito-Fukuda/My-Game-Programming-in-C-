#pragma once

/***************************************************************************

	Vector2

****************************************************************************/

class Vector2 {	

	float x[2];

public:
	Vector2(void);
	explicit Vector2(float a, float b);

// Operator --------------
	float& operator [](unsigned int i);
	const float& operator [](unsigned int i) const;
	Vector2 operator +(void) const;	
	Vector2 operator -(void) const;
	friend Vector2 operator +(const Vector2& a, const Vector2& b);
	friend Vector2 operator -(const Vector2& a, const Vector2& b);
	friend float operator *(const Vector2& a, const Vector2& b);
	friend Vector2 operator *(float a, const Vector2& b);
	friend Vector2 operator *(const Vector2& a, float b);
	friend Vector2 operator /(const Vector2& a, float b);
	Vector2& operator +=(const Vector2& a);
	Vector2& operator -=(const Vector2& a);
	Vector2& operator *=(float a);
	Vector2& operator /=(float a);

	float length(void) const;
	float squarelength(void) const;
	Vector2& normalize(void);

};


/***************************************************************************

	Vector3

****************************************************************************/

class Vector3 {

	float x[3];

public:
	Vector3(void);
	explicit Vector3(float a, float b, float c);

// Operator --------------
	float& operator [](unsigned int i);
	const float& operator [](unsigned int i) const;
	Vector3 operator +(void) const;
	Vector3 operator -(void) const;
	friend Vector3 operator +(const Vector3& a, const Vector3& b);
	friend Vector3 operator -(const Vector3& a, const Vector3& b);
	friend float operator *(const Vector3& a, const Vector3& b);
	friend Vector3 operator *(float a, const Vector3& b);
	friend Vector3 operator *(const Vector3& a, float b);
	friend Vector3 operator /(const Vector3& a, float b);
	Vector3& operator +=(const Vector3& a);
	Vector3& operator -=(const Vector3& a);
	Vector3& operator *=(float a);
	Vector3& operator /=(float a);

	float length(void) const;
	float squarelength(void) const;
	Vector3& normalize(void);
	static Vector3 Cross(const Vector3& first, const Vector3& second);
	
	void show(void) const;

};


/***************************************************************************

	Vector4

****************************************************************************/

class Vector4 {

	float x[4];

public:
	Vector4(void);
	explicit Vector4(float a, float b, float c, float d);

	// Operator --------------
public:
	float& operator [](unsigned int i);
	const float& operator [](unsigned int i) const;
	Vector4& operator +(void);
	Vector4 operator -(void);
	friend Vector4 operator +(const Vector4& a, const Vector4& b);
	friend Vector4 operator -(const Vector4& a, const Vector4& b);
	friend float operator *(const Vector4& a, const Vector4& b);
	friend Vector4 operator *(float a, const Vector4& b);
	friend Vector4 operator *(const Vector4& a, float b);
	friend Vector4 operator /(const Vector4& a, float b);
	Vector4& operator +=(const Vector4& a);
	Vector4& operator -=(const Vector4& a);
	Vector4& operator *=(float a);
	Vector4& operator /=(float a);

	float length(void) const;
	float squarelength(void) const;
	Vector4& normalize(void);

};


/***************************************************************************

	Matrix3

****************************************************************************/

class Matrix3 {

	float Mat[3][3];

public:
	Matrix3(void);
	explicit Matrix3(const float inmat[3][3]);
	Matrix3(const Vector3& base0, const Vector3& base1, const Vector3& base2);

	float* operator [](int i);
	const float* operator [](int i) const;
	friend Matrix3 operator +(const Matrix3& A, const Matrix3& B);
	friend Matrix3 operator -(const Matrix3& A, const Matrix3& B);
	friend Matrix3 operator *(const Matrix3& A, const Matrix3& B);
	friend Matrix3 operator *(const Matrix3& A, float f);
	friend Matrix3 operator *(float f, const Matrix3& A);
	friend Vector3 operator *(const Matrix3& A, const Vector3& vec);
	Matrix3& operator +=(const Matrix3& A);
	Matrix3& operator -=(const Matrix3& A);
	Matrix3& operator *=(const Matrix3& A);
	Matrix3& operator *=(float f);

	Vector3 Read_Xunit(void) const;
	Vector3 Read_Yunit(void) const;
	Vector3 Read_Zunit(void) const;
	Matrix3& xunitrotate(float theta);
	Matrix3& zunitrotate(float theta);
	Matrix3& targetyunit(const Vector3& target);
	Matrix3& targetzunit(const Vector3& target);

	void show(void);

	static Matrix3 zRotate(float theta);
	static Matrix3 RottoTarget(Vector3 startaxis, Vector3 targetaxis);
	static const Matrix3 Identity;

private:
	static void zunitnorm(Matrix3& matrix);

};


/***************************************************************************

	Matrix4

****************************************************************************/

class Matrix4 {

	float Mat[4][4];

public:
	Matrix4(void);
	explicit Matrix4(const float inmat[4][4]);

	float* operator [](int i);
	const float* operator [](int i) const;
	friend Matrix4 operator +(const Matrix4& A, const Matrix4& B);
	friend Matrix4 operator -(const Matrix4& A, const Matrix4& B);
	friend Matrix4 operator *(const Matrix4& A, const Matrix4& B);
	friend Vector4 operator *(const Matrix4& A, const Vector4& vec);
	Matrix4& operator +=(const Matrix4& A);
	Matrix4& operator -=(const Matrix4& A);
	Matrix4& operator *=(const Matrix4& A);

	static Matrix4 MakeGLMat4(const Matrix3& mat, const Vector3& trans, const Vector3& scale = Vector3(1.0f,1.0f,1.0f));
	static const Matrix4 Identity;

	void show(void) const;

};


/***************************************************************************

	GLMatrix

****************************************************************************/

class GLMatrix {

public:
	GLMatrix(void);

	Matrix4 MakeMat4(void) const;
	Matrix3 getInverse(void) const;	

	Vector3 pos;	
	Matrix3 unitary;
	Vector3 scale;

};


/***************************************************************************

	MATH

****************************************************************************/

class MATH {

	MATH(void);
	static const float epsilon;

public:
	static bool NearZero(float val);
	static float uipow(float val, unsigned int pow);

	static const float Pi;

};