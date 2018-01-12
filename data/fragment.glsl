#define MAX_LIGHTS 8

// Textures
uniform sampler2D fTexSampler;
uniform bool fTexEnabled;
varying vec3 fPos;
varying vec2 fTex;

// Lights
uniform bool lightingEnabled;
uniform bool lightEnabled[MAX_LIGHTS];
uniform vec4 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform float lightAtt[MAX_LIGHTS];
uniform vec3 diffuse;
uniform vec3 ambient;
uniform int shininess;
varying vec3 fNormal;

// Shadows
uniform bool shadowsEnabled;
uniform sampler2D depthSampler;
varying vec3 depthCoord;

void main() {
    vec3 fColor = diffuse;
    vec4 shadowColor = vec4(1, 1, 1, 1);

    if (lightingEnabled) {
        vec3 combinedDiffuse = ambient;
        vec3 combinedSpecular = vec3(0.0, 0.0, 0.0);
        vec3 N, L;

        for (int i = 0; i < MAX_LIGHTS; i++) {
            if (lightEnabled[i]) {
                N = fNormal;
                L = vec3(lightPos[i]);
                float attenuationFactor = 1.0;

                if (lightPos[i].w == 1.0) {
                    L -= fPos;
                    attenuationFactor = 1.0 / (attenuationFactor + lightAtt[i] * length(L));
                }

                L = normalize(L);
                float dotProductNL = max(0.0, dot(N, L));
                combinedDiffuse += dotProductNL * lightColor[i] * attenuationFactor;

                if (shininess > 0 && dotProductNL > 0.0) {
                    vec3 H = normalize(L - normalize(fPos));
                    float dotProductNH = max(0.0, dot(N, H));
                    combinedSpecular += pow(dotProductNH, float(shininess)) * attenuationFactor;
                }
            }
        }

        fColor = diffuse * combinedDiffuse + combinedSpecular;

        if (shadowsEnabled && texture2D(depthSampler, vec2(depthCoord)).z < depthCoord.z - 0.0009) {
            shadowColor = vec4(ambient, 1);
        }
    }
	
	if (fTexEnabled) {
		gl_FragColor = shadowColor * texture2D(fTexSampler, fTex) * vec4(fColor, 1);
	}
	else {
		gl_FragColor = shadowColor * vec4(fColor, 1);
	}
}
