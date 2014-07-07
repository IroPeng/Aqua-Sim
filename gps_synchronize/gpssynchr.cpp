#include "gpssynchr.h"
#include <math.h>

gpssynchr::gpssynchr(void* underwaternodepointer,double z)
{
	this->nodepointer = underwaternodepointer;
	this->s.z = z;
}


gpssynchr::~gpssynchr(void)
{
}

/*
����:CoordinatePair n �¼��������ʱ���

����ֵ: void

˵����
�˺�����Ψһ�Ķ���ӿ�
�˺����ж�����cplist�Ƿ����������ϵ������
����������localization()��������
*/
void gpssynchr::addnewpair(CoordinatePair n)
{
	this->cplist.push_back(n);

	if(this->cplist.size() == 4)
		this->cplist.erase(this->cplist.begin(),this->cplist.begin()+1);
	
	if(this->cplist.size() == 3)
		this->localization();
}


/*
����:coor ����ĳһ�����ǵ�����ṹ

����ֵ: [ax,ay,-c]

˵��:
coor ������ĳһ�����ǵ�����ṹ
���ݸýṹ�������������Ƶ�x y zֵ 
�����ϵ������ H ��һ��
ʵ���˹�ʽ1 2
*/
vector<double> gpssynchr::getcofficient(Coordinate coor)
{
	double ri = getri(coor);

	double ax = (coor.x - this->s.x) / ri;
	double ay = (coor.y - this->s.y) / ri;

	vector<double> vret;

	vret.push_back(ax);
	vret.push_back(ay);
	vret.push_back(VELOCITY);

	return vret;
}
	
/*
����  ���Ա���cplist�е�һ������ʱ���

����ֵ: deltarou = _rou - rou

˵��:
�ú������ڼ��� deltarou  ʵ�ֹ�ʽ3
*/
double gpssynchr::getdeltarou(CoordinatePair p)
{
	double _rou = getri(p.s) + \
				  VELOCITY * this->tu;
	double rou = VELOCITY * (p.localreceivetime - p.s.ts);

	return _rou - rou;
}

/*
���� һ������ṹ��
����ֵ ������Riֵ

˵��:�������������ͱ��������ŷ����þ���
*/
double gpssynchr::getri(Coordinate coor)
{
	return sqrt((pow(coor.x-this->s.x,2)+ \
		         pow(coor.y-this->s.y,2)+ \
				 pow(coor.z-this->s.z,2)));
}

/*
���� ��
����ֵ ��

˵��:�ú����Ƕ�λ��ͬ���ĺ��ĺ���

���ȼ���ϵ������ H���� ���� deltarou

������Է����� 4

�õ�������Ϊ [deltax,deltay,deltat]
Ȼ���ø������ֱ����x y �� t�����Խڵ�ı��ض�Ӧ������ֵ������
*/
void gpssynchr::localization()
{
	vector<vector<double>> matrixH;
	vector<double> rou;
	vector<double> solution(3,0.0);

	this->s.x = (cplist[0].s.x + cplist[1].s.x + cplist[2].s.x) / 3.0;
	this->s.y = (cplist[0].s.y + cplist[1].s.y + cplist[2].s.y) / 3.0;
	this->tu = 0.0;

	matrixH.push_back(this->getcofficient(this->cplist[0].s));
	matrixH.push_back(this->getcofficient(this->cplist[1].s));
	matrixH.push_back(this->getcofficient(this->cplist[2].s));

	rou.push_back(this->getdeltarou(this->cplist[0]));
	rou.push_back(this->getdeltarou(this->cplist[1]));
	rou.push_back(this->getdeltarou(this->cplist[2]));

	//solution = H^(-1) * rou

	//����
	//nodepointer
}
