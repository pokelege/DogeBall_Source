#version 400
in vec4 positions;
in vec2 uvsend;
in vec4 normals;

uniform vec4 lightColor;
uniform vec3 lightPosition;
//uniform vec3 cameraPosition;

uniform sampler2D colorTexture;
uniform vec4 tint = vec4(1,1,1,1);
out vec4 newColor;

void main()
{
	newColor = clamp(tint * texture2D(colorTexture, uvsend), 0, 1);

	vec3 normalLight = normalize(lightPosition - vec3(positions));
	float diffusePercent = clamp(dot(normalLight, vec3(normals)), 0, 1);

	newColor = clamp(((diffusePercent * lightColor)) * newColor,0,1);
};