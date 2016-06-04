// SpillAlgorithm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//全局变量
cMemory *cm;

//函数声明
float spillPercentage();
unsigned int __stdcall leeMap();
unsigned int __stdcall spill();



int main(int argc, char *argv[])
{
	HANDLE 	handle;
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
	setSpillPath("E:/test");
	handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)leeMap, NULL, 0, NULL);
	WaitForSingleObject(handle, INFINITE);
	cleanCM();
	return 0;
}

/*
	adjust percent of spill
*/
float spillPercentage()
{
	int map_rate = getMapTime(),
		spill_rate = getSpillTime();
	//校验速率是否为0 是则返回初始设置/上一次设置
	if (!map_rate || !spill_rate)
	{
		return getSpillPercent();
	}
	float n_per = map_rate / (map_rate + spill_rate);
	float per = n_per > 0.5f ? n_per : 0.5f;
	fprintf(stderr, "[INFO]调整后的溢出比为：%3.2f", per);
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
	//获取自定义map
	uMap u_map = getUMP();
	unsigned int is_spill = 0;
	//spill句柄
	HANDLE spill_handle;
	//时间变量
	time_t start_time, end_time;
	//获取溢出量
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
		fprintf(stderr, "[ERROR]Path:%s not found", path);
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
	//按行读取，每个结果写环形内存（线程） 释放已处理文件
	p = head;
	//记录首次map开始时间
	time(&start_time);
	while (p != NULL)
	{
		fp = fopen(p->path, "r");
		if (fp == NULL)
		{
			printf("[ERROR]打开文件：%s 失败\n", p->path);
			p = p->next;
			continue;
		}
		else
		{
			printf("[INFO]打开文件：%s\n", p->path);
		}
		while (fgets(inmap.key, 127, fp) != NULL)
		{
			int len = strlen(inmap.key);
			if (len == 1)
			{
				continue;
			}
			head_map_link = map_link = (*u_map)(&inmap);
			//开始写环形内存 循环写入
			while (map_link != NULL)
			{
				is_spill = cmWrite(map_link->my_map);
				if (is_spill == spill_size)
				{
					//记录map时间
					time(&end_time);
					fprintf(stderr, "[INFO]map时间：%d秒\n", end_time - start_time);
					recordMapTime(end_time - start_time);
					start_time = end_time;
					spill_handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)spill, NULL, 0, NULL);
					//重新计算溢出比
					if (getAdjustSpill())
					{
						spillPercentage();
						spill_size = getSpillSize();
					}
				}
				if (is_spill == -1)
				{
					fprintf(stderr, "[Map]write:%x\t read:%x\n",cm->cm_write,cm->cm_read);
					WaitForSingleObject(spill_handle, INFINITE);
					fprintf(stderr, "[Map]write:%x\t read:%x\n", cm->cm_write, cm->cm_read);
					//线程等待
					continue;
				}
				map_link = map_link->next;
			}
			freeMapLink(head_map_link);
		}
		p = p->next;
		fclose(fp);
	}
	//把环形内存中数据 写入support文件
	strcpy(head->path, getSpillPath());
	strcat(head->path, "support");
	if ((fp = fopen(head->path, "w")) == NULL)
	{
		fprintf(stderr, "[ERROR]support写入失败\n");
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
	//释放文件链
	freeFileName(head);
	return 0;
}

//spill 过程
unsigned int __stdcall spill()
{
	fprintf(stderr, "[INFO]spill开始\n");
	//新建临时spill指针
	myMap *sp = cm->cm_read;
	//新建临时文件 创建当前时间戳文件名
	FILE *fp;
	char name[128] = "", tmp[12] = "";
	//记录spill的时间，并作为缓存文件名
	time_t now, end_time;
	time(&now);
	sprintf(name, "%sspill_%d", getSpillPath(), now);
	//写缓存文件
	if ((fp = fopen(name, "w")) == NULL)
	{
		//打开文件出错，终止程序
		fprintf(stderr, "[ERROR]打开文件：\"%s\"出错\n", name);
		return 1;
	}
	//当spill与read之差为溢出量，则结束
	int spill_size = getSpillSize();
	while (getDistance(sp, cm->cm_read) < spill_size)
	{
		fprintf(fp, "%s\t%s\n", sp->key, sp->value);
		sp = cmNext(sp, false);
	}
	fclose(fp);
	//spill结束 调整read指针,调整is_empty
	fprintf(stderr, "[Spill]spill:%x\t write:%x\t read:%x\n", sp, cm->cm_write, cm->cm_read);
	cm->is_empty = sp - cm->cm_read > 0 ? cm->is_empty : (!cm->is_empty);
	cm->cm_read = sp;
	fprintf(stderr, "[Spill]write:%x\t read:%x\n", cm->cm_write, cm->cm_read);
	time(&end_time);
	printf("[INFO]spill耗时为：%d秒\n", end_time - now);
	//设置spill时间
	recordSpillTime(end_time - now);
	return 0;
}