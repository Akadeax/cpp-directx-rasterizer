#pragma once
#include <fstream>

#include "Vertex.h"

namespace fileParsers
{
	static bool ParseOBJ(
		const std::string& filename,
		std::vector<Vertex_PosCol>& vertices,
		std::vector<uint32_t>& indices,
		bool flipAxisAndWinding = true
	)
	{
		std::ifstream file(filename);
		if (!file)
			return false;

		std::vector<dae::Vector3> positions{};
		std::vector<dae::Vector3> normals{};
		std::vector<dae::Vector2> UVs{};

		vertices.clear();
		indices.clear();

		std::string sCommand;
		// start a while iteration ending when the end of file is reached (ios::eof)
		while (!file.eof())
		{
			//read the first word of the string, use the >> operator (istream::operator>>) 
			file >> sCommand;
			//use conditional statements to process the different commands	
			if (sCommand == "#")
			{
				// Ignore Comment
			}
			else if (sCommand == "v")
			{
				//Vertex
				float x, y, z;
				file >> x >> y >> z;

				positions.emplace_back(x, y, z);
			}
			else if (sCommand == "vt")
			{
				// Vertex TexCoord
				float u, v;
				file >> u >> v;
				UVs.emplace_back(u, 1 - v);
			}
			else if (sCommand == "vn")
			{
				// Vertex Normal
				float x, y, z;
				file >> x >> y >> z;

				normals.emplace_back(x, y, z);
			}
			else if (sCommand == "f")
			{
				//if a face is read:
				//construct the 3 vertices, add them to the vertex array
				//add three indices to the index array
				//add the material index as attibute to the attribute array
				//
				// Faces or triangles
				Vertex_PosCol vertex{};
				size_t iPosition, iTexCoord, iNormal;

				uint32_t tempIndices[3];
				for (size_t iFace = 0; iFace < 3; ++iFace)
				{
					// OBJ format uses 1-based arrays
					file >> iPosition;
					vertex.position = positions[iPosition - 1];

					if ('/' == file.peek())//is next in buffer ==  '/' ?
					{
						file.ignore();//read and ignore one element ('/')

						if ('/' != file.peek())
						{
							// Optional texture coordinate
							file >> iTexCoord;
							vertex.uv = UVs[iTexCoord - 1];
						}

						if ('/' == file.peek())
						{
							file.ignore();

							// Optional vertex normal
							file >> iNormal;
							// TODO REMOVE FOR: NORMAL
							//vertex.normal = normals[iNormal - 1];
						}
					}

					vertices.push_back(vertex);
					tempIndices[iFace] = static_cast<uint32_t>(vertices.size()) - 1;
				}

				indices.push_back(tempIndices[0]);
				if (flipAxisAndWinding)
				{
					indices.push_back(tempIndices[2]);
					indices.push_back(tempIndices[1]);
				}
				else
				{
					indices.push_back(tempIndices[1]);
					indices.push_back(tempIndices[2]);
				}
			}
			//read till end of line and ignore all remaining chars
			file.ignore(1000, '\n');
		}

		//Cheap Tangent Calculations
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t index0 = indices[i];
			uint32_t index1 = indices[static_cast<size_t>(i) + 1];
			uint32_t index2 = indices[static_cast<size_t>(i) + 2];

			const dae::Vector3& p0 = vertices[index0].position;
			const dae::Vector3& p1 = vertices[index1].position;
			const dae::Vector3& p2 = vertices[index2].position;
			const dae::Vector2& uv0 = vertices[index0].uv;
			const dae::Vector2& uv1 = vertices[index1].uv;
			const dae::Vector2& uv2 = vertices[index2].uv;

			const dae::Vector3 edge0 = p1 - p0;
			const dae::Vector3 edge1 = p2 - p0;
			const dae::Vector2 diffX = dae::Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
			const dae::Vector2 diffY = dae::Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
			float r = 1.f / dae::Vector2::Cross(diffX, diffY);

			// TODO REMOVE FOR: TANGENT
			/*dae::Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
			vertices[index0].tangent += tangent;
			vertices[index1].tangent += tangent;
			vertices[index2].tangent += tangent;*/
		}

		// TODO REMOVE FOR: TANGENT
		//Fix the tangents per vertex now because we accumulated
		for (auto& v : vertices)
		{
			//v.tangent = dae::Vector3::Reject(v.tangent, v.normal).Normalized();

			if (flipAxisAndWinding)
			{
				v.position.z *= -1.f;
				//v.normal.z *= -1.f;
				//v.tangent.z *= -1.f;
			}

		}

		return true;
	}


}
