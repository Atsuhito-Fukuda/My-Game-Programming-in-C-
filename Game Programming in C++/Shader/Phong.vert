#version 330

uniform mat4 uWorldTransform;
uniform mat4 uViewProj;		// �r���[�ˉe�ϊ�

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragWorldPos;

//vec4 uCheck;

void main(){

	vec4 pos = vec4(inPosition,1.0);
	pos = uWorldTransform * pos;

	fragWorldPos = pos.xyz;
	fragNormal = (uWorldTransform * vec4(inNormal, 0.0f)).xyz;


	gl_Position = uViewProj * pos;
	fragTexCoord = inTexCoord;


}


/* 
	GLSL��mat4����v�Z��������������Ƃ́H shaderprog���Ƃɒl��n���̂��낤��
	�������Aworldtrans�̈����ɉ���n���̂������̂����悭�킩���̂őS�̂����߂�܂ł̓X���[���邱�ƂɁB
*/