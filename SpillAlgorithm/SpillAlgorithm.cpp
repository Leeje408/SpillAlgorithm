// SpillAlgorithm.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#define NTL "name_too_long"

//map struct
struct myMap
{
	char key[128];
	char value[128];
};
//filename link
struct fileName
{
	char name[128];
	char path[256];
	fileName *next;
};
//�����ڴ�ṹ
static struct cMemory
{
	const myMap *cm_head;
	myMap *cm_read;
	myMap *cm_write;
	const myMap *cm_end;
};
//struct configuration
//ȫ������
struct lee_conf
{
	bool show_path_in;		//�Ƿ���ʾ�����ļ�
	float spill_percent;   //�����
	int cm_size;	//�����ڴ��С
	char in_path[128];	  //��������Ŀ¼
	char out_path[128];	  //�������Ŀ¼
};
static lee_conf myconf = {true,0.8f,1,"",""};
static myMap *cmhead;

float spillPercentage(float map_rate, float spill_rate);
myMap leeMap(cMemory cm);
const cMemory setCM(int total);

int main(int argc, char *argv[])
{
	char path[128] = "C:/Users/lx/AppData/Local/Microsoft/VisualStudio/14.0/Extensions/tasrga3o.xgg/Images";
	strcpy(myconf.in_path, path);
	cMemory cm = setCM(1);
	leeMap(cm);
	printf("%10d\t%10d\t%10d\t%10d\n",cm.cm_head,cm.cm_read,cm.cm_write,cm.cm_end);
	return 0;
}

/*
	adjust percent of spill
*/
float spillPercentage(float map_rate, float spill_rate)
{
	float per = 0.6f;
	float n_per = spill_rate / (map_rate + spill_rate);
	per = n_per > 0.5f ? n_per : 0.5f;
	return per;
}

/*
	map
*/
myMap leeMap(cMemory cm)
{
	myMap inmap;
	strcpy(inmap.key, "");
	strcpy(inmap.value, "");
	char *in_path = myconf.in_path;
	FILE *fp;
	fileName *head = NULL, *p = NULL;

	//read dir
#ifdef linux
	Dir *dir;
	struct dirent *rent;
	if ((dir = opendir(in_path)) == NULL)
	{
		perror("path not found!\n");
		exit(1);
	}
	while (rent = readdir(dir) != NULL)
	{
		if (strcmp(rent->d_name, ".") == 0 || strcmp(rent->d_name, "..") == 0)
		{
			continue;
		}
		else if (rent->d_type == 8)
		{
			//record filename
			fileName *fn;
			fn = (fileName *)malloc(sizeof(fileName));
			fn->next = NULL;
			char tmp[256] = "";
			strcpy(fn->name, strlen(fd.name) < 128 ? fd.name : NTL);
			strcat(tmp, strlen(in_path) < 128 ? in_path : NTL);
			if (strstr(in_path + strlen(in_path) - 1, "/") == NULL)
			{
				strcat(tmp, "/");
			}
			strcpy(fn->path, tmp);
			strcat(fn->path, fn->name);
			if (head == NULL)
			{
				head = p = fn;
				continue;
			}
			p->next = fn;
			p = fn;
		}
	}
	closedir(dir);
#endif
#ifdef WIN32
	_finddata_t fd;
	long fl;
	char path[128];
	strcpy(path, in_path);
	strcat(path, "\\*");
	if ((fl = _findfirst(path, &fd)) == -1)
	{
		fprintf(stderr, "Path:%s not found", path);
	}
	else
	{
		while (_findnext(fl, &fd) == 0)
		{
			if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0)
			{
				continue;
			}
			//record filename
			fileName *fn;
			fn = (fileName *)malloc(sizeof(fileName));
			fn->next = NULL;
			char tmp[256] = "";
			int path_length = strlen(path);
			strcpy(fn->name, strlen(fd.name) < 128 ? fd.name : NTL);
			strncpy(tmp, path_length < 128 ? path : NTL, path_length < 128 ? path_length - 3: strlen(NTL));
			if (strstr(path + strlen(path) - 1, "/") == NULL)
			{
				strcat(tmp, "/");
			}
			strcpy(fn->path, tmp);
			strcat(fn->path, fn->name);
			if (head == NULL)
			{
				head = p = fn;
				continue;
			}
			p->next = fn;
			p = fn;
		}
	}
	_findclose(fl);
#endif // WIN32
	//end read dir
	if (myconf.show_path_in && head != NULL)
	{
		p = head;
		printf("+------------------------------------------------------------+\n");
		while (p != NULL)
		{
			int p_length = strlen(p->path);
			char tmp[56] = "....";
			if (p_length > 55)
			{
				strcat(tmp, strstr(p->path + p_length - 51, "/"));
			}
			printf("|name:%55s|\n|path:%55s|\n", p->name, strlen(tmp) > 5 ? tmp : p->path);
			printf("+------------------------------------------------------------+\n");
			p = p->next;
		}
	}

	//read file

	return inmap;
}


//circulating memory
const cMemory setCM(int total)
{
	 //ȡ������myMap
	int num = (total * 1024 * 1024)/sizeof(myMap);
	cmhead = (myMap *)malloc(num*sizeof(myMap) + 1);
	cMemory cm = {cmhead,cmhead,cmhead,cmhead + (num * sizeof(myMap))};
	return cm;
}
//�ͷŻ����ڴ�
int cleanCM()
{
	free(cmhead);
	return 0;
}


int cmWrite(myMap *wp) 
{
	return 0;
}