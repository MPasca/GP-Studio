#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

// attenuation coeff
float constant = 1.0f; 
float linear = 0.0045f; 
float quadratic = 0.0075f;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);	//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute distance to light 
	float dist = length(lightDir - fPosEye.xyz); 

	//compute attenuation 
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

	//compute ambient light
	//ambient = att * ambientStrength * lightColor;
	ambient *= texture(diffuseTexture, fTexCoords); 

	
	//compute diffuse light
	//diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	diffuse *= texture(diffuseTexture, fTexCoords);

	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColor;
	//specular *= texture(specularTexture, fTexCoords);
}


void main() 
{
	computeLightComponents();

	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f); // orange
	
	ambient *= baseColor;
	diffuse *= baseColor;
	specular *= baseColor;

    //compute final vertex color
	vec3 color = min((ambient + diffuse) + specular, 1.0f);

    fColor = vec4(color, 1.0f);
}
