#version 330

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;	// ����Ȃ��̓n���̂Ȃ炱����ma4�����ׂ��Ƃ��v���邪
out vec4 outColor;
uniform sampler2D uTexture;

struct DirectionalLight {

	vec3 mDirection;	// ���̕���
	vec3 mDiffuseColor;	// �g�U���ːF
	vec3 mSpecColor;	// ���ʔ��ːF

};

uniform vec3 uCameraPos;	// uniform�̓����N����Ă�V�F�[�_�Ԃŋ��L
uniform float uSpecPower;	// ���ʔ��ˎw��
uniform vec3 uAmbientLight;	// �����̋���

uniform DirectionalLight uDirLight;	// ���s�����i����1�j

void main(){

	vec3 N = normalize(fragNormal);	// �K�i���@��
	vec3 L = normalize(-uDirLight.mDirection);	// ���s���������̌�
	vec3 V = normalize(uCameraPos - fragWorldPos);	// �J��������
	vec3 R = normalize(reflect(-L,N));	// ���ˌ��̌v�Z(�m���͐��K������ׂ�)

	// dot�̌��ʂ�if�Ȃ������ł��Ȃ����Ƃ͎v����(�擪bit�Ƃ̐ςƂ�)

	vec3 Phong = uAmbientLight;
	float dot_NL = dot(N, L);

	if(dot_NL > 0){
		Phong += uDirLight.mDiffuseColor * dot_NL + uDirLight.mSpecColor * pow(max(0,dot(R,V)), uSpecPower);
	}
	outColor = texture(uTexture,fragTexCoord) * vec4(Phong, 1.0f);	// RGBA��vec4��Ԃ��Ă���炵���H(�����ł̐ς͐������m�̐�)

}

/*
	�� GLSL�͍ŏI�o�͂Ɋ֌W���Ă��Ȃ��ϐ��͏����炵��!! ������Phong = vec3() �Ƃ��Ă���uniform�͂Ȃ��Ƃ�������
*/


/*
	�i�e�N�X�`���F�j[i] * {(�����̋���) + (�g�U���ːF * �ʖ@���Ɠ��ˌ��̓���) + (���ʔ��ːF * ���ˌ����������֌W)}[i]  
*/

/*
	��������uniform��ݒ肷��ꍇ�A�ł�����肻����g��Ȃ�obj�͒l0�Ƃ��œ���shader���g���܂킹�邱�Ƃ��]�܂������낤���H
*/

// ���g�p��uniform�͏��������̂ŁAGetUniform�ł��Ȃ��̂Œ��ӂ��ׂ�