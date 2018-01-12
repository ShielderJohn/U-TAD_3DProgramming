#ifndef UGINE_MODEL_H
#define UGINE_MODEL_H

#include "entity.h"
#include "array.h"

class Mesh;

class Model : public Entity {
public:
	static Ptr<Model> Create(Ptr<Mesh> mesh);

	Ptr<Mesh> GetMesh();

	void Animate(bool animate);
	int GetFPS() const;
	void SetFPS(int fps);
	float GetCurrentFrame() const;
	void SetCurrentFrame(float frame);

	virtual void Update(float elapsed);
	virtual void Render();
protected:
	Model(Ptr<Mesh> mesh);
	virtual ~Model() {}
private:
	Ptr<Mesh> mMesh;
	bool mAnimating;
	int mFPS;
	float mCurrentFrame;
	Array<glm::mat4> mAnimMatrices;
friend class Ptr<Model>;
friend class Ptr<const Model>;
};

#endif // UGINE_MODEL_H
