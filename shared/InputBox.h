#pragma once
#include<Windows.h>
//��ʾ����Ի��򣬰�ȷ������ 1, ȡ������ 0;
//ָ�� textExtraButton ʱ����ʾ��������ť����ָ��������ʾ�����¸ð�ť�󷵻� 2;
//maxlen Ϊ -1 ʱ��ʾ���� pszOut �ĳ��ȣ��õ����ַ�����󳤶�Ϊ maxlen - 1;
//���۰��ĸ���ť pszOut ���᷵��������ַ�����
int InputBox(HWND hwndParent, PTSTR pszOut, int maxlen, PCTSTR msgText, PCTSTR title, PCTSTR defaultInput = NULL,
	PCTSTR textExtraButton = NULL, PCTSTR textOK = NULL, PCTSTR textCancel = NULL);