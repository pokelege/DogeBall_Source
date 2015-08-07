#version 400
in vec2 uvsend;

uniform vec4 color = vec4(1,1,1,1);
uniform sampler2D colorTexture;
out vec4 newColor;

void main()
{
	newColor = texture2D(colorTexture, uvsend);
	newColor*=  color;
};