#include "../include/mesh.h"
#include "../lib/rapidjson/rapidjson.h"
#include "../lib/rapidjson/document.h"
#include "../include/resourcemanager.h"

Mesh::Mesh(const String& filename) {
	mFilename = filename;
}

Ptr<Mesh> Mesh::Create() {
	return new Mesh("");
}

Ptr<Mesh> Mesh::Create(const String& filename) {
	Ptr<Mesh> mesh = new Mesh(filename);
	String fileString = String::Read(filename);

	rapidjson::Document document;
	document.Parse<0>(const_cast<char*>(fileString.ToCString()));

	if (document.HasParseError() || !document.IsObject() ||
		!document.HasMember("submeshes") || !document["submeshes"].IsArray()) {
		return nullptr;
	}

	// Iterate over submeshes
	for (auto iterator = document["submeshes"].Begin(); iterator != document["submeshes"].End(); ++iterator) {
		Ptr<Submesh> submesh = Submesh::Create(nullptr);

		if ((*iterator).HasMember("texture")) {
			String imagePath = filename.ExtractDir() + "/" + (*iterator)["texture"].GetString();
			Ptr<Texture> texture = ResourceManager::Instance()->LoadTexture(imagePath);
			submesh = Submesh::Create(texture);
		}

		if (!(*iterator).HasMember("indices") || !(*iterator)["indices"].IsArray()) {
			return nullptr;
		}

		// Get indices
		for (auto index = (*iterator)["indices"].Begin(); index != (*iterator)["indices"].End(); index += 3) {
			submesh->AddTriangle((*index).GetInt(), (*(index + 1)).GetInt(), (*(index + 2)).GetInt());
		}

		// Get vertices coords and add them to the submesh
		if ((*iterator).HasMember("coords") && (*iterator)["coords"].IsArray()) {
			Array<Vertex> vertices;
			int counter = 0;

			for (auto coordsIterator = (*iterator)["coords"].Begin(); coordsIterator != (*iterator)["coords"].End(); coordsIterator += 3) {
				float x = static_cast<float>((*(coordsIterator + 0)).GetDouble());
				float y = static_cast<float>((*(coordsIterator + 1)).GetDouble());
				float z = static_cast<float>((*(coordsIterator + 2)).GetDouble());

				Vertex vertex;
				vertex.mPosition = glm::vec3(x, y, z);
				vertices.Add(vertex);
			}

			// Add texture coords to the created vertices
			if ((*iterator).HasMember("texcoords") && (*iterator)["texcoords"].IsArray()) {
				for (auto texCoordsIterator = (*iterator)["texcoords"].Begin(); texCoordsIterator != (*iterator)["texcoords"].End(); texCoordsIterator += 2) {
					float x = static_cast<float>((*(texCoordsIterator + 0)).GetDouble());
					float y = static_cast<float>((*(texCoordsIterator + 1)).GetDouble());

					vertices[counter].mTexCoords = glm::vec2(x, y);
					counter++;
				}
				counter = 0;
			}

			// Add normal vectors to the created vertices
			if ((*iterator).HasMember("normals") && (*iterator)["normals"].IsArray()) {
				for (auto normalsIterator = (*iterator)["normals"].Begin(); normalsIterator != (*iterator)["normals"].End(); normalsIterator += 3) {
					float x = static_cast<float>((*(normalsIterator + 0)).GetDouble());
					float y = static_cast<float>((*(normalsIterator + 1)).GetDouble());
					float z = static_cast<float>((*(normalsIterator + 2)).GetDouble());

					vertices[counter].mNormals = glm::vec3(x, y, z);
					counter++;
				}
				counter = 0;
			}

			// Add bones to vertices
			if ((*iterator).HasMember("bone_indices") && (*iterator)["bone_indices"].IsArray()) {
				for (auto boneIndicesIterator = (*iterator)["bone_indices"].Begin(); boneIndicesIterator != (*iterator)["bone_indices"].End(); boneIndicesIterator += 4) {
					int x = (*(boneIndicesIterator + 0)).GetInt();
					int y = (*(boneIndicesIterator + 1)).GetInt();
					int z = (*(boneIndicesIterator + 2)).GetInt();
					int w = (*(boneIndicesIterator + 3)).GetInt();

					vertices[counter].mBoneIndices = glm::vec4(x, y, z, w);
					counter++;
				}
				counter = 0;

				// Bone weights
				for (auto boneWeightsIterator = (*iterator)["bone_weights"].Begin(); boneWeightsIterator != (*iterator)["bone_weights"].End(); boneWeightsIterator += 4) {
					float x = static_cast<float>((*(boneWeightsIterator + 0)).GetDouble());
					float y = static_cast<float>((*(boneWeightsIterator + 1)).GetDouble());
					float z = static_cast<float>((*(boneWeightsIterator + 2)).GetDouble());
					float w = static_cast<float>((*(boneWeightsIterator + 3)).GetDouble());

					vertices[counter].mBoneWeights = glm::vec4(x, y, z, w);
					counter++;
				}
				counter = 0;
			}

			while (counter < vertices.Size()) {
				submesh->AddVertex(vertices[counter]);
				counter++;
			}
		}

		else {
			return nullptr;
		}

		mesh->AddSubmesh(submesh);
	}

	// Check if member last frame exists
	if (document.HasMember("last_frame") && document["last_frame"].IsInt()) {
		mesh->mLastFrame = document["last_frame"].GetInt();
	}
	else {
		mesh->mLastFrame = 0;
	}

	// Get member bones
	if (document.HasMember("bones") && document["bones"].IsArray()) {
		String boneName = "";
		String parentName = "";
		int parentIndex = 0;

		for (auto boneIterator = document["bones"].Begin(); boneIterator != document["bones"].End(); boneIterator++) {
			// Get bone name
			if ((*boneIterator).HasMember("name") && (*boneIterator)["name"].IsString()) {
				boneName = (*boneIterator)["name"].GetString();
			}
			else {
				return nullptr;
			}

			// Get parent name and index
			if ((*boneIterator).HasMember("parent") && (*boneIterator)["parent"].IsString()) {
				parentName = (*boneIterator)["parent"].GetString();

				if (parentName == "") {
					parentIndex = -1;
				}
				else {
					parentIndex = mesh->GetBoneIndex(parentName);
				}
			}

			Ptr<Bone> bone = Bone::Create(boneName, parentIndex);

			// Inverse pose matrix
			if ((*boneIterator).HasMember("inv_pose") && (*boneIterator)["inv_pose"].IsArray()) {
				glm::vec4 matrix[4];
				int invPoseCounter = 0;

				for (auto inversePoseIterator = (*boneIterator)["inv_pose"].Begin(); inversePoseIterator != (*boneIterator)["inv_pose"].End(); inversePoseIterator += 4) {
					float x = static_cast<float>((*(inversePoseIterator + 0)).GetDouble());
					float y = static_cast<float>((*(inversePoseIterator + 1)).GetDouble());
					float z = static_cast<float>((*(inversePoseIterator + 2)).GetDouble());
					float w = static_cast<float>((*(inversePoseIterator + 3)).GetDouble());

					matrix[invPoseCounter] = glm::vec4(x, y, z, w);
					invPoseCounter++;
				}
				bone->SetInversePoseMatrix(glm::mat4(matrix[0], matrix[1], matrix[2], matrix[3]));
			}
			else {
				return nullptr;
			}

			// Bone positions
			if ((*boneIterator).HasMember("positions") && (*boneIterator)["positions"].IsArray()) {
				for (auto positionIterator = (*boneIterator)["positions"].Begin(); positionIterator != (*boneIterator)["positions"].End(); positionIterator += 4) {
					int frame = static_cast<int>((*(positionIterator + 0)).GetDouble());

					float x = static_cast<float>((*(positionIterator + 1)).GetDouble());
					float y = static_cast<float>((*(positionIterator + 2)).GetDouble());
					float z = static_cast<float>((*(positionIterator + 3)).GetDouble());

					bone->AddPosition(frame, glm::vec3(x, y, z));
				}
			}

			// Bone rotations
			if ((*boneIterator).HasMember("rotations") && (*boneIterator)["rotations"].IsArray()) {
				for (auto rotationIterator = (*boneIterator)["rotations"].Begin(); rotationIterator != (*boneIterator)["rotations"].End(); rotationIterator += 5) {
					int frame = static_cast<int>((*(rotationIterator + 0)).GetDouble());

					float w = static_cast<float>((*(rotationIterator + 1)).GetDouble());
					float x = static_cast<float>((*(rotationIterator + 2)).GetDouble());
					float y = static_cast<float>((*(rotationIterator + 3)).GetDouble());
					float z = static_cast<float>((*(rotationIterator + 4)).GetDouble());

					bone->AddRotation(frame, glm::quat(w, x, y, z));
				}
			}

			// Bone scales
			if ((*boneIterator).HasMember("scales") && (*boneIterator)["scales"].IsArray()) {
				for (auto scaleIterator = (*boneIterator)["scales"].Begin(); scaleIterator != (*boneIterator)["scales"].End(); scaleIterator += 4) {
					int frame = static_cast<int>((*(scaleIterator + 0)).GetDouble());

					float x = static_cast<float>((*(scaleIterator + 1)).GetDouble());
					float y = static_cast<float>((*(scaleIterator + 2)).GetDouble());
					float z = static_cast<float>((*(scaleIterator + 3)).GetDouble());

					bone->AddScale(frame, glm::vec3(x, y, z));
				}
			}
			mesh->AddBone(bone);
		}
	}

	return mesh;
}

const String& Mesh::GetFilename() const {
	return mFilename;
}

void Mesh::AddSubmesh(Ptr<Submesh> submesh) {
	mSubmeshes.Add(submesh);
	submesh->Rebuild();
}

void Mesh::RemoveSubmesh(uint32 i) {
	mSubmeshes.RemoveAt(i);
}

void Mesh::RemoveSubmesh(Ptr<Submesh> submesh) {
	mSubmeshes.Remove(submesh);
}

uint32 Mesh::NumSubmeshes() const {
	return mSubmeshes.Size();
}

Ptr<const Submesh> Mesh::GetSubmesh(uint32 i) const {
	return mSubmeshes[i].ConstCast();
}

Ptr<Submesh> Mesh::GetSubmesh(uint32 i) {
	return mSubmeshes[i];
}

void Mesh::AddBone(const Ptr<Bone>& bone) {
	mBones.Add(bone);
}

const Array<Ptr<Bone>>& Mesh::GetBones() const {
	return mBones;
}

uint32 Mesh::GetLastFrame() const {
	return mLastFrame;
}

void Mesh::SetLastFrame(uint32 lastFrame) {
	mLastFrame = lastFrame;
}

void Mesh::CalculateAnimMatrices(float frame, Array<glm::mat4>& animMatrices) {
	int parentIndex = 0;

	for (int i = 0; i < mBones.Size(); i++) {
		parentIndex = mBones[i]->GetParentIndex();

		if (parentIndex > -1) {
			animMatrices[i] = animMatrices[parentIndex] * mBones[i]->CalculateAnimMatrix(frame);
		}
		else {
			animMatrices[i] = mBones[i]->CalculateAnimMatrix(frame);
		}
	}

	for (int i = 0; i < animMatrices.Size(); i++) {
		animMatrices[i] *= mBones[i]->GetInversePoseMatrix();
	}
}

int Mesh::GetBoneIndex(const String& name) {
	int returnValue = -1;

	for (int i = 0; i < mBones.Size(); i++) {
		if (mBones[i]->GetName() == name) {
			returnValue = i;
		}
	}

	return returnValue;
}

void Mesh::Render() {
	for (int i = 0; i < mSubmeshes.Size(); i++) {
		mSubmeshes[i]->Render();
	}
}