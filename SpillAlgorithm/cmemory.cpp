#include "stdafx.h"

//global variables
static myMap *cmhead;
static cMemory cm_p;
int cout = 0;

//init circulating memory
cMemory *setCM(int total)
{
	//ȡ������myMap
	int num = (total * 1024 * 1024) / sizeof(myMap);
	cmhead = (myMap *)malloc(num * sizeof(myMap));
	cMemory cm = { cmhead,cmhead,cmhead,cmhead + num - 1,true };
	cm_p = cm;
	return &cm_p;
}
//�ͷŻ����ڴ�
void cleanCM()
{
	free(cmhead);
}

//д�����ڴ�
int cmWrite(myMap *wp)
{
	//�ж��Ƿ��д
	if (cm_p.cm_write == cm_p.cm_read && !cm_p.is_empty)
	{
		//����д ����-1���еȴ�
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

//�����ڴ���һָ��
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

//��ȡ��ָ��֮��ļ���������ڴ棩
unsigned int getDistance(myMap *front, myMap *behind)
{
	int d = front - behind;
	if (d >= 0)
	{
		return d;
	}
	return cm_p.cm_end - cm_p.cm_head + d;
}