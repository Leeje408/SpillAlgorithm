#pragma once

//�����ڴ�ṹ
struct cMemory
{
	const myMap *cm_head;
	myMap *cm_read;
	myMap *cm_write;
	const myMap *cm_end;
	bool is_empty;
};

//���û����ڴ��С
cMemory *setCM(int total);
//�ͷŻ����ڴ�
void cleanCM();
//д�����ڴ�
int cmWrite(myMap *wp);
myMap *cmNext(myMap *p, bool change_empty);
unsigned int getDistance(myMap *front, myMap *behind);