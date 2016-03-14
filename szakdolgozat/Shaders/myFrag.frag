#version 330

//pipelinebol bejovo
in vec3 gs_pos ;
in vec3 gs_norm;

//kimeno szin
out vec4 _color;

//színtér tul.
uniform vec3 LightDirection;	//Fenyirany
uniform vec3 EyePosition;	//Kamera poz, szempoz
uniform int View;	//2D vagy 3D

uniform float opacity = 1.0f;

//fénytulajdonságok
vec4 LightColor = vec4(0.5f,0.87f,0.1f,1);	//?? Mik legyenek ezek - jó a piros?
float LightPower = 2.0f;
uniform vec3 MaterialDiffuseColor = vec3(1.0f, 0.0f, 0.0f);	//Lehet minden atom másszínu?
uniform vec3 MaterialSpecularColor = vec3(0.6f, 0.5f, 0.2f);
vec3 MaterialAmbientColor  = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;

void main()
{
	// Eye vector (towards the camera)
    vec3 E = normalize( EyePosition );
	// Normal of the computed fragment, in camera space
	vec3 n = normalize( gs_norm );
	 // Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection );

	//------------------------------------------

	float cosTheta = clamp( dot(n,l), 0, 1);
	vec3 dis_vec = normalize( EyePosition - gs_pos );
	float distance = sqrt( dot(dis_vec, dis_vec) );

	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot(E,R), 0, 1);

	//------------------------------------------
	//fragment végso színe
	_color =   View == 1 ? vec4(vec3(1,0,0), 1) :
			 // Ambient : simulates indirect lighting
			 MaterialAmbientColor +
			 // Diffuse : "color" of the object
			 MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
			 // Specular : reflective highlight, like a mirror
			 MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance); ;
	
	_color.a = opacity;
}