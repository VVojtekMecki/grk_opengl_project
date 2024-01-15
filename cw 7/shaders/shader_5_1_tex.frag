#version 430 core

float AMBIENT = 0.1;

uniform vec3 lightPos;
uniform sampler2D tex;  

in vec3 vecNormal;
in vec3 worldPos;
in vec2 fragTexCoord;
out vec4 outColor;

void main()
{
    vec3 lightDir = normalize(lightPos - worldPos);
    vec3 normal = normalize(vecNormal);
    float diffuse = max(0, dot(normal, lightDir));

    vec4 textureColor = texture(tex, fragTexCoord);
 нового цвета базового пикселя
    vec3 newColor = textureColor.rgb;

    outColor = vec4(newColor * min(1, AMBIENT + diffuse), 1.0);
}
