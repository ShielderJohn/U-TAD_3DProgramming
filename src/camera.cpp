#include "../include/camera.h"
#include "../include/renderer.h"
#include "../include/screen.h"

Camera::Camera() {
	mVX = 0;
	mVY = 0;
	mVW = Screen::Instance()->GetWidth();
	mVH = Screen::Instance()->GetHeight();

	mUsesTarget = false;
	mFrameBuffer = nullptr;

	mProjMatrix = glm::perspective(glm::radians(60.f), mVW * 1.f / mVH * 1.f, 0.1f, 1000.f);
}

Ptr<Camera> Camera::Create() {
	return new Camera();
}

int32 Camera::GetViewportX() const {
	return mVX;
}

int32 Camera::GetViewportY() const {
	return mVY;
}

uint16 Camera::GetViewportWidth() const {
	return mVW;
}

uint16 Camera::GetViewportHeight() const {
	return mVH;
}

void Camera::SetViewport(int32 x, int32 y, uint16 w, uint16 h) {
	mVX = x;
	mVY = y;
	mVW = w;
	mVH = h;
}

void Camera::SetProjection(const glm::mat4& proj) {
	mProjMatrix = proj;
}

const glm::mat4& Camera::GetProjection() const {
	return mProjMatrix;
}

const glm::mat4& Camera::GetView() const {
	return mViewMatrix;
}

const glm::vec3& Camera::GetColor() const {
	return mColor;
}

void Camera::SetColor(const glm::vec3& color) {
	mColor = color;
}

bool Camera::GetUsesTarget() const {
	return mUsesTarget;
}

void Camera::SetUsesTarget(bool usesTarget) {
	mUsesTarget = usesTarget;
}

const glm::vec3& Camera::GetTarget() const {
	return mTarget;
}

glm::vec3& Camera::GetTarget() {
	return mTarget;
}

Ptr<const FrameBuffer> Camera::GetFrameBuffer() const {
	return mFrameBuffer.ConstCast();
}

void Camera::SetFrameBuffer(const Ptr<FrameBuffer>& fb) {
	mFrameBuffer = fb;
}

void Camera::Prepare() {
	glm::vec3 upVector(0, 1, 0);
	glm::vec3 forwardPoint = Position() + Rotation() * glm::vec3(0, 0, -1);

	if (!mUsesTarget) {
		mViewMatrix = glm::lookAt(Position(), forwardPoint, upVector);
	}
	else {
		mViewMatrix = glm::lookAt(Position(), mTarget, upVector);
	}

	if (mFrameBuffer != nullptr) {
		Renderer::Instance()->BindFrameBuffer(mFrameBuffer->GetHandle());
	}
	else {
		Renderer::Instance()->BindFrameBuffer(0);
	}

	Renderer::Instance()->SetViewport(mVX, mVY, mVW, mVH);
	Renderer::Instance()->ClearColorBuffer(mColor);
	Renderer::Instance()->ClearDepthBuffer();
}