// SpillAlgorithm.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//ȫ�ֱ���
cMemory *cm;
bool is_spill = false;
//int e_i = 0;

//////////////////////////////////////////////////////////////////////////
struct coupleCharP
{
	char *sp;
	char *ep;
};

bool isInArray_s(char check, char sets[])
{
	while (sets != NULL && strlen(sets) != 0)
	{
		if (check == *sets)
		{
			return true;
		}
		sets++;
	}
	return false;
}

//���ո���
coupleCharP split(char *source, char split_char[])
{
	char *sp = NULL, *ep = NULL;
	while (source != NULL && strlen(source) != 0)
	{
		if (!isInArray_s(*source, split_char))
		{
			sp = sp ? sp : source;
		}
		else
		{
			ep = sp ? source : ep;
		}
		if (sp != NULL && ep != NULL)
		{
			break;
		}
		source++;
	}
	return{ sp,ep };
}

//�Զ���map����
mapLink *oMap(myMap *raw_map)
{
	mapLink *head = NULL, *p = NULL, *tmp;
	myMap *t;
	coupleCharP char_p;
	char *tp = raw_map->key, split_sign[10] = { ' ','<','>','"','.',',' ,'=','(',')' };
	//������ΪС���ı�
	while (tp != NULL && strlen(tp) != 0)
	{
		char_p = split(tp, split_sign);
		if (char_p.sp == NULL)
		{
			break;
		}
		t = (myMap *)malloc(sizeof(myMap));
		int len = char_p.ep ? (char_p.ep - char_p.sp) : strlen(char_p.sp);
		strncpy(t->key, char_p.sp, len);
		t->key[len] = '\0';
		strcpy(t->value, "1");
		tmp = (mapLink *)malloc(sizeof(mapLink));
		tmp->my_map = t;
		tmp->next = NULL;
		if (p)	p->next = tmp;
		head = head ? head : tmp;
		p = tmp;
		tp = char_p.ep;
	}
	return head;
}
//////////////////////////////////////////////////////////////////////////



//��������
float spillPercentage();
unsigned int __stdcall leeMap();
unsigned int __stdcall spill();



int main(int argc, char *argv[])
{
	HANDLE 	handle;
	SYSTEMTIME start_t, end_t;
	GetSystemTime(&start_t);
	bool is_argv = false;
	if (is_argv && argc < 3)
	{
		fprintf(stderr, "Usage:\t <function_name> <input_path> <output_path> ...\n");
		exit(1);
	}
	cm = setCM(1);
	initConf();
	setInPath("E:/test");
	setOutPath("E:/test");
	setSpillPath("E:/test/spill");
	setAdjustSpill(true);
	setUMP(&oMap);
	handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)leeMap, NULL, 0, NULL);
	WaitForSingleObject(handle, INFINITE);
	cleanCM();
	GetSystemTime(&end_t);
	int dif_t = 1000 * (end_t.wSecond - start_t.wSecond) + end_t.wMilliseconds - start_t.wMilliseconds;
	fprintf(stderr, "��������ʱ�䣺%d����\n", dif_t);
	return 0;
}

/*
	adjust percent of spill
*/
float spillPercentage()
{
	int map_rate = getMapTime(),
		spill_rate = getSpillTime();
	fprintf(stderr, "map_rate__%d\tspill_rate__%d\n", map_rate, spill_rate);
	//У�������Ƿ�Ϊ0 ���򷵻س�ʼ����/��һ������
	if (!map_rate || !spill_rate)
	{
		return getSpillPercent();
	}
	float n_per = (float)map_rate / (float)(map_rate + spill_rate);
	fprintf(stderr, "n_per:%d\n",n_per);
	float per = n_per > 0.5f ? (n_per > 0.9f ? 0.9f : n_per) : 0.51f;
	fprintf(stderr, "[INFO]������������Ϊ��%5.4f\n", per);
	setSpillPercent(per);
	return per;
}

/*
	map
*/
unsigned int __stdcall leeMap()
{
	myMap inmap;
	strcpy(inmap.key, "");
	strcpy(inmap.value, "1");
	mapLink *map_link = NULL, *head_map_link = NULL;
	char *in_path = getInPath();
	FILE *fp;
	fileName *head = NULL, *p = NULL;
	//��ȡ�Զ���map
	uMap u_map = getUMP();
	unsigned int is_spill = 0;
	//spill���
	HANDLE spill_handle;
	//ʱ�����
	SYSTEMTIME start_time, end_time;
	//��ȡ�����
	int spill_size = getSpillSize();


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
	strcat(path, "*");
	if ((fl = _findfirst(path, &fd)) == -1)
	{
		fprintf(stderr, "[ERROR]Path:%s not found\n", path);
		exit(1);
	}
	else
	{
		while (_findnext(fl, &fd) == 0)
		{
			if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0 || fd.attrib & _A_SUBDIR)
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
			strncpy(tmp, path_length < 128 ? path : NTL, path_length < 128 ? path_length - 1 : strlen(NTL));
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
	//show input files
	if (getShowPathIn() && head != NULL)
	{
		p = head;
		printf("+------------------------------------------------------------+\n");
		while (p != NULL)
		{
			int p_length = strlen(p->path);
			if (p_length > 55)
			{
				char tmp[56] = "....";
				strcat(tmp, strstr(p->path + p_length - 51, "/"));
				printf("|name:%55s|\n|path:%55s|\n", p->name, tmp);
			}
			else
			{
				printf("|name:%55s|\n|path:%55s|\n", p->name, p->path);
			}			
			printf("+------------------------------------------------------------+\n");
			p = p->next;
		}
	}

	//read files
	//���ж�ȡ��ÿ�����д�����ڴ棨�̣߳� �ͷ��Ѵ����ļ�
	p = head;
	//��¼�״�map��ʼʱ��
	GetSystemTime(&start_time);
	while (p != NULL)
	{
		fp = fopen(p->path, "r");
		if (fp == NULL)
		{
			printf("[ERROR]���ļ���%s ʧ��\n", p->path);
			p = p->next;
			continue;
		}
		else
		{
			printf("[INFO]���ļ���%s\n", p->path);
		}
		while (fgets(inmap.key, 127, fp) != NULL)
		{
			int len = strlen(inmap.key);
			if (len == 1)
			{
				continue;
			}
			//��ȥ�ַ���ĩβ���з�
			if (strstr(inmap.key + strlen(inmap.key) - 2,"\n") != NULL)
			{
				inmap.key[strlen(inmap.key) - 1] = '\0';
			}
			head_map_link = map_link = (*u_map)(&inmap);
			if (head_map_link == NULL)
			{
				continue;
			}
			//��ʼд�����ڴ� ѭ��д��
			while (map_link != NULL)
			{
				is_spill = cmWrite(map_link->my_map);
				if (is_spill == spill_size)
				{
					//��¼mapʱ��
					GetSystemTime(&end_time);
					int dif_time = 1000 * (end_time.wSecond - start_time.wSecond) + end_time.wMilliseconds - start_time.wMilliseconds;
					fprintf(stderr, "[INFO]mapʱ�䣺%d����\n", dif_time);
					recordMapTime(dif_time);
					if (is_spill)
					{
						WaitForSingleObject(spill_handle, INFINITE);
					}
					spill_handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)spill, NULL, 0, NULL);
					//���¼��������
					if (getAdjustSpill())
					{
						spillPercentage();
						spill_size = getSpillSize();
					}
					start_time = end_time;
				}
				if (is_spill == -1)
				{
					fprintf(stderr, "[Map]write:%x\t read:%x\n",cm->cm_write,cm->cm_read);
					WaitForSingleObject(spill_handle, INFINITE);
					fprintf(stderr, "[Map]write:%x\t read:%x\n", cm->cm_write, cm->cm_read);
					//�̵߳ȴ�
					continue;
				}
				map_link = map_link->next;
			}
			freeMapLink(head_map_link);
		}
		p = p->next;
		fclose(fp);
	}
	WaitForSingleObject(spill_handle, INFINITE);
	//�ѻ����ڴ������� д��support�ļ�
	strcpy(head->path, getSpillPath());
	strcat(head->path, "support");
	if ((fp = fopen(head->path, "w")) == NULL)
	{
		fprintf(stderr, "[ERROR]supportд��ʧ��\n");
	}
	else
	{
		while (cm->cm_read != cm->cm_write || !cm->is_empty)
		{
			fprintf(fp, "%s\t%s\n", cm->cm_read->key, cm->cm_read->value);
			cm->cm_read = cmNext(cm->cm_read, true);
		}
	}
	fclose(fp);
	//�ͷ��ļ���
	freeFileName(head);
	return 0;
}

//spill ����
unsigned int __stdcall spill()
{
	//����spill��ʶ
	is_spill = true;
	fprintf(stderr, "[INFO]spill��ʼ\n");
	//�½���ʱspillָ��
	myMap *sp = cm->cm_read;
	//�½���ʱ�ļ� ������ǰʱ����ļ���
	FILE *fp;
	char name[128] = "", tmp[12] = "";
	//��¼spill��ʱ�䣬����Ϊ�����ļ���
	SYSTEMTIME now, end_time;
	GetSystemTime(&now);
	sprintf(name, "%sspill_%d", getSpillPath(), 1000 * now.wSecond + now.wMilliseconds);
	//д�����ļ�
	if ((fp = fopen(name, "w")) == NULL)
	{
		//���ļ�������ֹ����
		fprintf(stderr, "[ERROR]���ļ���\"%s\"����\n", name);
		exit(1);
	}
	//��spill��read֮��Ϊ������������
	int spill_size = getSpillSize();
	while (getDistance(sp, cm->cm_read) < spill_size)
	{
		fprintf(fp, "%s\t%s\n", sp->key, sp->value);
		sp = cmNext(sp, false);
	}
	fclose(fp);
	//spill���� ����readָ��,����is_empty
	fprintf(stderr, "[Spill]spill:%x\t write:%x\t read:%x\n", sp, cm->cm_write, cm->cm_read);
	cm->is_empty = sp - cm->cm_read > 0 ? cm->is_empty : (!cm->is_empty);
	cm->cm_read = sp;
	fprintf(stderr, "[Spill]write:%x\t read:%x\n", cm->cm_write, cm->cm_read);
	GetSystemTime(&end_time);
	int dif_time = 1000 * (end_time.wSecond - now.wSecond) + end_time.wMilliseconds - now.wMilliseconds;
	printf("[INFO]spill��ʱΪ��%d����\n", dif_time);
	//����spillʱ��
	recordSpillTime(dif_time);
	is_spill = false;
	return 0;
}