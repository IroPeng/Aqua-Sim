#include "common.h"
#include <vector>

using namespace std;

#pragma once

class gpssynchr
{
private:
	//x y z�Ĺ���ֵ����ṹ
	Coordinate s;
	vector<CoordinatePair> cplist;

	//function getxxx are used for caculation
	vector<double> getcofficient(Coordinate coor);
	double getdeltarou(CoordinatePair p);
	double getri(Coordinate coor);

	void localization();

	//tu �Ĺ���ֵ
	double tu;

	void* nodepointer;

public:
	// add a new pair of coordinate and localreceivetime;
	void addnewpair(CoordinatePair n);
	gpssynchr(void* underwaternodepointer,double z);
	~gpssynchr(void);
};

