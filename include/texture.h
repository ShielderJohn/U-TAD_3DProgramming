#ifndef UGINE_TEXTURE_H
#define UGINE_TEXTURE_H

#include "renderer.h"
#include "smartptr.h"
#include "string.h"

class Texture {
public:
	static Ptr<Texture> Create(const String& filename);
	static Ptr<Texture> Create(uint32 width, uint32 height, bool isDepth);
	
	const String& GetFilename() const;
	uint32 GetHandle() const;
	uint32 GetWidth() const;
	uint32 GetHeight() const;

	bool IsDepth() const;

protected:
	Texture(const String& filename);
	~Texture();

private:
	String mFilename;
	uint32 mHandle;
	uint32 mWidth;
	uint32 mHeight;
	bool mIsDepth;

friend class Ptr<Texture>;
friend class Ptr<const Texture>;
};

#endif // UGINE_TEXTURE_H
