#include "Vector.h"
#include <cmath>
#include "Error.h"
#include <iostream>

/*

	・float型の要素を持つ、2,3,4次元ベクトルと3x3,4x4行列とその処理を扱う。

	・GLMatrixは3次元ベクトル(位置情報)と3x3行列(ユニタリー行列の情報)を持つ。

	・MATHクラスは数値計算を扱う関数などを用意。

*/


/***************************************************************************

	Vector2

****************************************************************************/


Vector2::Vector2(void) : x{ 0,0 }
{}

Vector2::Vector2(float a, float b) : x{ a,b }
{}

Vector2 Vector2::operator +(void) const{
	return *this;	
}

Vector2 Vector2::operator -(void) const{	
	Vector2 temp;
	temp[0] = x[0] * (-1);
	temp[1] = x[1] * (-1);

	return temp;
}

Vector2 operator +(const Vector2& a, const Vector2& b) {
	return Vector2(a.x[0] + b.x[0], a.x[1] + b.x[1]);
}

Vector2 operator -(const Vector2& a, const Vector2& b) {
	Vector2 temp(a.x[0] - b.x[0], a.x[1] - b.x[1]);
	return temp;
}

float operator *(const Vector2& a, const Vector2& b) {
	return a.x[0] * b.x[0] + a.x[1] * b.x[1];
}

Vector2 operator *(float a, const Vector2& b) {
	return Vector2(a * b.x[0], a * b.x[1]);
}

Vector2 operator *(const Vector2& a, float b) {
	return Vector2(a.x[0] * b, a.x[1] * b);
}

Vector2 operator /(const Vector2& a, float b) {
	return Vector2(a.x[0] / b, a.x[1] / b);
}

Vector2& Vector2::operator +=(const Vector2& a) {
	x[0] += a.x[0];
	x[1] += a.x[1];
	return *this;
}

Vector2& Vector2::operator -=(const Vector2& a) {
	x[0] -= a.x[0];
	x[1] -= a.x[1];
	return *this;
}

Vector2& Vector2::operator *=(float a) {
	x[0] *= a;
	x[1] *= a;
	return *this;
}

Vector2& Vector2::operator /=(float a) {
	x[0] /= a;
	x[1] /= a;
	return *this;
}

float& Vector2::operator [](unsigned int i) {
	if (i < 2)		
		return x[i];
	else {
		error::Vector::vectoroverlimit();
		return x[0];
	}
}

const float& Vector2::operator [](unsigned int i) const {
	if (i < 2)
		return x[i];
	else {
		error::Vector::vectoroverlimit();
		return x[0];
	}
}

float Vector2::length(void) const {
	return sqrt(squarelength());
}

float Vector2::squarelength(void) const {
	return x[0] * x[0] + x[1] * x[1];
}

Vector2& Vector2::normalize(void) {

	float temp = sqrtf(x[0] * x[0] + x[1] * x[1]);
	if (temp != 0.0) {	
		x[0] /= temp;
		x[1] /= temp;
	}
	else
		error::Vector::zerodividenormalize();

	return *this;

}



/***************************************************************************

	Vector3

****************************************************************************/


Vector3::Vector3(void) : x{ 0,0,0 }
{}

Vector3::Vector3(float a, float b, float c) : x{ a,b,c }
{}

Vector3 Vector3::operator +(void) const {
	return *this;	
}

Vector3 Vector3::operator -(void) const {
	Vector3 temp;
	temp[0] = x[0] * (-1);
	temp[1] = x[1] * (-1);
	temp[2] = x[2] * (-1);

	return temp;
}

Vector3 operator +(const Vector3& a, const Vector3& b) {
	return Vector3(a.x[0] + b.x[0], a.x[1] + b.x[1], a.x[2] + b.x[2]);
}

Vector3 operator -(const Vector3& a, const Vector3& b) {
	return Vector3(a.x[0] - b.x[0], a.x[1] - b.x[1], a.x[2] - b.x[2]);
}

float operator *(const Vector3& a, const Vector3& b) {
	return a.x[0] * b.x[0] + a.x[1] * b.x[1] + a.x[2] * b.x[2];
}

Vector3 operator *(float a, const Vector3& b) {
	return Vector3(a * b.x[0], a * b.x[1], a * b.x[2]);
}

Vector3 operator *(const Vector3& a, float b) {
	return Vector3(a.x[0] * b, a.x[1] * b, a.x[2] * b);
}

Vector3 operator /(const Vector3& a, float b) {
	return Vector3(a.x[0] / b, a.x[1] / b, a.x[2] / b);
}

Vector3& Vector3::operator +=(const Vector3& a) {
	x[0] += a.x[0];
	x[1] += a.x[1];
	x[2] += a.x[2];
	return *this;
}

Vector3& Vector3::operator -=(const Vector3& a) {
	x[0] -= a.x[0];
	x[1] -= a.x[1];
	x[2] -= a.x[2];
	return *this;
}

Vector3& Vector3::operator *=(float a) {
	x[0] *= a;
	x[1] *= a;
	x[2] *= a;
	return *this;
}

Vector3& Vector3::operator /=(float a) {
	x[0] /= a;
	x[1] /= a;
	x[2] /= a;
	return *this;
}

float& Vector3::operator [](unsigned int i) {
	if (i < 3)		
		return x[i];
	else {
		error::Vector::vectoroverlimit();
		return x[0];
	}
}

const float& Vector3::operator [](unsigned int i) const {
	if (i < 3)		
		return x[i];
	else {
		error::Vector::vectoroverlimit();
		return x[0];
	}
}

float Vector3::length(void) const {
	return sqrt(squarelength());
}

float Vector3::squarelength(void) const {
	return x[0] * x[0] + x[1] * x[1] + x[2] * x[2];
}

Vector3& Vector3::normalize(void) {

	float temp = sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	if (temp != 0.0) {
		x[0] /= temp;
		x[1] /= temp;
		x[2] /= temp;
	}
	else
		error::Vector::zerodividenormalize();	

	return *this;

}

Vector3 Vector3::Cross(const Vector3& first, const Vector3& second) {

	return Vector3(
		first[1] * second[2] - first[2] * second[1],
		first[2] * second[0] - first[0] * second[2],
		first[0] * second[1] - first[1] * second[0]
	);

}

void Vector3::show(void) const {

	for (int i(0); i < 3; ++i) {
		std::cout << x[i] << " ";
	}
	std::cout << "\n";

}

/***************************************************************************

	Vector4

****************************************************************************/

Vector4::Vector4(void) : x{ 0,0,0,0 }
{}

Vector4::Vector4(float a, float b, float c, float d) : x{ a,b,c,d }
{}

Vector4& Vector4::operator +(void) {
	return *this;	
}

Vector4 Vector4::operator -(void) {
	Vector4 temp;
	temp[0] = x[0] * (-1);
	temp[1] = x[1] * (-1);
	temp[2] = x[2] * (-1);
	temp[3] = x[3] * (-1);

	return temp;
}

Vector4 operator +(const Vector4& a, const Vector4& b) {
	return Vector4(a.x[0] + b.x[0], a.x[1] + b.x[1], a.x[2] + b.x[2], a.x[3] + b.x[3]);
}

Vector4 operator -(const Vector4& a, const Vector4& b) {
	return Vector4(a.x[0] - b.x[0], a.x[1] - b.x[1], a.x[2] - b.x[2], a.x[3] - b.x[3]);
}

float operator *(const Vector4& a, const Vector4& b) {
	return a.x[0] * b.x[0] + a.x[1] * b.x[1] + a.x[2] * b.x[2] + a.x[3] * b.x[3];
}

Vector4 operator *(float a, const Vector4& b) {
	return Vector4(a * b.x[0], a * b.x[1], a * b.x[2], a * b.x[3]);
}

Vector4 operator *(const Vector4& a, float b) {
	return Vector4(a.x[0] * b, a.x[1] * b, a.x[2] * b, a.x[3] * b);
}

Vector4 operator /(const Vector4& a, float b) {
	return Vector4(a.x[0] / b, a.x[1] / b, a.x[2] / b, a.x[3] / b);
}

Vector4& Vector4::operator +=(const Vector4& a) {
	x[0] += a.x[0];
	x[1] += a.x[1];
	x[2] += a.x[2];
	x[3] += a.x[3];
	return *this;
}

Vector4& Vector4::operator -=(const Vector4& a) {
	x[0] -= a.x[0];
	x[1] -= a.x[1];
	x[2] -= a.x[2];
	x[3] -= a.x[3];
	return *this;
}

Vector4& Vector4::operator *=(float a) {
	x[0] *= a;
	x[1] *= a;
	x[2] *= a;
	x[3] *= a;
	return *this;
}

Vector4& Vector4::operator /=(float a) {
	x[0] /= a;
	x[1] /= a;
	x[2] /= a;
	x[3] /= a;
	return *this;
}

float& Vector4::operator [](unsigned int i) {
	if (i < 4)		
		return x[i];
	else {
		error::Vector::vectoroverlimit();
		return x[0];
	}
}

const float& Vector4::operator [](unsigned int i) const {
	if (i < 4)		
		return x[i];
	else {
		error::Vector::vectoroverlimit();
		return x[0];
	}
}

float Vector4::length(void) const {
	return sqrt(squarelength());
}

float Vector4::squarelength(void) const {
	return x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3];
}

Vector4& Vector4::normalize(void) {

	const float temp(this->length());
	if (temp != 0.0) {	
		x[0] /= temp;
		x[1] /= temp;
		x[2] /= temp;
		x[3] /= temp;
	}
	else
		error::Vector::zerodividenormalize();	

	return *this;

}

/***************************************************************************

	Matrix3

****************************************************************************/

Matrix3::Matrix3(void) : Mat() {}
Matrix3::Matrix3(const float inmat[3][3]) {
	memcpy(Mat, inmat, 9 * sizeof(float));
}
Matrix3::Matrix3(const Vector3& base0, const Vector3& base1, const Vector3& base2) {
	Mat[0][0] = base0[0];	Mat[0][1] = base1[0];	Mat[0][2] = base2[0];
	Mat[1][0] = base0[1];	Mat[1][1] = base1[1];	Mat[1][2] = base2[1];
	Mat[2][0] = base0[2];	Mat[2][1] = base1[2];	Mat[2][2] = base2[2];
}

float* Matrix3::operator [](int i) {
	return Mat[i];
}

const float* Matrix3::operator [](int i) const {
	return Mat[i];
}

Matrix3 operator +(const Matrix3& A, const Matrix3& B) {

	Matrix3 temp;
	for (int i(0); i < 3; ++i) {
		for (int j(0); j < 3; ++j) {
			temp.Mat[i][j] = A.Mat[i][j] + B.Mat[i][j];
		}
	}
	return temp;
}

Matrix3 operator -(const Matrix3& A, const Matrix3& B) {

	Matrix3 temp;
	for (int i(0); i < 3; ++i) {
		for (int j(0); j < 3; ++j) {
			temp.Mat[i][j] = A.Mat[i][j] - B.Mat[i][j];
		}
	}
	return temp;
}

Matrix3 operator *(const Matrix3& A, const Matrix3& B) {

	Matrix3 temp;

	for (int i(0); i < 3; ++i) {
		for (int j(0); j < 3; ++j) {
			for (int k(0); k < 3; ++k) {
				temp.Mat[i][j] += A.Mat[i][k] * B.Mat[k][j];
			}
		}
	}
	return temp;
}

Matrix3 operator *(const Matrix3& A, float f) {

	Matrix3 temp;

	for (int i(0); i < 3; ++i) {
		for (int j(0); j < 3; ++j) {

			temp.Mat[i][j] = A.Mat[i][j] * f;

		}
	}
	return temp;
}

Matrix3 operator *(float f, const Matrix3& A) {

	Matrix3 temp;

	for (int i(0); i < 3; ++i) {
		for (int j(0); j < 3; ++j) {

			temp.Mat[i][j] = A.Mat[i][j] * f;

		}
	}
	return temp;
}

Vector3 operator *(const Matrix3& A, const Vector3& vec) {

	Vector3 temp;

	for (int i(0); i < 3; ++i) {
		for (int j(0); j < 3; ++j) {

			temp[i] += A.Mat[i][j] * vec[j];

		}
	}
	return temp;
}

Matrix3& Matrix3::operator +=(const Matrix3& A) {
	*this = *this + A;
	return *this;
}

Matrix3& Matrix3::operator -=(const Matrix3& A) {
	*this = *this - A;
	return *this;
}

Matrix3& Matrix3::operator *=(const Matrix3& A) {
	*this = *this * A;
	return *this;
}

Matrix3& Matrix3::operator *=(float f) {
	*this = *this * f;
	return *this;
}

Vector3 Matrix3::Read_Xunit(void) const {		
	return Vector3(Mat[0][0], Mat[1][0], Mat[2][0]);
}

Vector3 Matrix3::Read_Yunit(void) const {
	return Vector3(Mat[0][1], Mat[1][1], Mat[2][1]);
}

Vector3 Matrix3::Read_Zunit(void) const {
	return Vector3(Mat[0][2], Mat[1][2], Mat[2][2]);
}

Matrix3 Matrix3::zRotate(float theta) {

	float temp[3][3] = {
		{cos(theta),-sin(theta),0},
		{sin(theta),cos(theta),0},
		{0,0,1}
	};

	return Matrix3(temp);
}

Matrix3& Matrix3::xunitrotate(float theta) {

	Vector3 tempx(Read_Xunit());	
	Vector3 tempy(cos(theta) * Read_Yunit() + sin(theta) * Read_Zunit());
	Vector3 tempz(Vector3::Cross(tempx, tempy));
	tempy = Vector3::Cross(tempz, tempx);

	return (*this = Matrix3(tempx.normalize(), tempy.normalize(), tempz.normalize()));

}

Matrix3& Matrix3::zunitrotate(float theta) {

	Vector3 tempz(Read_Zunit()); //アクターの(相対座標の)z軸方向
	Vector3 tempx(cos(theta) * Read_Xunit() + sin(theta) * Read_Yunit());//z軸周りにtheta回転
	Vector3 tempy(Vector3::Cross(tempz, tempx)); //外積からtheta+90°回転の軸を
	tempx = Vector3::Cross(tempy, tempz); //(こうしないと値がずれていくので)

	return (*this = Matrix3(tempx.normalize(), tempy.normalize(), tempz.normalize()));

}

Matrix3 Matrix3::RottoTarget(Vector3 startaxis, Vector3 targetaxis) {	

	startaxis.normalize();
	targetaxis.normalize();

	Vector3 axis = Vector3::Cross(startaxis, targetaxis);
	axis.normalize();

	float cosine = startaxis * targetaxis;
	float sine = targetaxis * Vector3::Cross(axis, startaxis);

	const float temp[3][3] = { {0,-axis[2],axis[1]},{axis[2],0,-axis[0]},{-axis[1],axis[0],0} };

	Matrix3 Temp = cosine * Matrix3::Identity + sine * Matrix3(temp);

	return Temp;

}

void Matrix3::zunitnorm(Matrix3& matrix) {	

	Vector3 tempz(matrix.Read_Zunit());
	Vector3 tempy(Vector3::Cross(tempz, matrix.Read_Xunit()));
	Vector3 tempx(Vector3::Cross(tempy, tempz));
	tempz.normalize();
	tempy.normalize();
	tempx.normalize();

	matrix = Matrix3(tempx, tempy, tempz);

}

void Matrix3::show(void) {

	for (int i(0); i < 3; ++i) {
		for (int j(0); j < 3; ++j) {
			std::cout << Mat[i][j] << " ";
		}
		std::cout << "\n";
	}

}

const float Mat3_Identity[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
const Matrix3 Matrix3::Identity(Mat3_Identity);

/***************************************************************************

	Matrix4

****************************************************************************/

Matrix4::Matrix4(void) : Mat()
{}
Matrix4::Matrix4(const float inmat[4][4]) {
	memcpy(Mat, inmat, 16 * sizeof(float));
}

float* Matrix4::operator [](int i) {
	return Mat[i];
}

const float* Matrix4::operator [](int i) const {
	return Mat[i];
}


Matrix4 operator +(const Matrix4& A, const Matrix4& B) {

	Matrix4 temp;
	for (int i(0); i < 4; ++i) {
		for (int j(0); j < 4; ++j) {
			temp.Mat[i][j] = A.Mat[i][j] + B.Mat[i][j];
		}
	}
	return temp;
}

Matrix4 operator -(const Matrix4& A, const Matrix4& B) {

	Matrix4 temp;
	for (int i(0); i < 4; ++i) {
		for (int j(0); j < 4; ++j) {
			temp.Mat[i][j] = A.Mat[i][j] - B.Mat[i][j];
		}
	}
	return temp;
}

Matrix4 operator *(const Matrix4& A, const Matrix4& B) {

	Matrix4 temp;

	for (int i(0); i < 4; ++i) {
		for (int j(0); j < 4; ++j) {
			for (int k(0); k < 4; ++k) {
				temp.Mat[i][j] += A.Mat[i][k] * B.Mat[k][j];
			}
		}
	}
	return temp;
}

Vector4 operator *(const Matrix4& A, const Vector4& vec) {

	Vector4 temp;

	for (int i(0); i < 4; ++i) {
		for (int j(0); j < 4; ++j) {

			temp[i] += A.Mat[i][j] * vec[j];

		}
	}

	return temp;
}


Matrix4& Matrix4::operator +=(const Matrix4& A) {
	*this = *this + A;
	return *this;
}

Matrix4& Matrix4::operator -=(const Matrix4& A) {
	*this = *this - A;
	return *this;
}

Matrix4& Matrix4::operator *=(const Matrix4& A) {
	*this = *this * A;
	return *this;
}

Matrix4 Matrix4::MakeGLMat4(const Matrix3& mat, const Vector3& trans, const Vector3& scale/*=Vector3(1.0f,1.0f,1.0f)*/) {

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	  | |       | | s1     |  a || x |   | |					  |x|     |a| |
	  | |   A   | |	  s2   |  b || y | = | | s1[A]1 s2[A]2 s3[A]3 |y| + w |b| |
	  | |	    | |	    s3 |  c || z |   | |				      |z|     |c| |
	  |           0 	      1 || w |   |                 w		    	  |
	 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	float MatGL[4][4] = {	// w=1 をこれは返す
		{scale[0] * mat[0][0],scale[1] * mat[0][1],scale[2] * mat[0][2],trans[0]},
		{scale[0] * mat[1][0],scale[1] * mat[1][1],scale[2] * mat[1][2],trans[1]},
		{scale[0] * mat[2][0],scale[1] * mat[2][1],scale[2] * mat[2][2],trans[2]},
		{0,0,0,1}
	};

	return Matrix4(MatGL);
}

void Matrix4::show(void) const {

	for (int i(0); i < 4; ++i) {
		for (int j(0); j < 4; ++j) {
			std::cout << Mat[i][j] << " ";
		}
		std::cout << "\n";
	}

}

const float Mat4_Identity[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
const Matrix4 Matrix4::Identity(Mat4_Identity);

/***************************************************************************

	GLMatrix

****************************************************************************/

GLMatrix::GLMatrix(void) : unitary(Matrix3::Identity), pos(), scale(1.0f,1.0f,1.0f)
{}

Matrix4 GLMatrix::MakeMat4(void) const {
	return Matrix4::MakeGLMat4(unitary, pos, scale);
}

Matrix3 GLMatrix::getInverse(void) const {	

	const float mat[3][3]{
		unitary[0][0] / scale[0], unitary[1][0] / scale[0], unitary[2][0] / scale[0],
		unitary[0][1] / scale[1], unitary[1][1] / scale[1], unitary[2][1] / scale[1],
		unitary[0][2] / scale[2], unitary[1][2] / scale[2], unitary[2][2] / scale[2]
	};

	return Matrix3(mat);

}

/***************************************************************************

	MATH

****************************************************************************/

const float MATH::Pi(3.1415926f);
const float MATH::epsilon(0.001f);

bool MATH::NearZero(float val) {

	bool result(false);
	if (fabs(val) <= epsilon) result = true;

	return result;

}

float MATH::uipow(float val, unsigned int pow) {

	float result(1.0f);

	for (unsigned int n(0); n < pow; ++n) {
		result *= val;
	}

	return result;

}


