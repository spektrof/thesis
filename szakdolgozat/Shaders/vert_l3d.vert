#version 330

// VBO bol erkezo valtozok
layout(location = 0) in vec3 vs_in_pos;

//pipeline ban tovabb adando ertekek

//shader kulso parameterei
uniform mat4 World;

void main()
{
	gl_Position = World * vec4( vs_in_pos, 1);
}
