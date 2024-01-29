#version 430 core

float AMBIENT = 0.1;

uniform vec3 lightPos;
uniform sampler2D earth;
uniform sampler2D clouds;
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
    //vec3 lightDir = normalize(lightPos - worldPos);
    vec3 normal = normalize(normal);
    vec4 N = texture(normalSampler, fragTexCoord);
    vec3 normalN = normalize((N*2.0-1.0).xyz);
    float diffuse = max(0, dot(normalN, viewDirTs));

    

    vec4 cloudColor = texture(clouds, fragTexCoord);
    vec4 earthColor = texture(earth, fragTexCoord);  
    vec3 finalColor = mix(vec3(1.0), earthColor.rgb, cloudColor.r);
    //outColor = vec4(finalColor, 1.0) * 5.5;
    outColor = vec4(finalColor * min(1, AMBIENT + diffuse), 1.0);
}
