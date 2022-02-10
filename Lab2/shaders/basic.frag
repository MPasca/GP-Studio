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
uniform vec3 tvPos;
uniform vec3 tvLightDir;

vec3 pointLights1 = vec3(4.3627f,  5.8728f,  3.39823f);
vec3 pointLights2 = vec3(3.78756f, 5.68063f, 2.98536f);
vec3 pointLights3 = vec3(4.75649f,  5.71798f, 2.96634f);
vec3 pointLights4 = vec3(4.4341f,  5.67128f, 2.72644f);
vec3 pointLights5 = vec3(3.75614, 5.58234, 2.30304);
vec3 pointLights6 = vec3(4.10425, 5.58855, 2.26019);
vec3 pointLights7 = vec3(4.53368, 5.61986, 1.93849);
vec3 pointLights8 = vec3(3.75809, 5.53615, 1.60692);
vec3 pointLights9 = vec3(4.24105, 5.45638, 1.63722);
vec3 pointLights10 = vec3(4.78422, 5.49807, 1.39514);
vec3 pointLights11 = vec3(4.54942, 5.33139, 1.01812);
vec3 pointLights12 = vec3(3.73847, 5.29568, 0.904004);
vec3 pointLights13 = vec3(4.22911, 5.37028, 0.42218);

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
	vec3 lightDirN = normalize(lightDir - fPosition);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute distance to light 
	float dist = length(lightDir - fPosition); 

	//compute attenuation 
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

	//compute ambient light
	ambient = att * ambientStrength * lightColor;

	//compute diffuse light
	diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColor;
}

vec3 computePointLight(vec3 lightPointPos){
	vec3 normalEye = normalize(fNormal);	

	vec3 lightDir = normalize(lightPointPos - fPosition);

    // diffuse shading
    float diff = max(dot(normalEye, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, fNormal);
    float spec = pow(max(dot((cameraPosEye - fPosEye.xyz), reflectDir), 0.0), shininess);

	// attenuation
    float distance = length(lightPointPos - fPosition);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	vec3 ambientPoint  = attenuation * pointAmbientStrength * vec3(1.0f, 0.96f, 0.713f);
    vec3 diffusePoint  = attenuation * diff * vec3(1.0f, 0.96f, 0.713f);
    vec3 specularPoint = attenuation * pointSpecularStrength * spec * vec3(1.0f, 0.96f, 0.713f);

    return (ambientPoint + diffusePoint + specularPoint);
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
	
	// Get closest depth value from light's perspective 
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r; 
	
	// Get depth of current fragment from light's perspective 
    float currentDepth = normalizedCoords.z; 
	
	// Check whether current frag pos is in shadow 
	float bias = 0.00006f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f; 
	
	return shadow;
}

float computeFog()
{
	float fogDensity = 0.08f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
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
		//modulate with shadow 

	vec3 color = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb 
					+ (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);

	
	//color += computePointLight(pointLights1);


	//color += computeTVIllum();

	fColor = vec4(color, 1.0f);

	if(isSmokey){
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = mix(fogColor, vec4(color, colorFromTexture.a), fogFactor);
	}
	else{
		fColor = vec4(color, colorFromTexture.a);
	}


	/////EXPERIMENTAL CODE - TIBI

	
	// vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; 
	// float closestDepth;
    // // Transform to [0,1] range 
    // normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	// if (normalizedCoords.z > 1.0f) 
	// 	closestDepth = 0.0f;
	// else
	// 	closestDepth = texture(shadowMap, normalizedCoords.xy).r; 

	// fColor = vec4(vec3(closestDepth), 1.0f);

}
