#pragma once

#define NTL "name_too_long"

//struct configuration
struct lee_conf
{
	bool show_path_in;		//是否显示输入文件
	float spill_percent;   //溢出比
	int cm_size;	//环形内存大小
	char in_path[128];	  //设置输入目录
	char out_path[128];	  //设置输出目录
	char spill_path[128];	//设置spill临时目录
	bool adjust_spill;	  //是否应用动态溢出比
};

//获取配置
bool getShowPathIn();
float getSpillPercent();
int getCMSize();
char *getInPath();
char *getOutPath();
char *getSpillPath();
bool getAdjustSpill();
int getSpillSize();
//更改配置
int initConf();
int setShowPathIn(bool is_show);
int setSpillPercent(float percent);
int setCMSize(int cm_size);
int setInPath(char path[]);
int setOutPath(char path[]);
int setSpillPath(char path[]);
int setAdjustSpill(bool adjust);