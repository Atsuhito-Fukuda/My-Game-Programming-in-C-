#include "testScript.h"
#include "Camera.h"

/*

	・テスト用のGameScript(の派生クラス)。

	・テストのために仕様を変えたくないのでconst_castであろうが用いる。

*/


testScript::testScript(void){}

void testScript::Load(void) { 
// Debug
	error::Error::Debug_RecordPlace(error::Error::Load);

	Load_Object(
		"Assets/3D/Cube", "cube",
		"Assets/3D/Globe", "globe.global"
	);

// debug
	const_cast<GLMatrix&>(coord("cube")).pos = Vector3(-10.0f, -800.0f, 40.0f);
	const_cast<GLMatrix&>(coord("globe.global")).pos = Vector3(-30.0f, 700.0f, 50.0f);
	const_cast<GLMatrix&>(coord("globe.global")).unitary =
		Matrix3::RottoTarget(Vector3(0, 0, 1), Vector3(0.397f, 0, 0.918f)) * coord("globe.global").unitary;

	Camera(camera::Orbit, "cube");

	/*Load_Object("Assets/fonts/A.gpmesh", "A_w.ui");	
	const_cast<GLMatrix&>(coord("A_w.ui")).pos = Vector3(0.5f, 0.0f, 0.0f);
	const_cast<GLMatrix&>(coord("A_w.ui")).scale = Vector3(0.05f, 0.05f, 1.0f);
	*/

}
void testScript::Progress(void) {
	// Debug
	error::Error::Debug_RecordPlace(error::Error::Manipulate);

	//const_cast<GLMatrix&>(coord("globe.global")).unitary.show();
	//const_cast<GLMatrix&>(coord("cube")).pos.show();

}