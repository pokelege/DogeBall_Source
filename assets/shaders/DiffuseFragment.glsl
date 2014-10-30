#version 400
in vec4 positions;
in vec2 uvsend;

uniform sampler2D colorTexture;
uniform vec4 tint = vec4(1,1,1,1);
out vec4 newColor;

void main()
{
	newColor = clamp(tint * texture2D(colorTexture, uvsend), 0, 1);
};