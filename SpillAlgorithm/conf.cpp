#include "stdafx.h"

//global variables
static lee_conf myconf = { true,0.8f,1,"./","./","./",true };

int addBackSlant(char *path);

//获取配置
bool getShowPathIn()
{
	return myconf.show_path_in;
}
float getSpillPercent()
{
	return myconf.spill_percent;
}
int getCMSize()
{
	return myconf.cm_size;
}
char *getInPath()
{
	return myconf.in_path;
}
char *getOutPath()
{
	return myconf.out_path;
}
char *getSpillPath()
{
	return myconf.spill_path;
}
bool getAdjustSpill() 
{
	return myconf.adjust_spill;
}
//更改配置
int initConf()
{
	myconf.adjust_spill = true;
	myconf.cm_size = 1;
	myconf.show_path_in = true;
	myconf.spill_percent = 0.8f;
	strcpy(myconf.in_path, "./");
	strcpy(myconf.out_path, "./");
	strcpy(myconf.spill_path, "./");
	return 0;
}

int setShowPathIn(bool is_show)
{
	myconf.show_path_in = is_show;
	return 0;
}
int setSpillPercent(float percent)
{
	myconf.spill_percent = percent;
	return 0;
}
int setCMSize(int cm_size) 
{
	myconf.cm_size = cm_size;
	return 0;
}
int setInPath(char path[])
{
	char in_path[128];
	strcpy(in_path, path);
	if (addBackSlant(in_path))
	{
		return 1;
	}
	strcpy(myconf.in_path, in_path);
	return 0;
}
int setOutPath(char path[])
{
	char out_path[128];
	strcpy(out_path, path);
	if (addBackSlant(out_path))
	{
		return 1;
	}
	strcpy(myconf.out_path, out_path);
	return 0;
}
int setSpillPath(char path[])
{
	char spill_path[128];
	strcpy(spill_path, path);
	if (addBackSlant(spill_path))
	{
		return 1;
	}
	strcpy(myconf.spill_path, spill_path);
	return 0;
}
int setAdjustSpill(bool adjust)
{
	myconf.adjust_spill = adjust;
	return 0;
}

//为路径自动添加“/”
int addBackSlant(char *path)
{
	if (!strlen(path))
	{
		fprintf(stderr, "输入有误,请检查\n");
		return 1;
	}
	char* str_end = path + strlen(path);
	if (strstr(str_end - 2,"/") == NULL && strstr(str_end - 3,"\\") == NULL)
	{
		strcat(path, "/");
	}
	return 0;
}

//获取溢出量
int getSpillSize()
{
	//溢出量
	int spill_size = (getCMSize() * 1024 * 1024) / sizeof(myMap) * getSpillPercent();
	return spill_size;
}