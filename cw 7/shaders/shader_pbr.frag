#version 430 core

float AMBIENT = 0.25f;
float PI = 3.14159f;

uniform sampler2D depthMap;

uniform vec3 cameraPos;

uniform sampler2D colorTexture;
uniform sampler2D normalSampler; 
uniform vec3 sunDir;
uniform vec3 sunColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float metallic;
uniform float roughness;

uniform float exposition;

in vec3 vecNormal;
in vec3 worldPos;
in vec2 vtc;
in vec2 fragTexCoord;
out vec4 outColor;

in vec3 viewDirTS;
in vec3 lightDirTS;
in vec3 sunDirTS;

float DistributionGGX(vec3 normal, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 normal, vec3 V, vec3 lightDir, float roughness)
{
    float NdotV = max(dot(normal, V), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

vec3 PBRLight(vec3 lightDir, vec3 radiance, vec3 normal, vec3 V, vec3 color)
{
	float diffuse = max(0, dot(normal, lightDir));

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, color, metallic);

    vec3 H = normalize(V + lightDir);    
        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G = GeometrySmith(normal, V, lightDir, roughness);      
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);   
	return (kD * color / PI + specular) * radiance * NdotL;
}

vec3 toneMapping(vec3 color)
{
	float exposure = 0.03;
	vec3 mapped = 1 - exp(-color * exposure);
	return mapped;
}

void main()
{
    //vec3 normal = normalize(vecNormal);
    vec4 N = texture(normalSampler, fragTexCoord);
    vec3 normal = normalize((N*2.0-1.0).xyz);

    vec3 viewDir = normalize(viewDirTS);
    //vec3 viewDir = normalize(cameraPos - worldPos);

	vec3 lightDir = normalize(lightDirTS);
    //vec3 lightDir = normalize(lightPos - worldPos);

    vec4 textureColor = texture2D(colorTexture, vtc);

    float diffuse = max(0, dot(normal, lightDir));
    vec3 distance = lightColor / pow(length(lightPos - worldPos), 2.0) * 300;
    vec3 toneMappedColor = toneMapping(vec3(textureColor) * min(1, AMBIENT + diffuse) * distance);
    //gamma correction
    //toneMappedColor = pow(toneMappedColor, vec3(1.0/2.2)); 

	vec3 ambient = AMBIENT * toneMappedColor;
	vec3 attenuatedLightColor = lightColor / pow(length(lightPos - worldPos), 2);
	vec3 illumination;
	illumination = ambient + PBRLight(lightDir, attenuatedLightColor, normal, viewDir, toneMappedColor);

	//sun
	illumination = illumination + PBRLight(sunDir, sunColor, normal, viewDir, toneMappedColor);

	outColor = vec4(vec3(1.0) - exp(-illumination * exposition), 1);
}
