#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

out vec3 gs_norm;
out vec3 gs_pos;

uniform mat4 world;
uniform mat4 worldIT;

void main()
{
	//vec3 edge1 = gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz;
	//vec3 edge2 = gl_in[0].gl_Position.xyz-gl_in[1].gl_Position.xyz;
	
	vec3 edge1 = gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz;
	vec3 edge2 = gl_in[0].gl_Position.xyz-gl_in[1].gl_Position.xyz;

	vec3 normal = normalize(cross(edge1, edge2));

	//---------------------------------------------------------

	gl_Position = gl_in[0].gl_Position ;
	gs_pos = (world * vec4(gl_in[0].gl_Position.xyz,1)).xyz ;
	gs_norm = (worldIT * vec4(normal,0)).xyz ;

    EmitVertex();

    gl_Position = gl_in[1].gl_Position ;	
	gs_pos = (world * vec4(gl_in[1].gl_Position.xyz,1)).xyz ;
	gs_norm = (worldIT * vec4(normal,0)).xyz ;

    EmitVertex();

	gl_Position = gl_in[2].gl_Position ;
	gs_pos = (world * vec4(gl_in[2].gl_Position.xyz,1)).xyz ;
	gs_norm = (worldIT * vec4(normal,0)).xyz ;

    EmitVertex();

    EndPrimitive();
}