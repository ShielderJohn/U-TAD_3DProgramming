#ifndef UGINE_FRAMEBUFFER_H
#define UGINE_FRAMEBUFFER_H

#include "texture.h"

class FrameBuffer {
public:
	static Ptr<FrameBuffer> Create(Ptr<Texture> colortex, Ptr<Texture> depthtex);
	uint32 GetHandle() const;
	Ptr<const Texture> GetColorTexture() const;
	Ptr<const Texture> GetDepthTexture() const;

protected:
	FrameBuffer(Ptr<Texture> colortex, Ptr<Texture> depthtex);
	~FrameBuffer() {}

private:
	uint32 mHandle;
	Ptr<Texture> mColorTex;
	Ptr<Texture> mDepthTex;

friend class Ptr<FrameBuffer>;
friend class Ptr<const FrameBuffer>;
};

#endif // UGINE_FRAMEBUFFER_H