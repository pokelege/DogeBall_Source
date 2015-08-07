#version 400
uniform mat4 viewToProjection;
uniform mat4 worldToView;
uniform mat4 modelToWorld;
in layout(location=0) vec3 position;
in layout(location=3) vec2 uvs;
out vec2 uvsend;
void main()
{

	vec4 perpPos = viewToProjection*worldToView*modelToWorld*vec4(0,0,0,1);
	vec3 perpPos2 = vec3(perpPos) / perpPos.w;
	vec2 perpPos3 = vec2(perpPos2) / perpPos2.z;
	vec4 newPosition = vec4(vec2(mat3(modelToWorld) * position) + perpPos3,0,1);
	gl_Position = newPosition;
	uvsend = uvs;
};