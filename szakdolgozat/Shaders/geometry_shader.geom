layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

out vec3 gs_norm;
out vec3 gs_pos;

void main()
{
	//vec3 edge1 = gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz;
	//vec3 edge2 = gl_in[0].gl_Position.xyz-gl_in[1].gl_Position.xyz;
	
	vec3 edge1 = gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz;
	vec3 edge2 = gl_in[0].gl_Position.xyz-gl_in[1].gl_Position.xyz;

	vec3 normal = normalize(cross(edge1, edge2));

	//---------------------------------------------------------

	gl_Position = gl_in[0].gl_Position ;
	gs_pos = gl_in[0].gl_Position.xyz ;
	gs_norm = normal ;

    EmitVertex();

    gl_Position = gl_in[1].gl_Position ;
  	gs_pos = gl_in[1].gl_Position.xyz ;
	gs_norm = normal ;

    EmitVertex();

	gl_Position = gl_in[2].gl_Position ;
	gs_pos = gl_in[2].gl_Position.xyz ;
	gs_norm = normal ;

    EmitVertex();

    EndPrimitive();
}