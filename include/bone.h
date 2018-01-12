#ifndef UGINE_BONE_H
#define UGINE_BONE_H

#include "types.h"
#include "string.h"

class Bone {
public:
	static Ptr<Bone> Create(const String& name, int parentIndex);
	const String& GetName() const;
	int GetParentIndex() const;

	void SetInversePoseMatrix(const glm::mat4& matrix);
	const glm::mat4& GetInversePoseMatrix();
	glm::mat4 CalculateAnimMatrix(float frame);

	void AddPosition(uint32 frame, const glm::vec3& position);
	void AddRotation(uint32 frame, const glm::quat& rotation);
	void AddScale(uint32 frame, const glm::vec3& scale);

protected:
	Bone(const String& name, uint32 parentIndex);
	~Bone() {}

	glm::vec3 GetPosition(float frame) const;
	glm::quat GetRotation(float frame) const;
	glm::vec3 GetScale(float frame) const;

private:
	String mName;
	int mParentIndex;
	glm::mat4 mInvPoseMatrix;

	Array<uint32> mPositionFrames;
	Array<uint32> mRotationFrames;
	Array<uint32> mScaleFrames;

	Array<glm::vec3> mPositions;
	Array<glm::quat> mRotations;
	Array<glm::vec3> mScales;

friend class Ptr<Bone>;
friend class Ptr<const Bone>;
};

#endif