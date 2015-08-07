#version 400

in layout(location=0) vec3 position;
in layout(location=3) vec2 uvs;
out vec2 uvsend;
void main()
{
	vec4 newPosition = vec4(position,1);
	gl_Position = newPosition;
	uvsend = uvs;
};