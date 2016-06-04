#include "stdafx.h"

//global variables
static myMap *cmhead;
static cMemory cm_p;
int cout = 0;

//init circulating memory
cMemory *setCM(int total)
{
	//取整数个myMap
	int num = (total * 1024 * 1024) / sizeof(myMap);
	cmhead = (myMap *)malloc(num * sizeof(myMap));
	cMemory cm = { cmhead,cmhead,cmhead,cmhead + num - 1,true };
	cm_p = cm;
	return &cm_p;
}
//释放环形内存
void cleanCM()
{
	free(cmhead);
}

//写环形内存
int cmWrite(myMap *wp)
{
	//判断是否可写
	if (cm_p.cm_write == cm_p.cm_read && !cm_p.is_empty)
	{
		//不可写 返回-1进行等待
		return -1;
	}
	strcpy(cm_p.cm_write->key, wp->key);
	strcpy(cm_p.cm_write->value, wp->value);
	cm_p.cm_write = cmNext(cm_p.cm_write, true);
	return getDistance(cm_p.cm_write, cm_p.cm_read);
}

//myMap cmRead(myMap *rp)
//{
//	return *rp;
//}

//环形内存下一指针
myMap *cmNext(myMap *p, bool change_empty)
{
	++p;
	if (p == cm_p.cm_end)
	{
		p = (myMap *)cm_p.cm_head;
		cm_p.is_empty = change_empty ? (!cm_p.is_empty) : cm_p.is_empty;
	}
	return p;
}

//获取两指针之间的间隔（环形内存）
unsigned int getDistance(myMap *front, myMap *behind)
{
	int d = front - behind;
	if (d >= 0)
	{
		return d;
	}
	return cm_p.cm_end - cm_p.cm_head + d;
}