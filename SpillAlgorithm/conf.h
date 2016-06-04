#pragma once

#define NTL "name_too_long"

//struct configuration
struct lee_conf
{
	bool show_path_in;		//�Ƿ���ʾ�����ļ�
	float spill_percent;   //�����
	int cm_size;	//�����ڴ��С
	char in_path[128];	  //��������Ŀ¼
	char out_path[128];	  //�������Ŀ¼
	char spill_path[128];	//����spill��ʱĿ¼
	bool adjust_spill;	  //�Ƿ�Ӧ�ö�̬�����
};

//��ȡ����
bool getShowPathIn();
float getSpillPercent();
int getCMSize();
char *getInPath();
char *getOutPath();
char *getSpillPath();
bool getAdjustSpill();
int getSpillSize();
//��������
int initConf();
int setShowPathIn(bool is_show);
int setSpillPercent(float percent);
int setCMSize(int cm_size);
int setInPath(char path[]);
int setOutPath(char path[]);
int setSpillPath(char path[]);
int setAdjustSpill(bool adjust);