#pragma once
//https://github.com/mistydemeo/pmdmini
#include <xaudio2.h>
#include "pmdwin\pmdwinimport.h"

//https://github.com/lxfly2000/XAPlayer
class XAPlayer
{
public:
	XAPlayer(int nChannel, int sampleRate, int bytesPerVar);
	~XAPlayer();
	void Init(int nChannel, int sampleRate, int bytesPerVar);
	void Release();
	void Play(BYTE* buf, int length);
	int GetQueuedBuffersNum();
private:
	IXAudio2*xAudio2Engine;
	IXAudio2MasteringVoice* masterVoice;
	IXAudio2SourceVoice* sourceVoice;
	XAUDIO2_BUFFER xbuffer;
	XAUDIO2_VOICE_STATE state;
};

class PMDPlayer
{
public:
	//nChannel: ͨ������1Ϊ��������2Ϊ������
	//sampleRate: �����ʣ�ÿ����ٲ�����
	//bytesPerVar: һ���������һ��ͨ��ռ�����ֽ�
	//buffer_time_ms: ���룬��ʾbuffer��ʾ�೤ʱ�������
	PMDPlayer(int nChannel, int sampleRate, int bytesPerVar, int buffer_time_ms);
	~PMDPlayer();
	//�������
	void Init(int nChannel, int sampleRate, int bytesPerVar, int buffer_time_ms);
	//��������
	void Release();
	//���ļ����أ��ɹ�����0
	int LoadFromFile(const char* filepath);
	//���ڴ���أ��ɹ�����0
	int LoadFromMemory(uchar* pdata, int length);
	//���ţ�û�м���ʱ����-1������Ϊ0
	int Play();
	//��ͣ���������Ѵ�����ͣ״̬ʱ����-1������Ϊ0
	int Pause();
	//ֹͣ���ͷ�
	void Unload();
	//����ֹͣ��δ����ʱ����-1
	int FadeoutAndStop(int time_ms);
	//��ȡ����״̬
	const int *GetKeysState();
	//��ȡ�ĵ�Ԫ�����е���Ϣ���ɹ�����0�����û�м����ļ����øú���ʱ����-1
	//AL	���x
	//-2	#PPZFile(1,2���á�MML��ӛ�����������Ф��Τޤ�)
	//-1	#PPSFile
	// 0	#PCMFile �ޤ��� #PPCFile
	// 1	#Title
	// 2	#Composer
	// 3	#Arranger
	// 4	#Memo(1��Ŀ)
	// 5	#Memo(2��Ŀ)
	//����	����
	int GetNotes(char* outstr, int al);
	//��ȡ��ʱ��
	unsigned GetLengthInMs();
	//��ȡ��ǰλ��
	unsigned GetPositionInMs();
	//��ȡѭ������
	unsigned GetLoopLengthInMs();
	//��ȡѭ������
	int GetLoopedTimes();
	//��ȡ�ٶ�
	int GetTempo();
	//��ȡ��ǰTick
	int GetPositionInCount();
	//��ȡС�ڳ���
	int GetXiaojieLength();
	enum PlayerStatus { nofile, paused, playing, fadingout };
	//��ȡ����״̬
	PlayerStatus GetPlayerStatus();

	//���̵߳���
	static void _Subthread_Playback(void* param);
	//���̲߳��ź���
	void _LoopPlayback();
protected:
	void OnPlay();
	void OnFadingOut();
private:
	unsigned length_in_ms, loop_in_ms;
	PlayerStatus playerstatus;
	unsigned fadingout_end_time_sec;
	short* soundbuffer;//PMD_Renderer�Ǹ������õ�������short�ұ�ʾ������⡭��
	int bytesof_soundbuffer;//�ֽڳ���

	int m_channels;
	int m_bytesPerVar;
	int keyState[NumOfAllPart];
	uchar* pSourceData;
	int lengthSourceData;
	XAPlayer x;
	static bool subthread_on;
};

