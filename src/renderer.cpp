#include "../lib/glew/glew.h"
#include "../include/renderer.h"
#include "../include/vertex.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define USE_SHADERS
#define DEPTH_TEXTURES 15

Ptr<Renderer> Renderer::mInstance = nullptr;

Renderer::Renderer() {
	#ifdef USE_SHADERS
		mDefaultProgram = CreateProgram("data/vertex.glsl", "data/fragment.glsl");
		mDepthProgram = CreateProgram("data/depth_vertex.glsl", "data/depth_fragment.glsl");
		UseProgram(mDefaultProgram);
	#endif
}

void Renderer::Setup3D() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);
}

void Renderer::SetMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& depthBias) {
	#ifdef USE_SHADERS
		glm::mat4 MV(view * model);

		if (mMVPLoc != -1) {
			glUniformMatrix4fv(mMVPLoc, 1, false, glm::value_ptr(projection * MV));
		}

		if (mFragmentShaderModelViewLoc != -1) {
			glUniformMatrix4fv(mFragmentShaderModelViewLoc, 1, false, glm::value_ptr(MV));
		}

		if (mFragmentShaderNormalMatrixLoc != -1) {
			glUniformMatrix4fv(mFragmentShaderNormalMatrixLoc, 1, false, glm::value_ptr(glm::transpose(glm::inverse(MV))));
		}

		if (mVertexShaderDepthBiasLoc != -1) {
			glUniformMatrix4fv(mVertexShaderDepthBiasLoc, 1, false, glm::value_ptr(depthBias));
		}
	#else
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(projection));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(view * model));
	#endif
}

void Renderer::SetViewport(int x, int y, int w, int h) {
	glViewport(x, y, w, h);
	glScissor(x, y, w, h);
}

void Renderer::ClearColorBuffer(const glm::vec3& color) {
	glClearColor(color.r, color.g, color.b, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearDepthBuffer() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

uint32 Renderer::LoadTexture(const String& filename, uint32& width, uint32& height) {
	// Load bitmap
	int w, h, c;
	uint8* buffer = stbi_load(filename.ToCString(), &w, &h, &c, 4);
	if ( !buffer ) return 0;
	width = w;
	height = h;

	// Flip lines
	uint8* newBuffer = (uint8*)malloc(width*height*4);
	int line = 0;
	for ( int y = height-1; y >= 0; y-- ) {
		memcpy(&newBuffer[line*width*4], &buffer[y*width*4], width*4);
		line++;
	}
	free(buffer);
	buffer = newBuffer;

	// Create GL texture
	GLuint handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unload bitmap
	free(buffer);

	return handle;
}

void Renderer::FreeTexture(uint32 tex) {
	glDeleteTextures(1, &tex);
}

void Renderer::SetTexture(uint32 tex) {
	glBindTexture(GL_TEXTURE_2D, tex);
}

void Renderer::EnableTexture(bool enable) {
	glUniform1i(mFragmentShaderTexEnabledLoc, enable);
}

// Light
void Renderer::SetDiffuse(const glm::vec3& color) {
	glUniform3fv(mFragmentShaderDiffuseLoc, 1, glm::value_ptr(color));
}

void Renderer::SetAmbient(const glm::vec3& color) {
	glUniform3fv(mFragmentShaderAmbientLoc, 1, glm::value_ptr(color));
}

void Renderer::SetShininess(uint8 shininess) {
	glUniform1i(mFragmentShaderShinenessLoc, shininess);
}

void Renderer::EnableLighting(bool enable) {
	glUniform1i(mFragmentShaderLightingEnabledLoc, enable);
}

void Renderer::EnableLight(uint32 index, bool enabled) {
	glUniform1i(mFragmentShaderLightEnabledLoc + index, enabled);
}

void Renderer::SetLightData(uint32 index, const glm::vec4& vector, const glm::vec3& color, float attenuation) {
	glUniform4fv(mFragmentShaderLightPosLoc + index, 1, glm::value_ptr(vector));
	glUniform3fv(mFragmentShaderLightColorLoc + index, 1, glm::value_ptr(color));
	glUniform1f(mFragmentShaderLightAttLoc + index, attenuation);
}

uint32 Renderer::CreateBuffer() {
	uint32 buffer;
	glGenBuffers(1, &buffer);
	return buffer;
}

void Renderer::FreeBuffer(uint32 buffer) {
	glDeleteBuffers(1, &buffer);
}

void Renderer::SetVertexBufferData(uint32 vertexBuffer, const void* data, uint32 dataSize) {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::SetIndexBufferData(uint32 indexBuffer, const void* data, uint32 dataSize) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::DrawBuffers(uint32 vertexBuffer, uint32 indexBuffer, uint32 numIndices) {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	#ifdef USE_SHADERS
		if (mVertexShaderPosLoc != -1) {
			glEnableVertexAttribArray(mVertexShaderPosLoc);
			glVertexAttribPointer(mVertexShaderPosLoc, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, mPosition)));
		}

		if (mVertexShaderTexLoc != -1) {
			glEnableVertexAttribArray(mVertexShaderTexLoc);
			glVertexAttribPointer(mVertexShaderTexLoc, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, mTexCoords)));
		}

		if (mVertexShaderNormalLoc != -1) {
			glEnableVertexAttribArray(mVertexShaderNormalLoc);
			glVertexAttribPointer(mVertexShaderNormalLoc, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, mNormals)));
		}

		if (mVertexShaderBoneIndicesLoc != -1)
		{
			glEnableVertexAttribArray(mVertexShaderBoneIndicesLoc);
			glVertexAttribPointer(mVertexShaderBoneIndicesLoc, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, mBoneIndices)));
		}

		if (mVertexShaderBoneWeightsLoc != -1)
		{
			glEnableVertexAttribArray(mVertexShaderBoneWeightsLoc);
			glVertexAttribPointer(mVertexShaderBoneWeightsLoc, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, mBoneWeights)));
		}
	#else
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (const void*)offsetof(Vertex, mPosition));
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (const void*)offsetof(Vertex, mTexCoords));
	#endif

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32 Renderer::CreateProgram(const String& vertex, const String& fragment) {
	int status;
	
	// Vertex shader
	String vertexShaderString = String::Read(vertex);
	const char* vertexShaderCString = vertexShaderString.ToCString();
	uint32 vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderCString, nullptr);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		char errorLog[1024];

		glGetShaderInfoLog(vertexShader, sizeof(errorLog), nullptr, errorLog);
		glDeleteShader(vertexShader);

		mProgramError = errorLog;
		vertexShader = 0;

		return 0;
	}

	// Fragment shader
	String fragmentShaderString = String::Read(fragment);
	const char* fragmentShaderCString = fragmentShaderString.ToCString();
	uint32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderCString, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		char errorLog[1024];

		glGetShaderInfoLog(fragmentShader, sizeof(errorLog), nullptr, errorLog);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		mProgramError += errorLog;
		vertexShader = 0;
		fragmentShader = 0;

		return 0;
	}

	// GPU Program
	uint32 GPUProgram = glCreateProgram();
	glAttachShader(GPUProgram, vertexShader);
	glAttachShader(GPUProgram, fragmentShader);
	glLinkProgram(GPUProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glGetProgramiv(GPUProgram, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		char errorLog[1024];

		glGetProgramInfoLog(GPUProgram, sizeof(errorLog), nullptr, errorLog);
		glDeleteProgram(GPUProgram);

		mProgramError += errorLog;
		vertexShader = 0;
		fragmentShader = 0;
		GPUProgram = 0;

		return 0;
	}

	return GPUProgram;
}

void Renderer::FreeProgram(uint32 program) {
	glDeleteProgram(program);
}

void Renderer::UseProgram(uint32 program) {
	if (program == 0) {
		program = mDefaultProgram;
	}
	glUseProgram(program);

	mMVPLoc = glGetUniformLocation(program, "MVP");
	mVertexShaderPosLoc = glGetAttribLocation(program, "vPos");

	mFragmentShaderTexSamplerLoc = glGetUniformLocation(program, "fTexSampler");
	glUniform1i(mFragmentShaderTexSamplerLoc, 0);
	mFragmentShaderTexEnabledLoc = glGetUniformLocation(program, "fTexEnabled");

	mVertexShaderTexLoc = glGetAttribLocation(program, "vTex");
	mVertexShaderNormalLoc = glGetAttribLocation(program, "vNormal");

	// Light
	mFragmentShaderModelViewLoc = glGetUniformLocation(program, "modelView");
	mFragmentShaderNormalMatrixLoc = glGetUniformLocation(program, "normalMatrix");
	mFragmentShaderLightingEnabledLoc = glGetUniformLocation(program, "lightingEnabled");
	mFragmentShaderLightEnabledLoc = glGetUniformLocation(program, "lightEnabled");
	mFragmentShaderLightPosLoc = glGetUniformLocation(program, "lightPos");
	mFragmentShaderLightColorLoc = glGetUniformLocation(program, "lightColor");
	mFragmentShaderLightAttLoc = glGetUniformLocation(program, "lightAtt");
	mFragmentShaderDiffuseLoc = glGetUniformLocation(program, "diffuse");
	mFragmentShaderAmbientLoc = glGetUniformLocation(program, "ambient");
	mFragmentShaderShinenessLoc = glGetUniformLocation(program, "shininess");

	// Shadows
	mVertexShaderDepthBiasLoc = glGetUniformLocation(program, "depthBias");
	mFragmentShaderDepthSamplerLoc = glGetUniformLocation(program, "depthSampler");
	mFragmentShaderShadowsEnabledLoc = glGetUniformLocation(program, "shadowsEnabled");
	glUniform1i(mFragmentShaderDepthSamplerLoc, DEPTH_TEXTURES);

	// Animation
	mSkinnedLoc = glGetUniformLocation(program, "skinned");
	mAnimMatricesLoc = glGetUniformLocation(program, "animMatrices");
	mVertexShaderBoneIndicesLoc = glGetAttribLocation(program, "vboneIndices");
	mVertexShaderBoneWeightsLoc = glGetAttribLocation(program, "vboneWeights");
}

const String& Renderer::GetProgramError() {
	return mProgramError;
}

uint32 Renderer::GetDepthProgram() {
	return mDepthProgram;
}

uint32 Renderer::CreateTexture(uint32 width, uint32 height, bool isDepth) {
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (isDepth) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

void Renderer::EnableShadows(bool enable) {
	glUniform1i(mFragmentShaderShadowsEnabledLoc, enable);
}

uint32 Renderer::CreateFrameBuffer(uint32 colorTex, uint32 depthTex) {
	uint32 buffer;

	glGenFramebuffers(1, &buffer);

	// Bind the buffer in order to modify parameters
	BindFrameBuffer(buffer);

	// Color texture
	if (colorTex == 0) {
		glDrawBuffer(GL_NONE);
	}
	else {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTex, 0);
	}

	// Depth texture
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0);

	BindFrameBuffer(0);
	return buffer;
}

void Renderer::FreeFrameBuffer(uint32 handle) {
	glDeleteFramebuffers(1, &handle);
}

void Renderer::BindFrameBuffer(uint32 handle) {
	glBindFramebuffer(GL_FRAMEBUFFER, handle);
}

void Renderer::SetDepthTexture(uint32 tex) {
	glActiveTexture(GL_TEXTURE0 + DEPTH_TEXTURES);
	glBindTexture(GL_TEXTURE_2D, tex);
	glActiveTexture(GL_TEXTURE0);
}

void Renderer::SetSkinned(bool skinned) {
	if (mSkinnedLoc != -1) {
		glUniform1i(mSkinnedLoc, skinned);
	}
}

void Renderer::SetAnimMatrices(const Array<glm::mat4>& matrices) {
	if (mAnimMatricesLoc != -1) {
		int bones = 0;

		if (matrices.Size() < MAX_BONES) {
			bones = matrices.Size();
		}
		else {
			bones = MAX_BONES;
		}

		for (int i = 0; i < bones; i++) {
			glUniformMatrix4fv(mAnimMatricesLoc + i, 1, false, glm::value_ptr(matrices[i]));
		}
	}
}