#ifndef __OCTONODE_H
#define __OCTONODE_H
#include "MyEntity.h"
namespace Simplex
{
class Octonode
{
	std::vector<MyRigidBody> inside;
	std::vector<Octonode*> partitions;	
	vector3 center, extents;
	static MeshManager renderman;
public:
	Octonode();
	Octonode();
	void CheckColllisions();
	~Octonode();
	void Draw();
};

}
#endif