#pragma once
#include<Windows.h>
static const TCHAR projectURL[] = TEXT("https://github.com/lxfly2000/pmdplay");
static const TCHAR updateFileURL[] = TEXT("https://github.com/lxfly2000/pmdplay/raw/master/shared/resource.h");
//�����£������ظ������޸��·���0���и��·���1
int CheckForUpdate(const TCHAR *fileURL, int *newVersion);
