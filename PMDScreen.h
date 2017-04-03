#pragma once
class PMDScreen
{
public:
	PMDScreen();
	//���Ƽ���ͼ
	void Draw();
	//pKeys: ���ñ�ʾ���������飬num: ����Ҫ����ǰ���ٸ�ͨ��
	void SetKeyNotesSrc(const int *pkeys, int num);
	//���û�ͼ���������ָ���߶ȵĻ�����Զ�����׼��������߶�
	void SetRectangle(int _x, int _y, int _w, int _h = 0);
	//���û�ͼ���������ָ���߶ȵĻ�����Զ�����׼��������߶�
	void SetRectangle(float _x, float _y, float _w, float _h = 0.0f);
	//���ð׼���ɫ����֧��͸�� (0x00RRGGBB)
	void SetWhiteKeyColor(int color);
	//���úڼ���ɫ����֧��͸�� (0x00RRGGBB)
	void SetBlackKeyColor(int color);
	//���ð׼�����ʱ����ɫ����֧��͸�� (0x00RRGGBB)
	void SetWhiteKeyPressedColor(int color);
	//���úڼ�����ʱ����ɫ����֧��͸�� (0x00RRGGBB)
	void SetBlackKeyPressedColor(int color);
private:
	//���ư׼�
	void DrawWhiteKey();
	//���ƺڼ�
	void DrawBlackKey();
	//��ȡ�׼���ţ��������᷵�� -1, takeRightVar Ϊ true ʱ�������Ҳ�������ȷ�����
	int GetNumWhiteKey(int n, bool takeRightVar = false);
	//��ȡ�ڼ���ţ��������᷵�� -1.��ע���м�û�кڼ��Ĳ���Ҳ����һ��������ڳ�λ��
	int GetNumBlackKey(int n);
	int x, y, w, h, drawLength_keyWhite, drawWidth_keyWhite, start_keyBlackX, drawWidth_keyBlack, drawLength_keyBlack;
	int width_avgKey;
	int colorWhiteKey, colorBlackKey, colorWhiteKeyPressed, colorBlackKeyPressed;
	static int tableWhiteKey[];
	static int tableBlackKey[];

	const int *channel_key_notes;//��ͨ�����µļ�
	int num_channel;//Ҫ����ǰ���ٸ�ͨ��
};

