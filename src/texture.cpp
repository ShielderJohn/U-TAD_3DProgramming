#include "../include/texture.h"

Texture::Texture(const String& filename) {
	mFilename = filename;
	mHandle = Renderer::Instance()->LoadTexture(mFilename, mWidth, mHeight);
}

Texture::~Texture() {
	Renderer::Instance()->FreeTexture(mHandle);
}

Ptr<Texture> Texture::Create(const String& filename) {
	Ptr<Texture> texture = new Texture(filename);
	
	if (texture->mHandle == 0) {
		return nullptr;
	}
	else {
		return texture;
	}
}

Ptr<Texture> Texture::Create(uint32 width, uint32 height, bool isDepth) {
	Ptr<Texture> texture = new Texture("");

	texture->mWidth = width;
	texture->mHeight = height;
	texture->mHandle = Renderer::Instance()->CreateTexture(width, height, isDepth);
	texture->mIsDepth = isDepth;

	return texture;
}

const String& Texture::GetFilename() const {
	return mFilename;
}

uint32 Texture::GetHandle() const {
	return mHandle;
}

uint32 Texture::GetWidth() const {
	return mWidth;
}

uint32 Texture::GetHeight() const {
	return mHeight;
}

bool Texture::IsDepth() const {
	return mIsDepth;
}