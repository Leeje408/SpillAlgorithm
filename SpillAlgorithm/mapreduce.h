#pragma once

//map struct
struct myMap
{
	char key[128];
	char value[128];
};

//自定义map()函数返回数据
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

//定义函数指针 指向自定义map()
typedef mapLink* (*uMap)(myMap*);

int recordSpillTime(int _time);
int recordMapTime(int _time);
int getSpillTime();
int getMapTime();
int freeFileName(fileName *head);
int freeMapLink(mapLink *head);
mapLink* dmp(myMap *mp); //默认map
int setUMP(uMap func);
uMap getUMP();