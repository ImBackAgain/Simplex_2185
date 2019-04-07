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

void Simplex::Octonode::AdddPerson(MyRigidBody * newcomer, uint maxLv, uint entCount)
{
	if (!broken)
	{
		if (inside.size() < entCount || maxLv <= 1)
			//Lesss than limit			Can't subdivide
		{
			inside.push_back(newcomer);
		}
		else
		{
			inside.clear();
			broken = true;

			float ex = extents.x, ey = extents.y, ez = extents.z;

			for (uint i = 0; i < 8; i++)
			{
				vector3 disp = vector3
				(
					ex * (i < 4 ? -1 : 1),
					ey * (i % 4 < 2 ? -1 : 1),
					ez * (i % 2 < 1 ? -1 : 1)
				);
				Octonode* octant = new Octonode(center + disp/2, extents / 2);
				babies.push_back(octant);
			}
		}
	}
	if (broken)
	{
		for (Octonode *baby : babies)
		{
			if (baby->Contains(newcomer)) baby->AdddPerson(newcomer, maxLv - 1, entCount);
		}
	}
}

void Simplex::Octonode::CheckColllisions()
{
	if (!broken)
	{
		for (int i = 0; i + 1 < inside.size(); i++)
		{
			renderman->AddAxisToRenderList(IDENTITY_M4);//inside[i]
			for (uint j = i + 1; j < inside.size(); j++)
			{
				inside[i]->IsColliding(inside[j]);
			}
		}
		//matrix4 m = glm::translate(center) * glm::scale(extents);
		//renderman->AddIcoSphereToRenderList(m, C_RED);
	}
	else
	{
		for (Octonode *baby : babies)
		{
			baby->CheckColllisions();
		}
	}
}


Octonode::~Octonode()
{
	Die();
}

void Simplex::Octonode::Die()
{
	for (Octonode* baby : babies)
	{
		baby->Die();
		delete[] baby;
	}
}

void Simplex::Octonode::Draw()
{
	matrix4 transform = glm::translate(center) * glm::scale(2*extents);

	renderman->AddWireCubeToRenderList(transform, C_YELLOW);

	if (broken)
	{
		for (Octonode *baby : babies)
		{
			baby->Draw();
		}
	}
}

bool Simplex::Octonode::Contains(MyRigidBody *obj)
{
	vector3 thisMin = center - extents;
	vector3 thisMax = center + extents;
	vector3 objMin = obj->GetMinGlobal();
	vector3 objMax = obj->GetMaxGlobal();

	float EPSILON = 0.1f;

	if (thisMax.x < objMin.x - EPSILON) //this to the right of other
		return false;
	else if (thisMin.x > objMax.x + EPSILON) //this to the left of other
		return false;

	else if (thisMax.y < objMin.y - EPSILON) //this below of other
		return false;
	else if (thisMin.y > objMax.y + EPSILON) //this above of other
		return false;

	else if (thisMax.z < objMin.z - EPSILON) //this behind of other
		return false;
	else if (thisMin.z > objMax.z + EPSILON) //this in front of other
		return false;
}
