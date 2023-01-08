#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	// Ambient lighting
	// The ambient lighting is like the general light, 
	// it does not have an origin or direction
	
	float ambientStrength = 1.0f;
	vec3 ambient = ambientStrength * lightColor;

	
	// Diffuse lighting
	// The diffuse lighting it takes into account the direction of the light
	// and the normals, like the part of the object that is near to the light 
	// is more brighter. An example is the Phong Shading
	// The light here reflects in many directions and the angles different from 
	// the incident light angle
	
	vec3 norm = normalize(Normal);		// technically Normalizing our Normal
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	// Specular lighting
	// In specular lighting the light reflects in the same angle as the 
	// Incident striking light, causing the relection to be as a mirror

	float specularStrength = 0.5f;
	vec3 viewDir= normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = specularStrength * spec * lightColor;


	// Now the final result
	vec3 result = (ambient + diffuse + specular) * objectColor;
	color = vec4(result, 1.0f);

	//color = vec4(lightColor * objectColor, 1.0f);
}