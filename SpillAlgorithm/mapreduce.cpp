#include "stdafx.h"

//global variables
static int spill_time = 0, map_time = 0;
static uMap ump = &dmp;
//记录spill时间
int recordSpillTime(int _time)
{
	spill_time = _time;
	return 0;
}

int recordMapTime(int _time)
{
	map_time = _time;
	return 0;
}

int getSpillTime()
{
	return spill_time;
}
int getMapTime()
{
	return map_time;
}

//释放内存
int freeFileName(fileName *head)
{
	fileName *p = NULL;
	while (head != NULL)
	{
		p = head->next;
		free(head);
		head = p;
	}
	return 0;
}

int freeMapLink(mapLink *head)
{
	if (head == NULL)
	{
		return 0;
	}
	mapLink *p = head->next;
	free(head->my_map);
	free(head);
	freeMapLink(p);
	return 0;
}

//默认map
mapLink* dmp(myMap *mp)
{
	mapLink *mpl;
	myMap *t;
	t = (myMap *)malloc(sizeof(myMap));
	mpl = (mapLink *)malloc(sizeof(mapLink));
	strcpy(t->key, mp->key);
	strcpy(t->value, mp->value);
	mpl->my_map = t;
	mpl->next = NULL;
	return mpl;
}

//设置自定义map
int setUMP(uMap func)
{
	ump = func;
	return 0;
}

//获取map()函数
uMap getUMP()
{
	return ump;
}