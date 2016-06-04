// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef linux  
#include <unistd.h>  
#include <dirent.h>  
#endif  
#ifdef WIN32  
#include <direct.h>  
#include <io.h>  
#include <process.h>
#include <windows.h>
#endif 

#include "conf.h"
#include "mapreduce.h"
#include "cmemory.h"


// TODO:  在此处引用程序需要的其他头文件
