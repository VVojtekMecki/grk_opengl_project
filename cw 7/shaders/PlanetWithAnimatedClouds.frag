#version 430 core

float AMBIENT = 0.25;
float PI = 3.14;

uniform sampler2D depthMap;

uniform vec3 cameraPos;


uniform vec3 sunDir;
uniform vec3 sunColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform vec3 spotlightConeDir;
uniform vec3 spotlightPhi;

uniform float metallic;
uniform float roughness;

uniform float exposition;

uniform sampler2D texture1;
uniform sampler2D textureNormal;

uniform float time;


in vec3 vecNormal;
in vec3 worldPos;
in vec2 fragTexCoord;

out vec4 outColor;


in vec3 viewDirTS;
in vec3 lightDirTS;
in vec3 spotlightDirTS;
in vec3 sunDirTS;

in vec3 test;



float DistributionGGX(vec3 normal, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 normal, vec3 V, vec3 lightDir, float roughness){
    float NdotV = max(dot(normal, V), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

vec3 PBRLight(vec3 lightDir, vec3 radiance, vec3 normal, vec3 V, vec3 color){
	float diffuse=max(0,dot(normal,lightDir));

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, color, metallic);

    vec3 H = normalize(V + lightDir);    
        
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, V, lightDir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    float NdotL = max(dot(normal, lightDir), 0.0);   
	return (kD * color / PI + specular) * radiance * NdotL;
}


float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

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


    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

vec4 getNoiseColor() {
    float cloudIntensity = 21.0;

    vec2 st = fragTexCoord.xy;
    vec2 mirroredSt = vec2(1.0 - st.x, st.y);

    float timeOffset = time * 0.01;
    st.x -= timeOffset;
    mirroredSt.x += timeOffset;

    st.x = fract(st.x);
    mirroredSt.x = fract(mirroredSt.x);

    vec3 color = vec3(fbm(st * cloudIntensity));
    vec3 mirroredColor = vec3(fbm(mirroredSt * cloudIntensity));

    float blend = smoothstep(0.45, 0.55, st.x);

    vec3 noiseColor = mix(color, mirroredColor, blend);

    return vec4(noiseColor, 1.);
}



void main()
{
    
    vec4 texture1Color = texture(texture1, fragTexCoord);
    vec4 textureNoise = getNoiseColor();

    vec4 N = texture(textureNormal, fragTexCoord);
    vec3 normal = normalize((N*2.0-1.0).xyz);

    vec3 viewDir = normalize(viewDirTS);

	vec3 lightDir = normalize(lightDirTS);


    vec3 finalColor = mix(texture1Color.rgb, textureNoise.rgb*2, 0.3);
    
    
    float diffuse = max(0, dot(normal, lightDir));
    vec3 color = ((finalColor * min(1, AMBIENT + diffuse))).rgb;


	vec3 ambient = AMBIENT*color;
	vec3 attenuatedlightColor = lightColor/pow(length(lightPos-worldPos),2);
	vec3 ilumination;
	ilumination = ambient+PBRLight(lightDir,attenuatedlightColor,normal,viewDir,color);
	
	//flashlight
	vec3 spotlightDir= normalize(spotlightDirTS);
	

    float angle_atenuation = clamp((dot(-normalize(spotlightPos-worldPos),spotlightConeDir)-0.5)*3,0,1);
	attenuatedlightColor = angle_atenuation*spotlightColor/pow(length(spotlightPos-worldPos),2);
	ilumination=ilumination+PBRLight(spotlightDir,attenuatedlightColor,normal,viewDir,color);

	//sun
	ilumination=ilumination+PBRLight(sunDir,sunColor,normal,viewDir,color);


	outColor = vec4(color - exp(-ilumination*exposition),1);
}
