#pragma once
//https://github.com/mistydemeo/pmdmini
#include <dsound.h>
#include <thread>
#include "pmdwin\pmdwinimport.h"

#define PMDPLAYER_MAX_VOLUME	100
const TCHAR sectionname[] = TEXT("PMDPlayer"), profilename[] = TEXT(".\\pmdplay.ini"),
varstring_notifycount[] = TEXT("NotifyCount"), varstring_bufferblocktime[] = TEXT("BufferBlockTimeMS");

class XAPlayer
{
public:
	void Init(int nChannel, int sampleRate, int bytesPerVar, int onebufbytes);
	void Release();
	void *LockBuffer(DWORD length);
	void UnlockBuffer();
	void Play();
	void Stop();
	void SetVolume(long v);
	long GetVolume();
	HRESULT SetPlaybackSpeed(float);
	void WaitForBufferEndEvent();
private:
	DWORD m_bufbytes, writecursor = 0;
	DWORD lockedBufferBytes;
	void *pLockedBuffer;
	HANDLE hBufferEndEvent;
	IDirectSound8 *pDirectSound;
	IDirectSoundBuffer *pBuffer;
	IDirectSoundNotify *pNotify;
	WAVEFORMATEX w;
};

class PMDPlayer
{
public:
	//nChannel: ͨ������1Ϊ��������2Ϊ������
	//sampleRate: �����ʣ�ÿ����ٲ�����
	//bytesPerVar: һ���������һ��ͨ��ռ�����ֽ�
	//buffer_time_ms: ���룬��ʾbuffer��ʾ�೤ʱ�������
	//�������
	void Init(int nChannel, int sampleRate, int bytesPerVar, int buffer_time_ms);
	//��������
	void Release();
	//���ļ����أ��ɹ�����0
	int LoadFromFile(const char* filepath);
	//���ڴ���أ��ɹ�����0
	int LoadFromMemory(uchar* pdata, int length);
	//���ļ����ؽ�������
	bool LoadRhythmFromDirectory(char* dir);
	//���ڴ���ؽ�������
	bool LoadRhythmFromMemory(char* bd, char* sd, char* top, char* hh, char* tom, char* rim);
	//ת��PMD��ʽ�ļ���WAV
	bool Convert(char *srcfile, char *outfile, int loops, int fadetime);
	//���ţ�û�м���ʱ����-1������Ϊ0
	int Play();
	//��ͣ���������Ѵ�����ͣ״̬ʱ����-1������Ϊ0
	int Pause();
	//���ñ�Ƶ�Ĳ����ٶȿ��ƣ�1Ϊԭ�٣��ɹ�����0������ΪDSound������
	HRESULT SetPlaybackSpeed(float);
	//��ȡ��ǰ�Ĳ����ٶȱ���
	float GetPlaybackSpeed();
	//����������0��100(PMDPLAYER_MAX_VOLUME)��
	void SetVolume(int);
	//��ȡ������0��100(PMDPLAYER_MAX_VOLUME)��
	int GetVolume();
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
	enum PlayerStatus { nofile, paused, playing, fadingout, fadedout };
	//��ȡ����״̬
	PlayerStatus GetPlayerStatus();
	const int *GetKeyVoice();
	const int *GetKeyVolume();

	//���̵߳���
	static void _Subthread_Playback(PMDPlayer* param);
	//���̲߳��ź���
	void _LoopPlayback();
protected:
	void OnPlay();
	void OnFadingOut();
private:
	unsigned length_in_ms, loop_in_ms;
	PlayerStatus playerstatus;
	unsigned fadingout_end_time_sec;
	int bytesof_soundbuffer;//�ֽڳ���

	int m_channels;
	int m_bytesPerVar;
	int m_sampleRate;
	int keyState[NumOfAllPart];//����
	int voiceState[NumOfAllPart];//��ɫ
	int volumeState[NumOfAllPart];//����
	float playbackspeed;
	uchar* pSourceData;
	int lengthSourceData;
	XAPlayer x;
	std::thread tSubPlayback;
};

