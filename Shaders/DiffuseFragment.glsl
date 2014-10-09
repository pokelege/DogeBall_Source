#version 400
in vec4 positions;
in vec2 uvsend;

uniform sampler2D colorTexture;
uniform vec4 color = vec4(1,1,1,1);
out vec4 newColor;

void main()
{
	newColor = color * texture2D(colorTexture, uvsend);
};