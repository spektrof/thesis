#version 330

//pipelinebol bejovo
uniform vec3 COLOR;
uniform float ALPHA = 1.0f;

//kimeno szin
out vec4 _color;

void main()
{
	_color =   vec4(COLOR, ALPHA);
}