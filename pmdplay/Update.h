#pragma once
#include<Windows.h>
#define PROJECT_URL		"https://github.com/lxfly2000/pmdplay"
#define UPDATE_FILE_URL	"https://github.com/lxfly2000/pmdplay/raw/master/shared/resource.h"
//�����£������ظ������޸��·���0���и��·���1
int CheckForUpdate(const TCHAR *fileURL, int *newVersion);
