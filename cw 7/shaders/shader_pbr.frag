#version 430 core

float AMBIENT = 0.05;
float PI = 3.14159;

uniform sampler2D colorTexture;

uniform float exposition;
uniform float metallic;
uniform float roughness;

uniform vec3 cameraPos;

uniform vec3 sunDir;
uniform vec3 sunColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform bool atmosphereCheck;

uniform float u_time;
uniform float cloudLight;
uniform float cloudIntensity; 
uniform float cloudSpeed; 
uniform float cloudLightness; 

in vec3 vecNormal;
in vec3 worldPos;
in vec2 vtc;

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
	float exposure = 0.06;
	vec3 mapped = 1 - exp(-color * exposure);
	return mapped;
}

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

vec4 noiseColor() {
    vec2 st = vtc.xy;
    vec2 mirroredSt = vec2(1.0 - st.x, st.y);
    
    float timeOffset = u_time * cloudSpeed;
    st.x -= timeOffset;
    mirroredSt.x += timeOffset; // Inverse direction for mirrored effect

    st.x = fract(st.x);
    mirroredSt.x = fract(mirroredSt.x);

    vec3 color = vec3(fbm(st * cloudIntensity));
    vec3 mirroredColor = vec3(fbm(mirroredSt * cloudIntensity));

    float blend = smoothstep(0.45, 0.55, st.x); 

    vec3 finalColor = mix(color, mirroredColor, blend);

    vec4 noiseColor = vec4(finalColor, 1.0);

    return noiseColor;
}

void main() {
    vec3 normal = normalize(vecNormal);
    vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 lightDir = normalize(lightPos - worldPos);

    vec4 textureColor = texture2D(colorTexture, vtc);

    float atmosphereDot = dot(normal, viewDir);
    vec3 atmosphereColor = vec3(0.04, 0.2, 1.0);

    if (atmosphereCheck)
        textureColor = mix(textureColor, vec4(atmosphereColor / atmosphereDot, 1.0), 0.1);

    float diffuse = max(0, dot(normal, lightDir));
    vec3 distance = lightColor / pow(length(lightPos - worldPos), 2.0) * 10;
    vec3 toneMappedColor = toneMapping(vec3(textureColor) * distance);
    //gamma correction
    toneMappedColor = pow(toneMappedColor, vec3(1.0/2.2));

	vec3 ambient = AMBIENT * toneMappedColor;
	vec3 attenuatedLightColor = lightColor / pow(length(lightPos - worldPos), 2);
	vec3 illumination;
	illumination = ambient + PBRLight(lightDir, attenuatedLightColor, normal, viewDir, toneMappedColor);

	//sun
	illumination = illumination + PBRLight(sunDir, sunColor, normal, viewDir, toneMappedColor);

    vec3 pbrColor = vec3(1.0) - exp(-illumination * exposition);

    vec4 noiseColor = noiseColor() * min(1, AMBIENT + cloudLightness * diffuse) * vec4(lightColor, 0.0) / cloudLight;

    vec3 mixedColor = mix(pbrColor.rgb, noiseColor.rgb, noiseColor.r);

    outColor = vec4(mixedColor, 1.0);
}
