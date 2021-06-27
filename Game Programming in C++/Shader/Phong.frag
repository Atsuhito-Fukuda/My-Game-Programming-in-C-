#version 330

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;	// こんなもの渡すのならここでma4を作るべきとも思えるが
out vec4 outColor;
uniform sampler2D uTexture;

struct DirectionalLight {

	vec3 mDirection;	// 光の方向
	vec3 mDiffuseColor;	// 拡散反射色
	vec3 mSpecColor;	// 鏡面反射色

};

uniform vec3 uCameraPos;	// uniformはリンクされてるシェーダ間で共有
uniform float uSpecPower;	// 鏡面反射指数
uniform vec3 uAmbientLight;	// 環境光の強さ

uniform DirectionalLight uDirLight;	// 平行光源（今は1つ）

void main(){

	vec3 N = normalize(fragNormal);	// 規格化法線
	vec3 L = normalize(-uDirLight.mDirection);	// 平行光源方向の光
	vec3 V = normalize(uCameraPos - fragWorldPos);	// カメラ方向
	vec3 R = normalize(reflect(-L,N));	// 反射光の計算(Ｎｈは正規化するべし)

	// dotの結果をifなく処理できないかとは思うが(先頭bitとの積とか)

	vec3 Phong = uAmbientLight;
	float dot_NL = dot(N, L);

	if(dot_NL > 0){
		Phong += uDirLight.mDiffuseColor * dot_NL + uDirLight.mSpecColor * pow(max(0,dot(R,V)), uSpecPower);
	}
	outColor = texture(uTexture,fragTexCoord) * vec4(Phong, 1.0f);	// RGBAのvec4を返しているらしい？(ここでの積は成分同士の積)

}

/*
	※ GLSLは最終出力に関係していない変数は消すらしい!! だからPhong = vec3() としてやるとuniformはないとか言われる
*/


/*
	（テクスチャ色）[i] * {(環境光の強さ) + (拡散反射色 * 面法線と入射光の内積) + (鏡面反射色 * 反射光視線成分関係)}[i]  
*/

/*
	いくつものuniformを設定する場合、できる限りそれを使わないobjは値0とかで同じshaderを使いまわせることが望ましいだろうか？
*/

// 未使用のuniformは除去されるので、GetUniformできないので注意すべし