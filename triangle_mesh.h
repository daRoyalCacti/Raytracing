#pragma once

#include "triangle.h"

#include <vector>
#include <string>

//for loading a model

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct triangle_mesh : public hittable {
	const shared_ptr<bvh_node> tris;

	triangle_mesh() = delete;
	triangle_mesh(hittable_list &triangles, const double time0, const double time1) : tris(make_shared<bvh_node>(triangles,time0, time1)) {}


	inline bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) override {
		return tris->hit(r, t_min, t_max, rec);
	}

	inline bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		return tris->bounding_box(time0, time1, output_box);	
	}
};




void processMesh(aiMesh *mesh, const aiScene *scene, std::vector<double> &vertices, std::vector<unsigned> &indices, std::vector<double> &uvs, std::vector<std::string> &tex_paths, std::vector<double> &norms) {

	for (unsigned i = 0; i < mesh->mNumVertices; i++) {
		//process vertex positions, normals and texture coordinates

		vertices.push_back(mesh->mVertices[i].x);
		vertices.push_back(mesh->mVertices[i].y);
		vertices.push_back(mesh->mVertices[i].z);

		norms.push_back(mesh->mNormals[i].x);
		norms.push_back(mesh->mNormals[i].y);
		norms.push_back(mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0]) {	//does the mesh contain texture coords
			uvs.push_back(mesh->mTextureCoords[0][i].x);
			uvs.push_back(mesh->mTextureCoords[0][i].y);
		} else {
			uvs.push_back(0.0f);
			uvs.push_back(0.0f);
		}
	}

	for (unsigned i = 0; i < mesh->mNumFaces; i++) {
		//assimp defines eah mesh as having an array of faces
		// - aiProcess_Triangulate means these faces are always triangles
		//Iterate over all the faces and store the face's indices

		const aiFace face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		//currently only processes diffuse textures

		//retrieve the aiMaterial object from the scene
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		//saying that want all diffuse textures
		const aiTextureType type = aiTextureType_DIFFUSE;

		//actually loading the textures
		for (unsigned i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i , &str);
			std::string path = str.C_Str();

			bool already_loaded = false;
			for (auto & tex_path : tex_paths) {
				if (path == tex_path) {
					already_loaded = true;
					break;
				}
			}
			if (!already_loaded) {
				tex_paths.push_back(path);	
			}

		}
	}

}


void processNode(aiNode *node, const aiScene *scene, std::vector<double> &vertices, std::vector<unsigned> &indices, std::vector<double> &uvs, std::vector<std::string> &tex_paths, std::vector<double> &norms) {
	//process all the node's meshes
	for (unsigned i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene, vertices, indices, uvs, tex_paths, norms);
	}

	//process the meshes of all the nodes children
	for (unsigned i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, vertices, indices, uvs, tex_paths, norms);
	}
}



std::shared_ptr<triangle_mesh> generate_model(const std::string& file_name, const bool flip_uvs = false)  {
	//https://learnopengl.com/Model-Loading/Model	

	std::vector<double> vertices;
	std::vector<unsigned> indices;
	std::vector<double> uvs;
	std::vector<double> norms;
	std::vector<std::string> tex_paths;

	unsigned assimp_settings = aiProcess_Triangulate | aiProcess_GenNormals;


	if (flip_uvs)
		assimp_settings |= aiProcess_FlipUVs;


	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(file_name, assimp_settings);	
	//aiProcess_Triangulate tells assimp to make the model entirely out of triangles
	//aiProcess_GenNormals creates normal vectors for each vertex
	//aiProcess_FlipUVS flips the texture coordinates on the y-axis


	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp Error:\n\t" << importer.GetErrorString() << std::endl;
	}

	processNode(scene->mRootNode, scene, vertices, indices, uvs, tex_paths, norms);


	//turing the read in data into a triangle mesh
	hittable_list triangles;	

	std::string file_dir = file_name.substr(0, file_name.find_last_of('/') );
	file_dir.append("/");

	auto current_material = make_shared<lambertian>(make_shared<image_texture>(file_dir.append(tex_paths[0]).c_str()) );

	for (int i = 0; i < indices.size(); i += 3) {
		triangles.add(make_shared<triangle>(	vec3(vertices[ 3*indices[i] ],		//each element of indices refers to 1 vertex
							vertices[ 3*indices[i]  + 1],	//each vertex has 3 elements - x,y,z
							vertices[ 3*indices[i]  + 2]),	

							vec3(vertices[ 3*indices[i+1] ],
							vertices[ 3*indices[i+1]  + 1],
							vertices[ 3*indices[i+1]  + 2]),

							vec3(vertices[ 3*indices[i+2] ],
							vertices[ 3*indices[i+2]  + 1],
							vertices[ 3*indices[i+2]  + 2]),

							vec3(norms[ 3*indices[i] ],		//each element of indices refers to 1 normal
							norms[ 3*indices[i]  + 1],		//each normal has 3 elements - x,y,z
							norms[ 3*indices[i]  + 2]),	

							vec3(norms[ 3*indices[i+1] ],
							norms[ 3*indices[i+1]  + 1],
							norms[ 3*indices[i+1]  + 2]),

							vec3(norms[ 3*indices[i+2] ],
							norms[ 3*indices[i+2]  + 1],
							norms[ 3*indices[i+2]  + 2]),


							uvs[2*indices[i] ],
							uvs[2*indices[i] +1],

							uvs[2*indices[i+1] ],
							uvs[2*indices[i+1] +1],

							uvs[2*indices[i+2] ],
							uvs[2*indices[i+2] +1],

							current_material));


	}

	return make_shared<triangle_mesh>(triangles, 0, 1);

}

