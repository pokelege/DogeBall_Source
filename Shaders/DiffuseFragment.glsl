#version 400
in vec4 positions;
in vec2 uvsend;

uniform sampler2D tex;

out vec4 newColor;

void main()
{
	newColor = vec4(1,1,1,1);
};