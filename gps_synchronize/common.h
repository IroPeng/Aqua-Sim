/*
*basic data struct 
*
*/

/*
*Sink�ڵ㷢���Ľṹ��
*/

#define VELOCITY 1500.0

struct Coordinate
{
	float x;
	float y;
	float z;  // coordinate of a node
	float ts; // 
	Coordinate() : x(0),y(0),z(0),ts(0){};
	Coordinate(double x,double y,double z, double ts) : x(x),y(y),z(z),ts(ts){};
};

/*
* ˮ�½ڵ�ά�������ݽṹ
*/
struct CoordinatePair
{
	Coordinate s;
	float localreceivetime;
	
	CoordinatePair(Coordinate S,float time): s(S),localreceivetime(time){}
};
