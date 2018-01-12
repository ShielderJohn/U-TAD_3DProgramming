#ifndef UGINE_MESH_H
#define UGINE_MESH_H

#include "array.h"
#include "string.h"
#include "submesh.h"
#include "bone.h"

class Mesh {
public:
	static Ptr<Mesh> Create();
	static Ptr<Mesh> Create(const String& filename);

	const String& GetFilename() const;

	void AddSubmesh(Ptr<Submesh> submesh);
	void RemoveSubmesh(uint32 i);
	void RemoveSubmesh(Ptr<Submesh> submesh);
	uint32 NumSubmeshes() const;
	Ptr<const Submesh> GetSubmesh(uint32 i) const;
	Ptr<Submesh> GetSubmesh(uint32 i);

	void AddBone(const Ptr<Bone>& bone);
	const Array<Ptr<Bone>>& GetBones() const;
	uint32 GetLastFrame() const;
	void SetLastFrame(uint32 lastFrame);
	void CalculateAnimMatrices(float frame, Array<glm::mat4>& animMatrices);
	int GetBoneIndex(const String& name);

	void Render();
protected:
	Mesh(const String& filename);
	~Mesh() {}
private:
	String mFilename;
	Array<Ptr<Submesh>> mSubmeshes;
	Array<Ptr<Bone>> mBones;
	uint32 mLastFrame;

friend class Ptr<Mesh>;
friend class Ptr<const Mesh>;
};

#endif // UGINE_MESH_H
