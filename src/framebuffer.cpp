#include "../include/framebuffer.h"

FrameBuffer::FrameBuffer(Ptr<Texture> colorTex, Ptr<Texture> depthTex) {
	mColorTex = colorTex;
	mDepthTex = depthTex;

	if (colorTex == nullptr) {
		mHandle = Renderer::Instance()->CreateFrameBuffer(0, depthTex->GetHandle());
	}
	else {
		mHandle = Renderer::Instance()->CreateFrameBuffer(colorTex->GetHandle(), depthTex->GetHandle());
	}
}

Ptr<FrameBuffer> FrameBuffer::Create(Ptr<Texture> colorTex, Ptr<Texture> depthTex) {
	Ptr<FrameBuffer> frameBuffer = new FrameBuffer(colorTex, depthTex);
	return frameBuffer;
}

uint32 FrameBuffer::GetHandle() const {
	return mHandle;
}

Ptr<const Texture> FrameBuffer::GetColorTexture() const {
	return mColorTex.ConstCast();
}

Ptr<const Texture> FrameBuffer::GetDepthTexture() const {
	return mDepthTex.ConstCast();
}