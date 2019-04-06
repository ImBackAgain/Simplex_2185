#include "Octonode.h"

using namespace Simplex;

MeshManager* Simplex::Octonode::renderman = nullptr;

void Simplex::Octonode::FindRenderman()
{
	if (!renderman) renderman = MeshManager::GetInstance();
}

Simplex::Octonode::Octonode()
{
	FindRenderman();
}

Simplex::Octonode::Octonode(vector3 center, vector3 ext)
{
	FindRenderman();
	this->center = center;
	extents = ext;
	//printf("\n\nNode:22\nExtents: [%f, %f, %f]", ext.x, ext.y, ext.z);
}

void Simplex::Octonode::AddEntity(MyRigidBody * newcomer, uint maxLv, uint entCount)
{
	if (!broken)
	{
		if (inside.size() < entCount || maxLv <= 1)
			//Okay to addd more				Can't subdivide
		{
			inside.push_back(newcomer);
		}
		else
		{
			inside.clear();
			broken = true;

			float cx = center.x, cy = center.y, cz = center.z;

			for (uint i = 0; i < 8; i++)
			{
				vector3 newC = vector3
				(
					cx * (i > 4 ? -1 : 1),
					cy * (i % 4 > 2 ? -1 : 1),
					cz * (i % 2 > 0 ? -1 : 1)
				);
				Octonode* octant = new Octonode(newC, extents / 2);
				partitions.push_back(octant);
			}
		}
	}
	
}

void Simplex::Octonode::CheckColllisions()
{
	for (uint i = 0; i < (inside.size()-1); i++)
	{
		for (uint j = i + 1; j < inside.size(); j++)
		{
			inside[i]->IsColliding(inside[j]);
		}
	}
}


Octonode::~Octonode()
{
	Die();
}

void Simplex::Octonode::Die()
{
	for (Octonode* baby : partitions)
	{
		baby->Die();
		delete[] baby;
	}
}

void Simplex::Octonode::Draw()
{
	matrix4 transform = glm::translate(center) * glm::scale(2*extents);

	renderman->AddWireCubeToRenderList(transform, C_YELLOW);
}
