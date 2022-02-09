#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec4 fragPosLightSpace;

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
uniform sampler2D shadowMap;

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

// tv light parameters
uniform bool isTVon;
uniform vec3 tvPos;
uniform vec3 tvLightDir;

// wallLights
#define NR_POINT_LIGHTS 13 
uniform vec3 pointLights[13];
float pointAmbientStrength =  0.000001f;
float pointSpecularStrength = 0.000002f;

// smoke/fog/whatever
uniform bool isSmokey;

vec3 cameraPosEye = vec3(0.0f);	//in eye coordinates, the viewer is situated at the origin

void computeLightComponents()
{		
	
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
	ambient = att * ambientStrength * lightColor;

	//compute diffuse light
	diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColor;
}

vec3 computePointLight(vec3 lightPointPos){
	vec3 lightDir = normalize(lightPointPos - fPosition);

    // diffuse shading
    float diff = max(dot(fNormal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, fNormal);
    float spec = pow(max(dot((cameraPosEye - fPosEye.xyz), reflectDir), 0.0), shininess);

	// attenuation
    float distance = length(lightPointPos - fPosition);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	vec3 ambientPoint  = pointAmbientStrength * attenuation * vec3(1.0f, 0.96f, 0.713f);
    vec3 diffusePoint  = diff * attenuation * vec3(1.0f, 0.96f, 0.713f);
    vec3 specularPoint = pointSpecularStrength * spec * attenuation * vec3(1.0f, 0.96f, 0.713f);

    return (ambientPoint + diffusePoint + specularPoint);
}

float computeFog()
{
	float fogDensity = 0.08f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow()
{
    // perform perspective divide 
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; 
 
    if (normalizedCoords.z > 1.0f) 
	   return 0.0f;
    // Transform to [0,1] range 
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	// Get closest depth value from light's perspective 
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r; 
	
	// Get depth of current fragment from light's perspective 
    float currentDepth = normalizedCoords.z; 
	
	// Check whether current frag pos is in shadow 
	float bias = 0.005f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f; 
	
	return shadow;
}

void main() 
{
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1){
		discard;
	}

	computeLightComponents();

    float shadow = computeShadow(); 
	
	//compute final vertex color (texture + light for now)
	//vec3 color = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb 
	//				+ (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
	// Phong
	vec3 color = (ambient + diffuse + specular) * texture(diffuseTexture, fTexCoords).rgb;
	vec3 auxColor = (ambient + diffuse + specular) * texture(diffuseTexture, fTexCoords).rgb;
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
         auxColor += computePointLight(pointLights[i]);
	}
	//color += computeTVIllum();

	if(isSmokey){
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = mix(fogColor, vec4(color, colorFromTexture.a), fogFactor);
	}
	else{
		fColor = vec4(color, colorFromTexture.a);
	}
}
