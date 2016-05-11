#version 330

//pipelinebol bejovo
in vec3 gs_pos ;
in vec3 gs_norm;

//kimeno szin
out vec4 _color;

//színtér tul.
uniform vec3 LightDirection;	//Fenyirany (origobol a fenybe mutato)
uniform vec3 EyePosition;	//Kamera poz, szempoz

uniform float opacity = 1.0f;

//fénytulajdonságok
vec4 LightColor = vec4(0.5f,0.87f,0.1f,1);
float LightPower = 6.0f;
uniform vec3 MaterialDiffuseColor = vec3(1.0f, 0.0f, 0.0f);	
uniform vec3 MaterialSpecularColor = vec3(0.6f, 0.5f, 0.2f);
vec3 MaterialAmbientColor  = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;

void main()
{
	// Kamerapoz fele mutato vektor
    vec3 E = normalize( EyePosition - gs_pos);
	// Normalis, amit a geometry shaderben szamoltunk
	vec3 n = normalize( gs_norm );
	 // Feny iranya, felulettol a fenyig
	vec3 l = normalize(LightDirection - gs_pos);

	//------------------------------------------

	float cosTheta = clamp( dot(n,l), 0, 1);
	vec3 dis_vec = normalize( LightDirection - gs_pos );
	float distance = sqrt( dot(dis_vec, dis_vec) );	//hossz a skalaris szorzat gyoke

	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot(E,R), 0, 1);

	//------------------------------------------
	//fragment végso színe
	_color =
			// Ambient : simulates indirect lighting
			 vec4(MaterialAmbientColor,1) +
			 // Diffuse : "color" of the object
			 vec4(MaterialDiffuseColor,1) * LightColor * LightPower * cosTheta / (distance*distance) +
			 // Specular : reflective highlight, like a mirror
			 vec4(MaterialSpecularColor,1) * LightColor * LightPower * pow(cosAlpha,16) / (distance*distance);
	_color.a = opacity;
}