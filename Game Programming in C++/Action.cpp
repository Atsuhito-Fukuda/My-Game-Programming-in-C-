#include "Error.h"
#include "Action.h"
#include "Object.h"

/*
	[ namespace action ]

	・ここでは「衝突」を扱う (まだまだ構築中)。
	・ActionBaseクラスは衝突を扱うstatic関数を用意。
	・Shapeクラスは各オブジェクトがもつ衝突に関わる情報を保持する。Actionクラスの一部で外部には宣言不明。(pimplイディオム)
	・Actionクラスの非static部分はobject::ObjectBaseクラスが継承する。

*/

namespace action {
	class ActionBase;
	enum Type : unsigned int {	

		/* 
		  [ collisionの型をTypeで指定 ]
			・sortは図形の種類
			・attributeは「球形」とかの属性
		*/

	//Null = 0b0,
		Dot = 0b1, LineSegment = 0b10, Plane = 0b100,
		AABB = 0b1000, max = AABB,

		isotropic = max << 1,	
		vertical = isotropic << 1,	

		Sphere = Dot | isotropic,
		Capsule = LineSegment | isotropic,

		sort = (max << 1) - 1,
		attribute = ~sort

	};
	enum enumpoints : unsigned int {//Shape::pointsの要素番号。
		p1, p2, p3, att1
	};
	enum Max : unsigned int {
		POINTSNUM = enumpoints::att1 + 1
	};
}

class action::ActionBase {

	using Code = char;

	static float sqdistance0(const Shape& dot0, const Shape& dot1);
	static float sqdistance1(const Shape& linesegment, const Shape& dot);
	static float sqdistance2(const Shape& aabb, const Shape& dot);
	static float sqdistance3(const Shape& linesegment0, const Shape& linesegment1);
	static float sqdistance4(const Shape& dot, const Shape& plane);
	static float sqdistance5(const Shape& aabb, const Shape& plane);

public:

	static void FirstAct(Shape& shape);
	static void SecondAct(Shape& shape);
	static void GetBoundary(Shape& shape);
	static void TreeRegister(Shape& shape);

	static bool boundarycolliison(const Shape& shape1, const Shape& shape2);
	static bool collision(Shape& shape0, Shape& shape1);
	static const Vector3 sectionlength;
	static BinaryTree<object::ObjectID> actiontree;	

};

using namespace action;

/***************************************************************************

	Shape

****************************************************************************/

/*

	・コンストラクタより図形の情報を渡す。

*/

namespace adds {	
	enum dot : unsigned int {
		point = enumpoints::p1
	};
	enum lineseg : unsigned int {
		start = enumpoints::p1, end = enumpoints::p2
	};
	enum plane : unsigned int {
		plpoint = enumpoints::p1, normal = enumpoints::p2
	};
	enum aabb : unsigned int {
		min = enumpoints::p1, max = enumpoints::p2
	};
}	

class action::Shape : virtual public object::ObjectCore { //一応virtualにする

	friend class Action;
	friend class ActionBase;

	Type type; //collisionの型
	std::vector<Vector3> points; //各collisionの型に用いる値をいれる
	Vector3 minboundary, maxboundary; //境界AABB
	bool reprocess;		// 未使用
	GLMatrix previous;	// 未使用
	Vector3 velocity;

	BinaryTree<object::ObjectID>::NodeInfo nodeinfo; //BinaryTreeの登録情報

	Shape(const std::string& collisiontype, const std::vector<Vector3>& collidepoints,
		const object::ObjectCore& core);

};

Shape::Shape(const std::string& collisiontype, const std::vector<Vector3>& collidepoints,
	const object::ObjectCore& core) :
	ObjectCore(core),
	type(Type::Dot), velocity(), reprocess(), points(), previous(), minboundary(), maxboundary()
{

	if (collisiontype == "sphere") type = Type::Sphere; //取り敢えずこれで

	else { /*undefiened collision type*/ }

	points = collidepoints;
	if (points.size() != Max::POINTSNUM) error::Action::pointslengtherror();

	//このタイミングでTreeに登録すべきか

}

/***************************************************************************

	ActionBase

****************************************************************************/

/*
  [ ActionBase ]

	・Objectでposの入力等が行われた後にFirstActでは物理的な処理などの処理をする(？)

	・SecondActでは衝突判定のあったobjの物理的な処理を(？)その後Objectで定めたSecondActを

	・First,SecondAct後の位置変更に伴いTreeに再登録すべきだろう

	・各図形の境界AABBからTree(八分木)の配置場所を決め登録する

*/

const Vector3 ActionBase::sectionlength(10.0f, 10.0f, 10.0f);	
BinaryTree<object::ObjectID> ActionBase::actiontree(3);	// Dimension 3

void ActionBase::FirstAct(Shape& shape) {
	shape.previous = shape.mat;
	shape.Mat.pos += shape.velocity /* dt*/;
}

void ActionBase::SecondAct(Shape& shape) {

}

bool ActionBase::boundarycolliison(const Shape& shape1, const Shape& shape2) {
	return !(
		shape1.maxboundary[0] < shape2.minboundary[0] ||
		shape1.maxboundary[1] < shape2.minboundary[1] ||
		shape1.maxboundary[2] < shape2.minboundary[2] ||
		shape2.maxboundary[0] < shape1.minboundary[0] ||
		shape2.maxboundary[1] < shape1.minboundary[1] ||
		shape2.maxboundary[2] < shape1.minboundary[2]
		);
}

bool ActionBase::collision(Shape& shape0, Shape& shape1) {

	bool result(false);	

	switch ((shape0.type | shape1.type) & Type::sort) {

	case Type::Dot | Type::Dot:
	{
		result = sqdistance0(shape0, shape1)
			<= MATH::uipow(shape0.points[enumpoints::att1][0] + shape1.points[enumpoints::att1][0], 2);

		break;	
	}
	case Type::Dot | Type::LineSegment:
	{
		const Shape* dot(&shape0), * linesegment(&shape1);
		if (shape0.type == Type::LineSegment) {
			dot = &shape1;
			linesegment = &shape0;
		}

		result = sqdistance1(*linesegment, *dot)
			<= MATH::uipow(dot->points[att1][0] + linesegment->points[att1][0], 2);

		break;
	}
	case Type::Dot | Type::Plane:
	{
		const Shape* dot(&shape0), * plane(&shape1);
		if (shape0.type == Type::Plane) {
			dot = &shape1;
			plane = &shape0;
		}

		result = sqdistance4(*dot, *plane)
			<= MATH::uipow(dot->points[att1][0] + plane->points[att1][0], 2);

		break;
	}
	case Type::Dot | Type::AABB:	
	{
		const Shape* dot(&shape0), * aabb(&shape1);
		if (shape0.type == Type::AABB) {
			dot = &shape1;
			aabb = &shape0;
		}

		result = sqdistance2(*aabb, *dot)
			<= MATH::uipow(dot->points[att1][0] + aabb->points[att1][0], 2);

		break;
	}
	case Type::LineSegment | Type::LineSegment:
	{	

		result = sqdistance3(shape0, shape1) <=
			MATH::uipow(shape0.points[att1][0] + shape1.points[att1][0], 2);

		break;
	}
	case Type::LineSegment | Type::Plane:
	{
		const Shape* lineseg(&shape0), * plane(&shape1);
		if (shape0.type == Type::Plane) {
			lineseg = &shape1;
			plane = &shape0;
		}
		const Vector3
			pl_point(plane->mat.unitary * plane->points[adds::plane::plpoint] + plane->mat.pos),
			pl_normal(plane->mat.unitary * plane->points[adds::plane::normal]),
			ls_start(lineseg->mat.unitary * lineseg->points[adds::lineseg::start] + lineseg->mat.pos),
			ls_end(lineseg->mat.unitary * lineseg->points[adds::lineseg::end] + lineseg->mat.pos);

		const float	
			sdistance1((ls_start - pl_point) * pl_normal),
			sdistance2((ls_end - pl_point) * pl_normal);

		if (sdistance1 <= 0.0f || sdistance2 <= 0.0f)	
			result = true;
		else
			result = MATH::uipow(std::fmin(sdistance1, sdistance2), 2) <=
			MATH::uipow(lineseg->points[att1][0] + plane->points[att1][0], 2);

		break;
	}
	case Type::Plane | Type::Plane:
	{	
		const Vector3	
			normal1(shape0.mat.unitary * shape0.points[adds::plane::normal]),
			normal2(shape1.mat.unitary * shape1.points[adds::plane::normal]);

		result = true;

		if (MATH::NearZero(Vector3::Cross(normal1, normal2).squarelength())) {

			const Vector3
				point1(shape0.mat.unitary * shape0.points[adds::plane::plpoint] + shape0.mat.pos),
				point2(shape1.mat.unitary * shape1.points[adds::plane::plpoint] + shape1.mat.pos);

			const float sdistance((point2 - point1) * normal1);

			if (sdistance > 0.0f)	
				result = sdistance * sdistance <=
				MATH::uipow(shape0.points[att1][0] + shape1.points[att1][0], 2);

		}

		break;
	}
	case Type::Plane | Type::AABB:
	{
		const Shape* plane(&shape0), * aabb(&shape1);
		if (shape0.type == Type::AABB) {
			plane = &shape1;
			aabb = &shape0;
		}

		result = sqdistance5(*aabb, *plane) <=
			MATH::uipow(plane->points[att1][0] + aabb->points[att1][0], 2);

		break;
	}
	default:
		error::Action::undefiendcollision((shape0.type | shape1.type) & Type::sort);
		break;
	}

	return result;

}

void ActionBase::GetBoundary(Shape& shape) {

	switch (shape.type & Type::sort) {	

	case Type::Dot:	{
		const Vector3 point(shape.mat.unitary * shape.points[adds::dot::point] + shape.mat.pos);	

		shape.minboundary = point - Vector3(1.0f, 1.0f, 1.0f) * shape.points[enumpoints::att1][0];
		shape.maxboundary = point + Vector3(1.0f, 1.0f, 1.0f) * shape.points[enumpoints::att1][0];

		break;	
	}
	case Type::LineSegment:	{
		const Vector3
			start(shape.mat.unitary * shape.points[adds::lineseg::start] + shape.mat.pos),	
			end(shape.mat.unitary * shape.points[adds::lineseg::end] + shape.mat.pos);

		for (int i(0); i < 3; ++i) {

			float fmin(start[i]), fmax(end[i]);	
			if (start[i] > end[i]) {
				fmin = end[i];
				fmax = start[i];
			}

			shape.minboundary[i] = fmin;
			shape.maxboundary[i] = fmax;
		}

		shape.minboundary -= Vector3(1.0f, 1.0f, 1.0f) * shape.points[enumpoints::att1][0];
		shape.maxboundary += Vector3(1.0f, 1.0f, 1.0f) * shape.points[enumpoints::att1][0];

		break;
	}
	case Type::AABB:	{
		const Vector3& aabbmin(shape.points[adds::aabb::min]);
		const Vector3& aabbmax(shape.points[adds::aabb::max]);

		std::vector<Vector3> dots{	
		Vector3(aabbmin),
		Vector3(aabbmin[0],aabbmin[1],aabbmax[2]),
		Vector3(aabbmin[0],aabbmax[1],aabbmin[2]),
		Vector3(aabbmin[0],aabbmax[1],aabbmax[2]),
		Vector3(aabbmax[0],aabbmin[1],aabbmin[2]),
		Vector3(aabbmax[0],aabbmin[1],aabbmax[2]),
		Vector3(aabbmax[0],aabbmax[1],aabbmin[2]),
		Vector3(aabbmax)
		};

		for (auto& dot : dots) {
			dot = shape.mat.unitary * dot + shape.mat.pos;	
		}

		shape.minboundary = dots[0];	shape.maxboundary = dots[0];

		for (int i(1); i < dots.size(); ++i) {

			if (dots[i][0] < shape.minboundary[0])	shape.minboundary[0] = dots[i][0];
			else if (dots[i][0] > shape.maxboundary[0])	shape.maxboundary[0] = dots[i][0];

			if (dots[i][1] < shape.minboundary[1])	shape.minboundary[1] = dots[i][1];
			else if (dots[i][1] > shape.maxboundary[1])	shape.maxboundary[1] = dots[i][1];

			if (dots[i][2] < shape.minboundary[2])	shape.minboundary[2] = dots[i][2];
			else if (dots[i][2] > shape.maxboundary[2])	shape.maxboundary[2] = dots[i][2];

		}


		break;	
	}
	default:
		error::Action::undefiendboundary(shape.type);
		break;

	}

}

void ActionBase::TreeRegister(Shape& shape) {

	GetBoundary(shape); 

	error::Action::rangecheck(shape.minboundary, shape.maxboundary);	

	std::vector<char> mincode(3), maxcode(3);
	
	for (int i(0); i < 3; ++i) {
		if (shape.minboundary[i] < 0.0f)	mincode[i] = -1;
		if (shape.maxboundary[i] < 0.0f)	maxcode[i] = -1;
		mincode[i] += (char)(shape.minboundary[i] / sectionlength[i]);
		maxcode[i] += (char)(shape.maxboundary[i] / sectionlength[i]);
	}

	actiontree.Register(shape.nodeinfo, shape.id, mincode, maxcode);

}

float ActionBase::sqdistance0(const Shape& dot0, const Shape& dot1) {
	return (dot1.mat.unitary * dot1.points[adds::dot::point] + dot1.mat.pos -
		(dot0.mat.unitary * dot0.points[adds::dot::point] + dot0.mat.pos)).squarelength();
}

float ActionBase::sqdistance1(const Shape& linesegment, const Shape& dot) {

	float value(0.0f);

	const Vector3
		ab(
			linesegment.mat.unitary * (linesegment.points[adds::lineseg::end] -
				linesegment.points[adds::lineseg::start])
		),
		ac(
			dot.mat.unitary * dot.points[adds::dot::point] + dot.mat.pos -
			(linesegment.mat.unitary * linesegment.points[adds::lineseg::start] + linesegment.mat.pos)
		);

	if ((ab * ac) < 0.0f)
		value = ac.squarelength();

	else if ((ab * (ac - ab)) > 0.0f)
		value = (ac - ab).squarelength();

	else
		value = (ac - (ac * ab / (ab * ab)) * ab).squarelength();

	return value;

}

float ActionBase::sqdistance2(const Shape& aabb, const Shape& dot) {	

	const Vector3 pos(
		aabb.mat.getInverse() * (dot.mat.unitary * dot.points[adds::dot::point] + dot.mat.pos - aabb.mat.pos)
	);

	float dx(std::fmax(aabb.points[adds::aabb::min][0] - pos[0], 0.0f));
	dx = std::fmax(dx, pos[0] - aabb.points[adds::aabb::max][0]);
	float dy(std::fmax(aabb.points[adds::aabb::min][1] - pos[1], 0.0f));
	dy = std::fmax(dy, pos[1] - aabb.points[adds::aabb::max][1]);
	float dz(std::fmax(aabb.points[adds::aabb::min][2] - pos[2], 0.0f));
	dz = std::fmax(dz, pos[2] - aabb.points[adds::aabb::max][2]);

	return dx * dx + dy * dy + dz * dz;

}

float ActionBase::sqdistance3(const Shape& lineseg0, const Shape& lineseg1) {

	//書籍のコピペ

	const Vector3
		u(lineseg0.mat.unitary * (lineseg0.points[adds::lineseg::end] - lineseg0.points[adds::lineseg::start])),
		v(lineseg1.mat.unitary * (lineseg1.points[adds::lineseg::end] - lineseg1.points[adds::lineseg::start])),
		w(
			lineseg0.mat.unitary * lineseg0.points[adds::lineseg::start] + lineseg0.mat.pos
			- (lineseg1.mat.unitary * lineseg1.points[adds::lineseg::start] + lineseg1.mat.pos)
		);

	const float
		a(u * u), b(u * v), c(v * v), d(u * w), e(v * w), D(a * c - b * b);  // D always >= 0

	float
		sc, sN, sD(D),       // sc = sN / sD, default sD = D >= 0
		tc, tN, tD(D);       // tc = tN / tD, default tD = D >= 0

								   // compute the line parameters of the two closest points
	if (MATH::NearZero(D)) { // the lines are almost parallel
		sN = 0.0f;         // force using point P0 on segment S1
		sD = 1.0f;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0f) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0f;
		// recompute sc for this edge
		if (-d < 0.0f)
			sN = 0.0f;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0f)
			sN = 0.0f;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (MATH::NearZero(sN) ? 0.0f : sN / sD);
	tc = (MATH::NearZero(tN) ? 0.0f : tN / tD);

	// get the difference of the two closest points
	//Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return (w + (sc * u) - (tc * v)).squarelength();   // return the closest distance squared

}

float ActionBase::sqdistance4(const Shape& dot, const Shape& plane) {

	const float sdistance(
		(dot.mat.unitary * dot.points[adds::dot::point] + dot.mat.pos -
			(plane.mat.unitary * plane.points[adds::plane::plpoint] + plane.mat.pos)) *
		(plane.mat.unitary * plane.points[adds::plane::normal])
	);

	float result(0.0f);

	if (sdistance > 0.0f)
		result = sdistance * sdistance;

	return result;

}

float ActionBase::sqdistance5(const Shape& aabb, const Shape& plane) {

	const Vector3& min(aabb.points[adds::aabb::min]), & max(aabb.points[adds::aabb::max]);

	std::vector<Vector3> dots{	
		Vector3(min),
		Vector3(min[0],min[1],max[2]),
		Vector3(min[0],max[1],min[2]),
		Vector3(min[0],max[1],max[2]),
		Vector3(max[0],min[1],min[2]),
		Vector3(max[0],min[1],max[2]),
		Vector3(max[0],max[1],min[2]),
		Vector3(max)
	};

	const Vector3
		invert_pos(
			aabb.mat.getInverse() * (plane.mat.unitary * plane.points[adds::plane::plpoint] + plane.mat.pos - aabb.mat.pos)
		),
		invert_normal(
			aabb.mat.getInverse() * (plane.mat.unitary * plane.points[adds::plane::normal])
		);

	float mindistance(FLT_MAX);	

	for (const auto& dot : dots) {

		const float sdistance((dot - invert_pos) * invert_normal);

		if (sdistance > 0.0f) 
			mindistance = std::fmin(mindistance, sdistance);

		else {
			mindistance = 0.0f;
			break;
		}

	}

	return mindistance * mindistance;

}

/***************************************************************************
	satellite
****************************************************************************/

void satellite::Game::Initialize(void) {

	const Vector3 length(
		ActionBase::sectionlength[0] * 128,
		ActionBase::sectionlength[1] * 128, 
		ActionBase::sectionlength[2] * 128
	);

	if ((length[0] <= 0) || (length[1] <= 0) || (length[2] <= 0))	
		error::Action::nonpositivelength();

	std::cout <<
		"Action Message : Position coordinates range is (" <<
		-length[0] << ", " << length[0] << ")x(" <<
		-length[1] << ", " << length[1] << ")x(" <<
		-length[2] << ", " << length[2] << ").\n";

	error::Action::minrange = -length;	
	error::Action::maxrange = length;

}

/***************************************************************************

	Action 

****************************************************************************/

void Action::FirstAct(void) {
	object.FirstAct(); 
	ActionBase::FirstAct(this->shape); 
	ActionBase::TreeRegister(this->shape);
}
void Action::SecondAct(void) {
	ActionBase::SecondAct(this->shape); 
	object.SecondAct();
	ActionBase::TreeRegister(this->shape); 
}
bool Action::collision(Action& action0, Action& action1) {	

	bool result(false);

	if (ActionBase::boundarycolliison(action0.shape, action1.shape) &&
		ActionBase::collision(action0.shape, action1.shape)) {	

		result = true;
	}

	return result;

}

BinaryTree<object::ObjectID>& Action::actiontree(ActionBase::actiontree);

Action::Action(const std::string& collisiontype, const std::vector<Vector3>& collidepoints) :
	shape(*new Shape(collisiontype, collidepoints, *this)),
	object::ObjectCore()
{}

Action::~Action(void) { delete &shape; } 