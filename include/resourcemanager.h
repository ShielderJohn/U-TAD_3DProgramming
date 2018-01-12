#ifndef UGINE_RESOURCEMANAGER_H
#define UGINE_RESOURCEMANAGER_H

#include "array.h"
#include "mesh.h"
#include "texture.h"

class ResourceManager {
public:
	static Ptr<ResourceManager> Instance();

	Ptr<Mesh> LoadMesh(const String& filename);
	Ptr<Texture> LoadTexture(const String& filename);

	void FreeMeshes();
	void FreeTextures();
	void FreeResources();
protected:
	ResourceManager() {}
	~ResourceManager() { FreeResources(); }
private:
	static Ptr<ResourceManager> mInstance;
	Array<Ptr<Mesh>> mMeshes;
	Array<Ptr<Texture>> mTextures;

	friend class Ptr<ResourceManager>;
};

inline Ptr<ResourceManager> ResourceManager::Instance() {
	if ( mInstance == nullptr ) mInstance = new ResourceManager();
	return mInstance;
}

inline Ptr<Mesh> ResourceManager::LoadMesh(const String& filename) {
	for ( uint32 i = 0; i < mMeshes.Size(); i++ ) {
		if ( mMeshes[i]->GetFilename() == filename ) return mMeshes[i];
	}

	Ptr<Mesh> mesh = Mesh::Create(filename);
	if ( mesh != nullptr ) mMeshes.Add(mesh);
	return mesh;
}

inline Ptr<Texture> ResourceManager::LoadTexture(const String& filename) {
	for ( uint32 i = 0; i < mTextures.Size(); i++ ) {
		if ( mTextures[i]->GetFilename() == filename ) return mTextures[i];
	}

	Ptr<Texture> tex = Texture::Create(filename);
	if ( tex != NULL ) mTextures.Add(tex);
	return tex;
}

inline void ResourceManager::FreeMeshes() {
	mMeshes.Clear();
}

inline void ResourceManager::FreeTextures() {
	mTextures.Clear();
}

inline void ResourceManager::FreeResources() {
	FreeMeshes();
	FreeTextures();
}

#endif // UGINE_RESOURCEMANAGER_H
