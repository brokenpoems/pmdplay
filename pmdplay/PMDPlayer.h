#pragma once
//https://github.com/mistydemeo/pmdmini
#include <thread>

#define SECTION_NAME	"PMDPlayer"
#define PROFILE_NAME	".\\pmdplay.ini"

#define PMDPLAYER_CHANNELS_NUM	24
#define PMDPLAYER_MAX_VOLUME	100

#ifdef _DEBUG
#define C(e) if(e)\
if(MessageBox(NULL,__FILE__ L":" _STRINGIZE(__LINE__) "\n" __FUNCTION__ "\n" _STRINGIZE(e),NULL,MB_OKCANCEL)==IDOK)\
DebugBreak()
#else
#define C(e) e
#endif

class PMDPlayer
{
public:
	//nChannel: ͨ������1Ϊ��������2Ϊ������
	//sampleRate: �����ʣ�ÿ����ٲ�����
	//bytesPerVar: һ���������һ��ͨ��ռ�����ֽ�
	//buffer_time_ms: ���룬��ʾbuffer��ʾ�೤ʱ������ݣ����Ϊ0��ʾʹ��Ĭ��ֵ
	//������ã��ɹ�����0��β��׷��
	virtual int Init(int nChannel, int sampleRate, int bytesPerVar, int buffer_time_ms);
	//�������ã��ɹ�����0��β��׷��
	virtual int Release();
	//���ļ����أ��ɹ�����0
	int LoadFromFile(const char* filepath);
	//���ڴ���أ��ɹ�����0
	int LoadFromMemory(unsigned char* pdata, int length);
	//���ļ����ؽ�������
	bool LoadRhythmFromDirectory(char* dir);
	//���ڴ���ؽ�������
	bool LoadRhythmFromMemory(char* bd, char* sd, char* top, char* hh, char* tom, char* rim);
	//ת��PMD��ʽ�ļ���WAV
	bool Convert(char *srcfile, char *outfile, int loops, int fadetime, bool splittracks);
	//���ţ�û�м���ʱ����-1������Ϊ0��ͷ��׷��
	virtual int Play();
	//��ͣ���������Ѵ�����ͣ״̬ʱ����-1������Ϊ0��β��׷��
	virtual int Pause();
	//���ñ�Ƶ�Ĳ����ٶȿ��ƣ�1Ϊԭ�٣��ɹ�����0������ΪDSound�����룬ֱ�Ӹ���
	virtual int SetPlaybackSpeed(float);
	//��ȡ��ǰ�Ĳ����ٶȱ��ʣ�ֱ�Ӹ���
	virtual float GetPlaybackSpeed();
	//����������0��100(PMDPLAYER_MAX_VOLUME)����ֱ�Ӹ���
	virtual void SetVolume(int);
	//��ȡ������0��100(PMDPLAYER_MAX_VOLUME)����ֱ�Ӹ���
	virtual int GetVolume();
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
	//���ò���λ��
	void SetPositionInMs(int);
	//����/�ر�ͨ��
	void SetChannelOn(int, bool);
	//����/�ر�SSG��������
	void SetSSGEffectOn(bool);
	bool GetSSGEffectOn();
	//����/�ر�YM2608��������
	void SetRhythmOn(bool);
	//ֹͣ���ţ�����λ�ñ����õ���ʼ����β��׷��
	virtual int Stop();

	//���̵߳���
	static void _Subthread_Playback(PMDPlayer* param);
	//���̲߳��ź���
	void _LoopPlayback();
protected:
	//ͷ��׷��
	virtual void OnPlay();
	void OnFadingOut();
	int bytesof_soundbuffer;//�ֽڳ���

	float playbackspeed;
	int m_channels;
	int m_bytesPerVar;
private:
	unsigned length_in_ms, loop_in_ms;
	PlayerStatus playerstatus;
	unsigned fadingout_end_time_sec;

	int m_sampleRate;
	int keyState[PMDPLAYER_CHANNELS_NUM];//����
	int voiceState[PMDPLAYER_CHANNELS_NUM];//��ɫ
	int volumeState[PMDPLAYER_CHANNELS_NUM];//����
	unsigned char* pSourceData;
	int lengthSourceData;
	std::thread tSubPlayback;
};

