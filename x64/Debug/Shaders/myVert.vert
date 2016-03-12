#version 330

// VBO bol erkezo valtozok
layout(location = 0) in vec3 vs_in_pos;

//pipeline ban tovabb adando ertekek
//semmi

//shader kulso parameterei
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4( vs_in_pos, 1);
}
