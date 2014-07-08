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

/*
*���׵�ģ��underwaternode��
*/
struct underwaternode
{
	double x;
	double y;
	double z;

	double timenow;

	underwaternode(double x = 0.0, double y = 0.0, double z = 0.0, double t = 0.0) : x(x), y(y), z(z), timenow(t){};
}; 