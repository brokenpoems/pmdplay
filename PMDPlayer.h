#pragma once
//https://github.com/mistydemeo/pmdmini
#include <xaudio2.h>
#include <thread>
#include "pmdwin\pmdwinimport.h"

#define PMDPLAYER_MAX_VOLUME	100.0f

class XASCallback :public IXAudio2VoiceCallback
{
public:
	HANDLE hBufferEndEvent;
	XASCallback();
	~XASCallback();
	void OnBufferEnd(void *)override;
	void OnBufferStart(void*)override {}
	void OnLoopEnd(void*)override {}
	void OnStreamEnd()override {}
	void OnVoiceError(void*,HRESULT)override {}
	void OnVoiceProcessingPassEnd()override {}
	void OnVoiceProcessingPassStart(UINT32)override {}
};
//https://github.com/lxfly2000/XAPlayer
class XAPlayer
{
public:
	XAPlayer(int nChannel, int sampleRate, int bytesPerVar);
	~XAPlayer();
	void Init(int nChannel, int sampleRate, int bytesPerVar);
	void Release();
	void Play(BYTE* buf, int length);
	void SetVolume(float v);
	float GetVolume();
	int SetPlaybackSpeed(float);
	int GetQueuedBuffersNum();
	void WaitForBufferEndEvent();
private:
	IXAudio2*xAudio2Engine;
	IXAudio2MasteringVoice* masterVoice;
	IXAudio2SourceVoice* sourceVoice;
	XAUDIO2_BUFFER xbuffer;
	XAUDIO2_VOICE_STATE state;
	XASCallback xcallback;
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
	//���ñ�Ƶ�Ĳ����ٶȿ��ƣ�1Ϊԭ�٣��ɹ�����0������ΪXAudio2������
	int SetPlaybackSpeed(float);
	//��ȡ��ǰ�Ĳ����ٶȱ���
	float GetPlaybackSpeed();
	//����������0��100.0f(PMDPLAYER_MAX_VOLUME)��
	void SetVolume(float);
	//��ȡ������0��100.0f(PMDPLAYER_MAX_VOLUME)��
	float GetVolume();
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
	short* soundbuffer;//PMD_Renderer�Ǹ������õ�������short�ұ�ʾ������⡭��
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

