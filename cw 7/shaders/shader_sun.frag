#version 430 core

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 cameraPos;

uniform float u_time;

in vec3 vecNormal;
in vec3 worldPos;
in vec2 vtc;

out vec4 outColor;


vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float snoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
        + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

uniform sampler2D colorTexture;

vec3 toneMapping(vec3 color)
{
	float exposure = 0.06;
	vec3 mapped = 1 - exp(-color * exposure);
	return mapped;
}

void main()
{
    //noise
    vec2 st = vtc.xy;
    vec2 mirroredSt = vec2(1.0 - st.x, st.y);

    st.x = fract(st.x);
    mirroredSt.x = fract(mirroredSt.x);

    vec3 red = vec3(0.225,0.000,0.000); // Pure red
    vec3 orange = vec3(0.775,0.291,0.134); // Orange

    vec2 pos = vec2(st*5.);
    vec2 posMirrored = vec2(mirroredSt*5.);
    float DF = 0.0;
    float DFMirror = 0.0;

    vec2 vel = vec2(u_time*.4);
    DF += snoise(pos+vel)*.25+.25;

    float noise = snoise(pos*vec2(cos(u_time*0.15),sin(u_time*0.1))*0.1)*3.1415;
    vel = vec2(cos(noise),sin(noise));
    DF += snoise(pos+vel)*.25+.25;

    vec3 color = mix(red, orange, smoothstep(.0, 1.0, fract(DF)));

    //mirrored
    vec2 velMirror = vec2(u_time*.4);
    DFMirror += snoise(posMirrored+velMirror)*.25+.25;

    float noiseMirror = snoise(posMirrored*vec2(cos(u_time*0.15),sin(u_time*0.1))*0.1)*3.1415;
    velMirror = vec2(cos(noiseMirror),sin(noiseMirror));
    DFMirror += snoise(posMirrored+velMirror)*.25+.25;

    vec3 colorMirrored = mix(red, orange, smoothstep(.0, 1.0, fract(DFMirror)));

    float blend = smoothstep(0.45, 0.55, st.x); 

    vec3 finalColor = mix(color, colorMirrored, blend);

    vec4 noiseColor = vec4(finalColor,1.0);


	vec3 normal = normalize(vecNormal);
    vec3 viewDir = normalize(cameraPos - worldPos);

	vec4 textureColor = texture2D(colorTexture, vtc);

	vec3 distance = lightColor / pow(length(lightPos - worldPos), 2.0) * 20;
    vec3 toneMappedColor = toneMapping(vec3(textureColor) * distance);
    toneMappedColor = pow(toneMappedColor, vec3(1.0/2.2));

	vec4 texColor = vec4(toneMappedColor * lightColor * 0.22f, 1.0);

    outColor = mix(texColor, noiseColor, 0.8);
}
