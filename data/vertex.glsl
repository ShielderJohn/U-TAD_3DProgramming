#define MAX_BONES 75

// Textures
uniform mat4 MVP;
uniform bool fTexEnabled;
attribute vec3 vPos;
varying vec3 fPos;
attribute vec2 vTex;
varying vec2 fTex;

// Lights
uniform mat4 modelView;
uniform mat4 normalMatrix;
attribute vec3 vNormal;
varying vec3 fNormal;
uniform bool lightingEnabled;

// Shadows
uniform mat4 depthBias;
uniform bool shadowsEnabled;
varying vec3 depthCoord;

// Animation
uniform bool skinned;
uniform mat4 animMatrices[MAX_BONES];
attribute vec4 vboneIndices;
attribute vec4 vboneWeights;

void main() {
    vec4 vPos4 = vec4(vPos, 1);

    // Bone animation
    if (skinned) {
        mat4 boneTransform = mat4(1);

        for (int i = 0; i < 4; ++i) {
            int index = int(vboneIndices[i]);

            if (index > -1) {
                boneTransform += animMatrices[index] * vboneWeights[i];
            }
        }
        vPos4 = boneTransform * vPos4;
    }

    // Vertex transformed position
    vec4 MVPvPos4 = MVP * vPos4;
    gl_Position = MVPvPos4;

    // Texture
    if (fTexEnabled) {
        fTex = vTex;
    }
    else {
        fTex = vec2(0.0, 0.0);
    }

    // Lighting
    if (lightingEnabled) {
        fNormal = vec3(normalMatrix * vec4(vNormal, 0.0));
        fPos = vec3(modelView * vec4(vPos, 1));
    }
    else {
        fNormal = vec3(0.0, 0.0, 0.0);
        fPos = vec3(MVPvPos4);
    }

    // Shadows
    if (shadowsEnabled) {
        depthCoord = vec3(depthBias * vec4(vPos, 1));
    }
    else {
        depthCoord = vec3(0.0, 0.0, 0.0);
    }
}
