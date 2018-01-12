#ifndef UGINE_CAMERA_H
#define UGINE_CAMERA_H

#include "entity.h"
#include "framebuffer.h"

class Camera : public Entity {
public:
	static Ptr<Camera> Create();

	int32 GetViewportX() const;
	int32 GetViewportY() const;
	uint16 GetViewportWidth() const;
	uint16 GetViewportHeight() const;
	void SetViewport(int32 x, int32 y, uint16 w, uint16 h);

	void SetProjection(const glm::mat4& proj);
	const glm::mat4& GetProjection() const;

	const glm::mat4& GetView() const;

	const glm::vec3& GetColor() const;
	void SetColor(const glm::vec3& color);

	bool GetUsesTarget() const;
	void SetUsesTarget(bool usesTarget);

	const glm::vec3& GetTarget() const;
	glm::vec3& GetTarget();

	Ptr<const FrameBuffer> GetFrameBuffer() const;
	void SetFrameBuffer(const Ptr<FrameBuffer>& fb);

	void Prepare();
	virtual void Render() {}
protected:
	Camera();
	virtual ~Camera() {}
private:
	glm::mat4 mProjMatrix;
	glm::mat4 mViewMatrix;
	int32 mVX, mVY;
	uint16 mVW, mVH;
	glm::vec3 mColor;
	bool mUsesTarget;
	glm::vec3 mTarget;
	Ptr<FrameBuffer> mFrameBuffer;
	friend class Ptr<Camera>;
	friend class Ptr<const Camera>;
};

#endif // UGINE_CAMERA_H