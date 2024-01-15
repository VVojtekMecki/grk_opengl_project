#version 430 core

float AMBIENT = 0.1;

uniform vec3 lightPos;
uniform sampler2D ship;
uniform sampler2D rust;
uniform sampler2D asteroid;
uniform sampler2D normalSampler; 

in vec3 tangent;         
in vec3 bitangent;  
in vec3 normal;
in vec3 worldPos;
in vec2 fragTexCoord;
in vec3 viewDirTs;
in vec3 lightDirTs;
out vec4 outColor;

void main()
{
    vec3 normal = normalize(normal);
    vec3 tangent = normalize(tangent);
    vec3 bitangent = normalize(bitangent);
    vec4 N = texture(normalSampler, fragTexCoord);

    vec3 normalN = normalize((N*2.0-1.0).xyz);
    float diffuse = max(0, dot(normalN, viewDirTs));
    
    vec4 asteroidColor = texture(asteroid, fragTexCoord);
    vec4 rustColor = texture(rust, fragTexCoord);
    vec4 shipColor = texture(ship, fragTexCoord);  
    vec3 mixedRust = mix(vec3(1.0), shipColor.rgb, rustColor.r);
    vec3 finalColor = mix(mixedRust, mixedRust.rgb, asteroidColor.r);
    //outColor = vec4(finalColor, 1.0) * 5.5;
    outColor = vec4(finalColor * min(1, AMBIENT + diffuse), 1.0);
}
