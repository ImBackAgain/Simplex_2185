#ifndef __OCTONODE_H
#define __OCTONODE_H
#include "MyEntity.h"
namespace Simplex
{
class Octonode
{
	std::vector<MyRigidBody*> inside;
	std::vector<Octonode*> partitions;	
	vector3 center, extents;
	bool broken = false;
	void FindRenderman();
	void Die();
public:
	static MeshManager* renderman;
	Octonode();
	Octonode(vector3 center, vector3 extents);
	void AddEntity(MyRigidBody* newcomer, uint maxLv, uint entCount);
	void CheckColllisions();
	~Octonode();
	void Draw();
};

}
#endif