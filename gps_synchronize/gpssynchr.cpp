#include "gpssynchr.h"
#include <math.h>
#include <time.h> 



gpssynchr::gpssynchr(underwaternode* underwaternodepointer, double z)
{
	this->nodepointer = underwaternodepointer;
	this->s.z = z;
}


gpssynchr::~gpssynchr(void)
{
}


enum{S_X,S_Y,TU};
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
	
	float last_result[3];
	this->cplist.push_back(n);

	printf("The new pair is (%lf,%lf,%lf)\n", n.s.x, n.s.y, n.s.z);

	if(this->cplist.size() == 4)
		this->cplist.erase(this->cplist.begin(),this->cplist.begin()+1);
	
	if (this->cplist.size() == 3)
	{
		this->s.x = (cplist[0].s.x + cplist[1].s.x + cplist[2].s.x) / 3.0;
		this->s.y = (cplist[0].s.y + cplist[1].s.y + cplist[2].s.y) / 3.0;
		this->tu = 0.0;

		int count = 1;
		do 
		{
			printf("++++++++++++\n\tRound %d\n++++++++++++\n",count++);
			last_result[S_X] = this->s.x;
			last_result[S_Y] = this->s.y;
			last_result[TU] = this -> tu;
			this->localization();
		} while (fabs(last_result[S_X] - this->s.x) > 0.001 || 
			fabs(last_result[S_Y] - this->s.y) > 0.001 || fabs(last_result[TU] - this->tu) > 0.001);
			
		for (int i = 0; i < 6;i ++)
			this->localization();

		this->nodepointer->x = this->s.x;
		this->nodepointer->y = this->s.y;
		this->nodepointer->z = this->s.z;
		this->nodepointer->timenow = this->tu;
		//��nodepointer ���в���
	}
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
	vret.push_back(-VELOCITY);

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
// 	vector<vector<double>> matrixH;
	vector<double> rou;
	vector<double> solution(3,0.0);

	float *matrixH = (float *)malloc(sizeof(float) * 18);
	float *p;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			*(matrixH + i * 3 + j) = this->getcofficient(this->cplist[i].s).at(j);
		}
	}
		
// 
// 	matrixH.push_back(this->getcofficient(this->cplist[0].s));
// 	matrixH.push_back(this->getcofficient(this->cplist[1].s));
// 	matrixH.push_back(this->getcofficient(this->cplist[2].s));

	rou.push_back(this->getdeltarou(this->cplist[0]));
	rou.push_back(this->getdeltarou(this->cplist[1]));
	rou.push_back(this->getdeltarou(this->cplist[2]));



	//���棬 ���ǲ���������get
	//solution = H^(-1) * rou

	float determ = matrixdeterm(matrixH,3);
	p = matrixH+9;
	if (fabs(determ) > 1e-6) //����ʽֵ��Ϊ0���������
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				*(p + j * 3 + i) = make3adjmatrix(matrixH, i, j) / determ;
			}
		}

		printf("H-1\n");
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				printf("%lf\t",*(p+i*3+j));
			}
			printf("\n");
		}
		printf("---------------\n");
		for (int i = 0; i < 3; i++)
		{
			solution[i] = 0;
			for (int j = 0; j < 3; j++)
			{
				solution[i] += rou[j] * (*(p + i * 3 + j));
			}
		}
	}
	else //���󲻿���
	{
		printf("matrixH can't be inversed!\n");
		solution[0] = solution[1] = solution[2] = 0;
	}





	//����
	this->s.x += solution[0];
	this->s.y += solution[1];
	this->tu += solution[2]; 
	
	printf("s.x = %lf\n", this->s.x);
	printf("s.y = %lf\n", this->s.y);
	printf("tu = %lf\n", this->tu);
	printf("================\n");
}

/*
������matrixH Ҫ��������ʽ�ľ���H
����ֵ�����������ʽֵ
˵�����������ָ��ľ��������ʽֵ
*/


float gpssynchr::matrixdeterm(float *p, int n)
{
	int r, c, m;
	int lop ;
	float result = 0;
	float mid = 1;
	if (n == 1)
		return *p;
	lop = n == 2 ? 1 : n;
	for (m = 0; m < lop; m++)
	{
		mid = 1;
		for (r = 0, c = m; r < n; r++, c++)
		{
			mid = mid * (*(p + r*n + c%n));
		}
		result += mid;
	}
	for (m = 0; m < lop; m++)
	{
		mid = 1;
		for (r = 0, c = 2*n-1 - m; r < n; r++, c--)
		{
			mid = mid * (*(p + r*n + c%n));
		}
		result -= mid;
	}
	return result;
}
/*
������matrixH Ҫ����������ľ���H
	m������Ԫ�ص�������
	n������Ԫ�ص�������
����ֵ��λ�ھ���H������Ϊ(m,n)��Ԫ�صİ�������Ӧ��ֵ
˵�������������ĳԪ�ض�Ӧ�İ������λ�õ�ֵ
*/
float gpssynchr::make3adjmatrix(float *p,int m,int n)
{
	int len = 4;
	int i, j;
	float mid_result = 0;
	int sign = 1;
	float *p_create, *p_mid;
	
	p_create = (float *)malloc(sizeof(float)*len);
	p_mid = p_create;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (i != m && j != n)
			{
				*p_mid++ = *(p + i*3 + j);
			}
		}
	}
	sign = (m + n) % 2 == 0?1: - 1;
	mid_result = (float)sign * matrixdeterm(p_create,2);
	free(p_create);
	return mid_result;
}


void gpssynchr::print()
{
	printf("x = %lf\n", nodepointer->x);
	printf("y = %lf\n", nodepointer->y);
	printf("z = %lf\n", nodepointer->z);
	printf("timenow = %lf\n", nodepointer->timenow);
}

double edistance(Coordinate c1, Coordinate c2)
{
	return sqrt((c1.x - c2.x) * (c1.x - c2.x) + \
		(c1.y - c2.y) * (c1.y - c2.y) + \
		(c1.z - c2.z) * (c1.z - c2.z));
}

double propagationtime(Coordinate c1, Coordinate c2)
{
	return edistance(c1, c2) / VELOCITY;
}

// #define MAX_X 500
// #define MAX_Y 500
// #define MAX_Z 500
// 
// void createnewpair(underwaternode* node,Coordinate c1, Coordinate c2, Coordinate c3)
// {
// 
// 
// 	
// }


int main()
{
	char a;
	underwaternode* node = new underwaternode(100.0, 200.0, 100.0);
	gpssynchr inanode(node, 320);

	Coordinate target(142, 350, 320, 1.23547);

	Coordinate c1(500.0, 400.0, 0., 1.23547);
	Coordinate c2(250.0, 150.0, 0., 1.23547);
	Coordinate c3(100.0, 200.0, 0., 1.23547);

// 	srand((unsigned)time(NULL));
// 	int coor_count = 0;
// 	while (scanf("%*c")!=-1)
// 	{
// 		Coordinate target(rand() % MAX_X, rand() % MAX_Y, rand() % MAX_Z, 1.23547);
// 		if (coor_count == 0)
// 		{
// 			CoordinatePair p(c1, node->timenow + propagationtime(c1, target)); 
// 			inanode.addnewpair(p);
// 		}
// 		else if (coor_count == 1)
// 		{
// 			CoordinatePair p(c2, node->timenow + propagationtime(c2, target));
// 			inanode.addnewpair(p);
// 		}
// 		else if (coor_count == 2)
// 		{
// 			CoordinatePair p(c3, node->timenow + propagationtime(c3, target));
// 			inanode.addnewpair(p);
// 		}
// 		coor_count = (coor_count + 1) % 3;
// 	
// 	inanode.print();
// 	}

 
  	CoordinatePair p1(c1, node->timenow + propagationtime(c1, target));
  	CoordinatePair p2(c2, node->timenow + propagationtime(c2, target));
  	CoordinatePair p3(c3, node->timenow + propagationtime(c3, target));
 
  	inanode.addnewpair(p1);
  	inanode.addnewpair(p2);
  	inanode.addnewpair(p3);
	inanode.print();
	

	system("pause");
}
