#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

// matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// lighting
uniform vec3 lightDir;
uniform vec3 lightDirColor;
uniform vec3 lightPosColor;


// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

// components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

//pointLight
uniform vec3 PLPosition;
uniform vec3 PLPosition1;
uniform vec3 PLPosition2;
uniform vec3 PLPosition3;
float constant = 1.0f;
float linear = 0.09f;
float quadratic = 0.032f;

vec3 ambientM;
vec3 diffuseM;
vec3 specularM;

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightDirColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightDirColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightDirColor;
	
	ambientM += ambient;
	diffuseM += diffuse;
	specular += specular;
	
	//return (ambient + diffuse + specular);
}

float computeShadow()
{

	//perform perspective divide
	vec3 normalizedCoords= fragPosLightSpace.xyz / fragPosLightSpace.w;

	//tranform from [-1,1] range to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	//get closest depth value from lights perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

	//get depth of current fragment from lights perspective
	float currentDepth = normalizedCoords.z;

	//if the current fragments depth is greater than the value in the depth map, the current fragment is in shadow 
	//else it is illuminated
	//float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	float bias = 0.05f;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	return shadow;
}


void calcPointLight(){

	 //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);
	
	//compute ambient light
    ambient = ambientStrength * lightPosColor;
	
	//compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightPosColor;
	
	//compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	specular = specularStrength * specCoeff * lightPosColor;
	
	//attenuation
	float distance  = length(PLPosition - fPosition);
	float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));    
				
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;  
	
	ambientM += ambient;
	diffuseM += diffuse;
	specular += specular;
	
	//return (ambient + diffuse + specular);
	
	
}

void calcPointLight1(){

	 //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);
	
	//compute ambient light
    ambient = ambientStrength * lightPosColor;
	
	//compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightPosColor;
	
	//compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	specular = specularStrength * specCoeff * lightPosColor;
	
	//attenuation
	float distance  = length(PLPosition1 - fPosition);
	float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));    
				
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;  
	
	ambientM += ambient;
	diffuseM += diffuse;
	specular += specular;
	
	//return (ambient + diffuse + specular);
	
	
}

void calcPointLight2(){

	 //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);
	
	//compute ambient light
    ambient = ambientStrength * lightPosColor;
	
	//compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightPosColor;
	
	//compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	specular = specularStrength * specCoeff * lightPosColor;
	
	//attenuation
	float distance  = length(PLPosition2 - fPosition);
	float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));    
				
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;  
	
	ambientM += ambient;
	diffuseM += diffuse;
	specular += specular;
	
	//return (ambient + diffuse + specular);
	
	
}

void calcPointLight3(){

	 //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);
	
	//compute ambient light
    ambient = ambientStrength * lightPosColor;
	
	//compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightPosColor;
	
	//compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	specular = specularStrength * specCoeff * lightPosColor;
	
	//attenuation
	float distance  = length(PLPosition3 - fPosition);
	float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));    
				
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;  

	ambientM += ambient;
	diffuseM += diffuse;
	specular += specular;
	
	//return (ambient + diffuse + specular);
	
	
}



void main() 
{
    //vec3 dirLight = computeDirLight();
    //vec3 pointLight = calcPointLight();
	//vec3 pointLight1 = calcPointLight1();
	//vec3 pointLight2 = calcPointLight2();
	//vec3 pointLight3 = calcPointLight3();

    float shadow = computeShadow();
    computeDirLight();
    calcPointLight();
	calcPointLight1();
	calcPointLight2();
	calcPointLight3();
	
	ambientM *= texture(diffuseTexture, fTexCoords).rgb;
    diffuseM *= texture(diffuseTexture, fTexCoords).rgb;
    specularM *= texture(specularTexture, fTexCoords).rgb;


    // Combine the lighting contributions
    //vec3 result = dirLight + pointLight + pointLight1 + pointLight2 + pointLight3;

    // Compute final vertex color
    //vec3 color = min(result * texture(diffuseTexture, fTexCoords).rgb, 1.0);
    //float shadows = 0.0f;
	
    //vec3 color = min(result * texture(diffuseTexture, fTexCoords).rgb * (1.0 - shadows), 1.0);
    vec3 color = min((ambientM + (1.0f - shadow)*diffuseM) + (1.0f - shadow)*specularM, 1.0f);
    fColor = vec4(color, 1.0);
}


