#ifndef UGINE_RENDERER_H
#define UGINE_RENDERER_H

#include "smartptr.h"
#include "string.h"
#include "types.h"

#define MAX_LIGHTS 8
#define MAX_BONES 75

class Renderer {
public:
	static Ptr<Renderer> Instance() { if ( mInstance == 0 ) mInstance = new Renderer(); return mInstance; }

	// Setup
	void Setup3D();
	void SetMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& depthBias);
	void SetViewport(int x, int y, int w, int h);

	// Drawing
	void ClearColorBuffer(const glm::vec3& color);
	void ClearDepthBuffer();

	// Texture
	uint32 LoadTexture(const String& filename, uint32& width, uint32& height);
	void FreeTexture(uint32 tex);
	void SetTexture(uint32 tex);
	void EnableTexture(bool enable);

	// Light
	void SetDiffuse(const glm::vec3& color);
	void SetAmbient(const glm::vec3& color);
	void SetShininess(uint8 shininess);
	void EnableLighting(bool enable);
	void EnableLight(uint32 index, bool enabled);
	void SetLightData(uint32 index, const glm::vec4& vector, const glm::vec3& color, float attenuation);

	// VBO
	uint32 CreateBuffer();
	void FreeBuffer(uint32 buffer);
	void SetVertexBufferData(uint32 vertexBuffer, const void* data, uint32 dataSize);
	void SetIndexBufferData(uint32 indexBuffer, const void* data, uint32 dataSize);
	void DrawBuffers(uint32 vertexBuffer, uint32 indexBuffer, uint32 numIndices);

	// Shaders
	uint32 CreateProgram(const String& vertex, const String& fragment);
	void FreeProgram(uint32 program);
	void UseProgram(uint32 program);
	const String& GetProgramError();

	// Shadows
	uint32 GetDepthProgram();
	uint32 CreateTexture(uint32 width, uint32 height, bool isDepth);
	void EnableShadows(bool enable);

	// FBO
	uint32 CreateFrameBuffer(uint32 colorTex, uint32 depthTex);
	void FreeFrameBuffer(uint32 handle);
	void BindFrameBuffer(uint32 handle);
	void SetDepthTexture(uint32 tex);

	// Animation
	void SetSkinned(bool skinned);
	void SetAnimMatrices(const Array<glm::mat4>& matrices);

protected:
	Renderer();
	virtual ~Renderer() {}
private:
	static Ptr<Renderer> mInstance;
	int mMVPLoc;
	int mVertexShaderPosLoc;
	int mVertexShaderTexLoc;
	int mVertexShaderNormalLoc;

	// Lights
	int mFragmentShaderTexSamplerLoc;
	int mFragmentShaderTexEnabledLoc;
	int mFragmentShaderModelViewLoc;
	int mFragmentShaderNormalMatrixLoc;
	int mFragmentShaderLightingEnabledLoc;
	int mFragmentShaderLightEnabledLoc;
	int mFragmentShaderLightPosLoc;
	int mFragmentShaderLightColorLoc;
	int mFragmentShaderLightAttLoc;
	int mFragmentShaderDiffuseLoc;
	int mFragmentShaderAmbientLoc;
	int mFragmentShaderShinenessLoc;

	// Shadows
	int mVertexShaderDepthBiasLoc;
	int mFragmentShaderDepthSamplerLoc;
	int mFragmentShaderShadowsEnabledLoc;

	// Animation
	int mSkinnedLoc;
	int mAnimMatricesLoc;
	int mVertexShaderBoneIndicesLoc;
	int mVertexShaderBoneWeightsLoc;

	uint32 mDefaultProgram;
	uint32 mDepthProgram;
	String mProgramError;
friend class Ptr<Renderer>;
friend class Ptr<const Renderer>;
};

#endif // UGINE_RENDERER_H
