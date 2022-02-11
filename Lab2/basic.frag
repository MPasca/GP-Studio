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

vec3 fLightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.3f;
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
vec3 tvPos = vec3(-3.361f, 1.11f, -1.97f);
vec3 tvLightDir = vec3(7.123f, 2.55f, 3.20f);
vec3 tvLightColor = vec3(0.78, 0.8, 0.71);
float cutOff = 0.9;
float outerCutOff = 0.8;

uniform vec3 pointLights[13];

// smoke/fog/whatever
uniform bool isSmokey;
// day cycles
uniform bool isDay;

vec3 cameraPosEye = vec3(0.0f);	//in eye coordinates, the viewer is situated at the origin

void computeLightComponents()
{		
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir - fPosition);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute distance to light 
	float dist = length(lightDir - fPosition); 


	//compute attenuation 
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

	if(!isDay){
		fLightColor = vec3(0.19, 0.17, 0.17);
	}else{
		fLightColor = lightColor;
	}

	//compute ambient light
	ambient = att * ambientStrength * fLightColor;

	//compute diffuse light
	diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * fLightColor;

	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * fLightColor;
}

float computeShadow()
{
    // perform perspective divide 
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; 

    // Transform to [0,1] range 
    normalizedCoords = normalizedCoords * 0.5 + 0.5;

	if(normalizedCoords.z > 1.0f){
		return 0.0f;
	}
	
	// Get depth of current fragment from light's perspective 
    float currentDepth = normalizedCoords.z; 
	
	// Check whether current frag pos is in shadow 
	float bias = 0.001f;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	float shadow = 0.0f;

	for (int x = 0; x <= 7; x++){
		for (int y = 0; y <= 7; y++){
			// Get closest depth value from light's perspective 
    		float closestDepth = texture(shadowMap, normalizedCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > closestDepth  ? 1.0f : 0.0f; 

		}
	}
	
	return shadow/64.0f;
}

vec3 computeTVIllum(){
	vec3 nLightDir = normalize(tvPos - fPosition);

	float theta = dot(nLightDir, normalize(tvLightDir));

	if(theta > cutOff){
		vec3 normalEye = normalize(fNormal); //poate fara matrix

		vec3 nViewDir = normalize(tvPos - tvLightDir);
		vec3 halfVector = normalize(nLightDir + nViewDir);

		float dist = length(tvPos - fPosition);
		float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
		float spec = pow(max(dot(normalEye, halfVector), 0.0f), shininess);

		float epsilon = cutOff - outerCutOff;
		float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

		vec3 ambientPoint = tvLightColor *  intensity * att * vec3(texture(diffuseTexture, fTexCoords));
		vec3 diffusePoint = 				intensity * att * max(dot(normalEye, nLightDir), 0.0f) * vec3(texture(diffuseTexture, fTexCoords));
		vec3 specularPoint = tvLightColor * intensity * att * spec * vec3(texture(specularTexture, fTexCoords));

		return (ambientPoint + diffusePoint + specularPoint) * theta;
	}

	return vec3(0.0f);
}


vec3 ambientPoint;
vec3 diffusePoint;
vec3 specularPoint;

vec3 computePointLight(vec3 lightPointPos, vec3 normal){
	vec3 pLightDir = normalize(lightPointPos - fPosition);

    // diffuse shading
    float diff = max(dot(normal, pLightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-pLightDir, normal);
    float spec = pow(max(dot(cameraPosEye, reflectDir), 0.0f), shininess);

	// attenuation
    float distance = length(lightPointPos - fPosition);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	vec3 pointLightColor = vec3(1.0f, 0.96f, 0.713f);

	ambientPoint  = pointLightColor *  0.1 * texture(diffuseTexture, fTexCoords).rgb;
    diffusePoint  = pointLightColor * diff * texture(diffuseTexture, fTexCoords).rgb;
    specularPoint = pointLightColor * spec * texture(diffuseTexture, fTexCoords).rgb;

	ambientPoint *= attenuation;
	diffusePoint *= attenuation;
	specularPoint *= attenuation;

    return (ambientPoint + diffusePoint + specularPoint);
}


float computeFog()
{
	float fogDensity = 0.08f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

int time = 1;
void main() 
{
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1){
		discard;
	}

	computeLightComponents();

	float shadow = computeShadow(); 

	//compute final vertex color (texture + light for now)
		//modulate with shadow 

	vec3 color = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb 
					+ (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);

	
	vec3 normalEye = normalize(fNormal);	
	//computePointLight(pointLights1, normalEye);
	//color += min((ambientPoint + (1.0f - shadow) * diffusePoint) + (1.0f - shadow) * specularPoint, 1.0f);

	for(int i = 0; i < 13; i++){
		color += computePointLight(pointLights[i], normalEye) * 0.1f;
	}

	if(isTVon){
		color += computeTVIllum();
	}

	fColor = vec4(color, texture(diffuseTexture, fTexCoords).a);

	if(isSmokey){
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = mix(fogColor, vec4(color, colorFromTexture.a), fogFactor);
	}
	else{
		fColor = vec4(color, colorFromTexture.a);
	}

}
