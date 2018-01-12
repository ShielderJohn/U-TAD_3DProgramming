#include "../include/bone.h"
#include "../include/types.h"

Bone::Bone(const String& name, uint32 parentIndex) {
	mName = name;
	mParentIndex = parentIndex;
}

Ptr<Bone> Bone::Create(const String& name, int parentIndex) {
	Ptr<Bone> bone = new Bone(name, parentIndex);
	return bone;
}

const String& Bone::GetName() const {
	return mName;
}

int Bone::GetParentIndex() const {
	return mParentIndex;
}

void Bone::SetInversePoseMatrix(const glm::mat4& matrix) {
	mInvPoseMatrix = matrix;
}

const glm::mat4& Bone::GetInversePoseMatrix() {
	return mInvPoseMatrix;
}

glm::mat4 Bone::CalculateAnimMatrix(float frame) {
	glm::mat4 animationMatrix;
	glm::quat animationMatrixRotation = GetRotation(frame);
	float rotationAngle = angle(animationMatrixRotation);
	glm::vec3 rotationAxis = glm::axis(animationMatrixRotation);

	// Translate, rotate and scale animation matrix
	animationMatrix = glm::translate(animationMatrix, GetPosition(frame));
	animationMatrix = glm::rotate(animationMatrix, rotationAngle, rotationAxis);
	animationMatrix = glm::scale(animationMatrix, GetScale(frame));

	return animationMatrix;
}

void Bone::AddPosition(uint32 frame, const glm::vec3& position) {
	mPositionFrames.Add(frame);
	mPositions.Add(position);
}

void Bone::AddRotation(uint32 frame, const glm::quat& rotation) {
	mRotationFrames.Add(frame);
	mRotations.Add(rotation);
}

void Bone::AddScale(uint32 frame, const glm::vec3& scale) {
	mScaleFrames.Add(frame);
	mScales.Add(scale);
}

glm::vec3 Bone::GetPosition(float frame) const {
	// Array is empty
	if (mPositionFrames.Size() == 0) {
		return glm::vec3();
	}

	// Frame is equal to the frame inside the array,
	// or frame is smaller than the one inside the array
	for (int i = 0; i < mPositionFrames.Size(); i++) {
		if (frame == mPositionFrames[i]) {
			return mPositions[i];
		}
		else if (frame < mPositionFrames[i]) {
			float alpha = (frame - mPositionFrames[i - 1]) / ((mPositionFrames[i] - mPositionFrames[i - 1]) * 1.0f);
			return glm::mix(mPositions[i - 1], mPositions[i], alpha);
		}
	}

	// Return the last stored frame
	return mPositions[mPositions.Size() - 1];
}

glm::quat Bone::GetRotation(float frame) const {
	// Array is empty
	if (mRotationFrames.Size() == 0) {
		return glm::quat();
	}

	// Frame is equal to the frame inside the array,
	// or frame is smaller than the one inside the array
	for (int i = 0; i < mRotationFrames.Size(); i++) {
		if (frame == mRotationFrames[i]) {
			return mRotations[i];
		}
		else if (frame < mRotationFrames[i]) {
			float alpha = (frame - mRotationFrames[i - 1]) / ((mRotationFrames[i] - mRotationFrames[i - 1]) * 1.0f);
			return glm::slerp(mRotations[i - 1], mRotations[i], alpha);
		}
	}

	// Return the last stored frame
	return mRotations[mRotations.Size() - 1];
}

glm::vec3 Bone::GetScale(float frame) const {
	// Array is empty
	if (mScaleFrames.Size() == 0) {
		return glm::vec3();
	}

	// Frame is equal to the frame inside the array,
	// or frame is smaller than the one inside the array
	for (int i = 0; i < mScaleFrames.Size(); i++) {
		if (frame == mScaleFrames[i]) {
			return mScales[i];
		}
		else if (frame < mScaleFrames[i]) {
			float alpha = (frame - mScaleFrames[i - 1]) / ((mScaleFrames[i] - mScaleFrames[i - 1]) * 1.0f);
			return glm::mix(mScales[i - 1], mScales[i], alpha);
		}
	}

	// Return the last stored frame
	return mScales[mScales.Size() - 1];
}