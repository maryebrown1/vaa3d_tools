#ifndef INTEGRATEDDATATYPES_H
#define INTEGRATEDDATATYPES_H

#include <vector>
#include <deque>
#include <map>
#include <string>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include "basic_surf_objs.h"

using namespace std;

#ifndef PI
#define PI 3.1415926
#endif

#ifndef NODE_TILE_LENGTH
#define NODE_TILE_LENGTH 100
#endif

#ifndef SEGtileXY_LENGTH
#define SEGtileXY_LENGTH 30
#endif

#ifndef radANGLE_THRE
#define radANGLE_THRE 0.25
#endif

#ifndef zRATIO
#define zRATIO (1 / 0.2) // This is the ratio of z resolution to x and y in fMOST images.
//#define zRATIO (0.28 / 0.1144) // This is the ratio of z resolution to x and y in IVSCC images.
#endif

namespace integratedDataTypes
{
	enum connectOrientation { head_head, head_tail, tail_head, tail_tail, all_ort, head, tail };

	struct profiledNode
	{
		int index;
		bool front, back;
		long segID, frontSegID, backSegID, nodeNum, x, y, z;

		double innerProduct;
		double previousSqr, nextSqr, radAngle, distToMainRoute, turnCost;
	};

	struct topoCharacter
	{
		topoCharacter() {};
		topoCharacter(NeuronSWC centerNode, int streamLength = 10) : topoCenter(centerNode) {};
		NeuronSWC topoCenter;
		deque<NeuronSWC> upstream;
		map<int, deque<NeuronSWC>> downstreams;
	};

	struct segUnit
	{
		segUnit() : to_be_deleted(false) {};
		//segUnit(const segUnit& sourceSegUnit) {};

		int segID;
		int head;
		vector<int> tails;
		QList<NeuronSWC> nodes;
		map<int, size_t> seg_nodeLocMap;
		map<int, vector<size_t>> seg_childLocMap;
		vector<topoCharacter> topoCenters;

		bool to_be_deleted;
	};

	struct segPairProfile
	{
		segPairProfile() {};
		segPairProfile(const segUnit& inputSeg1, const segUnit& inputSeg2) : seg1Ptr(&inputSeg1), seg2Ptr(&inputSeg2) {};

		const segUnit* seg1Ptr;
		const segUnit* seg2Ptr;
		map<connectOrientation, double> connDistMap;

		void getSegDistance(connectOrientation connOrt = all_ort);
	};

	struct profiledTree
	{
		// With reinitialization function provided, this struct needs copy control constructors. 
		// Will be implemented later.

		profiledTree() {};
		profiledTree(const NeuronTree& inputTree, float segTileLength = SEGtileXY_LENGTH);
		float segTileSize;
		float nodeTileSize;
		void nodeTileResize(float nodeTileLength);

		NeuronTree tree;
		map<int, size_t> node2LocMap;
		map<int, vector<size_t>> node2childLocMap;

		map<string, vector<int>> nodeTileMap; // tile label -> node ID
		map<int, segUnit> segs; // key = seg ID

		map<string, vector<int>> segHeadMap;   // tile label -> seg ID
		map<string, vector<int>> segTailMap;   // tile label -> seg ID

		boost::container::flat_map<int, boost::container::flat_set<int>> segHeadClusters; // key is ordered cluster number label
		boost::container::flat_map<int, boost::container::flat_set<int>> segTailClusters; // key is ordered cluster number label
		boost::container::flat_map<int, int> headSeg2ClusterMap;
		boost::container::flat_map<int, int> tailSeg2ClusterMap;

		map<int, topoCharacter> topoList;
		void addTopoUnit(int nodeID);
	};

}

#endif