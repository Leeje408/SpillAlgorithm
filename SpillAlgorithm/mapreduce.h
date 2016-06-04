#pragma once

//map struct
struct myMap
{
	char key[128];
	char value[128];
};

//�Զ���map()������������
struct mapLink
{
	myMap *my_map;
	mapLink *next;
};

//filename link
struct fileName
{
	char name[128];
	char path[256];
	fileName *next;
};

//���庯��ָ�� ָ���Զ���map()
typedef mapLink* (*uMap)(myMap*);

int recordSpillTime(int _time);
int recordMapTime(int _time);
int getSpillTime();
int getMapTime();
int freeFileName(fileName *head);
int freeMapLink(mapLink *head);
mapLink* dmp(myMap *mp); //Ĭ��map
int setUMP(uMap func);
uMap getUMP();