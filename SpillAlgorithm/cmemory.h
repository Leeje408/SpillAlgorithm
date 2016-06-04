#pragma once

//环形内存结构
struct cMemory
{
	const myMap *cm_head;
	myMap *cm_read;
	myMap *cm_write;
	const myMap *cm_end;
	bool is_empty;
};

//设置环形内存大小
cMemory *setCM(int total);
//释放环形内存
void cleanCM();
//写环形内存
int cmWrite(myMap *wp);
myMap *cmNext(myMap *p, bool change_empty);
unsigned int getDistance(myMap *front, myMap *behind);