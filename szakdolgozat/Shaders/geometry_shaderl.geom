#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

out vec3 gs_norm;	//oldalhoz tartozo normalis
out vec3 gs_pos;	//vilaggal beszorzott vertex

uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vec3 edge1 = gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz;
	vec3 edge2 = gl_in[0].gl_Position.xyz-gl_in[1].gl_Position.xyz;

	vec3 normal = normalize(cross(edge1, edge2)); // worldben levo pontokbol szamolt normalisa

	//---------------------------------------------------------
	
	gs_pos =  gl_in[0].gl_Position.xyz;
	gl_Position = Proj * View * gl_in[0].gl_Position;
	gs_norm =  vec4(normal,0).xyz;

    EmitVertex();

    gs_pos =  gl_in[1].gl_Position.xyz;
	gl_Position = Proj * View * gl_in[1].gl_Position;
	gs_norm =  vec4(normal,0).xyz;	

    EmitVertex();

	gs_pos =  gl_in[2].gl_Position.xyz;
	gl_Position = Proj * View * gl_in[2].gl_Position;
	gs_norm =  vec4(normal,0).xyz;	

    EmitVertex();

    EndPrimitive();
}