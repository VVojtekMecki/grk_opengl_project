#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;  
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 lightPos;  
uniform vec3 cameraPos;

out vec3 tangent;        
out vec3 bitangent;   
out vec3 normal;

out vec3 worldPos;
out vec2 fragTexCoord;  

out vec3 viewDirTs;
out vec3 lightDirTs;

void main()
{
    worldPos = (modelMatrix * vec4(vertexPosition, 1)).xyz;
    normal = (modelMatrix * vec4(vertexNormal, 0)).xyz;
    tangent = (modelMatrix * vec4(vertexTangent, 0)).xyz;
    bitangent = (modelMatrix * vec4(vertexBitangent, 0)).xyz;
    mat3 TBN = transpose(mat3(tangent, bitangent, normal));
    fragTexCoord = vec2(vertexTexCoord.x, vertexTexCoord.y);

    viewDirTs = normalize(TBN * (cameraPos - worldPos));
    lightDirTs = normalize(TBN * (lightPos - worldPos));

    gl_Position = transformation * vec4(vertexPosition, 1.0);
}
