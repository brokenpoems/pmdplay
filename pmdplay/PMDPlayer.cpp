#include<fstream>
#include<string>
#include "../shared/PMDPlayer.h"
#include "../shared/pmdmini.h"
#pragma comment(lib,"XAudio2.lib")

#ifdef _DEBUG
#define C(e) if(e)\
if(MessageBox(NULL,__FILE__ L":" _STRINGIZE(__LINE__) "\n" __FUNCTION__ "\n" _STRINGIZE(e),NULL,MB_OKCANCEL)==IDOK)\
DebugBreak()
#else
#define C(e) e
#endif

#include <xaudio2.h>
class XASCallback :public IXAudio2VoiceCallback
{
public:
	HANDLE hBufferEndEvent;
	XASCallback();
	~XASCallback();
	void WINAPI OnBufferEnd(void *)override;
	void WINAPI OnBufferStart(void*)override {}
	void WINAPI OnLoopEnd(void*)override {}
	void WINAPI OnStreamEnd()override {}
	void WINAPI OnVoiceError(void*, HRESULT)override {}
	void WINAPI OnVoiceProcessingPassEnd()override {}
	void WINAPI OnVoiceProcessingPassStart(UINT32)override {}
};
//https://github.com/lxfly2000/XAPlayer
class XAPlayer
{
public:
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

XAPlayer x;
static short* soundbuffer;//PMD_Renderer�Ǹ������õ�������short�ұ�ʾ������⡭��

XASCallback::XASCallback()
{
	hBufferEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

XASCallback::~XASCallback()
{
	CloseHandle(hBufferEndEvent);
}

void WINAPI XASCallback::OnBufferEnd(void *p)
{
	SetEvent(hBufferEndEvent);
}

void XAPlayer::Init(int nChannel, int sampleRate, int bytesPerVar)
{
	xAudio2Engine = NULL;
	masterVoice = NULL;
	sourceVoice = NULL;
	WAVEFORMATEX w;
	w.wFormatTag = WAVE_FORMAT_PCM;
	w.nChannels = nChannel;
	w.nSamplesPerSec = sampleRate;
	w.nAvgBytesPerSec = sampleRate*bytesPerVar*nChannel;
	w.nBlockAlign = 4;
	w.wBitsPerSample = bytesPerVar * 8;
	w.cbSize = 0;
	xbuffer = { 0 };
	xbuffer.Flags = XAUDIO2_END_OF_STREAM;
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))return;
	if (FAILED(XAudio2Create(&xAudio2Engine)))return;
	if (FAILED(xAudio2Engine->CreateMasteringVoice(&masterVoice)))return;
	xAudio2Engine->CreateSourceVoice(&sourceVoice, &w, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &xcallback);
	sourceVoice->Start();
}

void XAPlayer::Release()
{
	sourceVoice->Stop();
	sourceVoice->FlushSourceBuffers();
	if (sourceVoice)sourceVoice->DestroyVoice();
	masterVoice->DestroyVoice();
	xAudio2Engine->Release();
	CoUninitialize();
}

void XAPlayer::Play(BYTE*buf, int length)
{
	xbuffer.pAudioData = buf;
	xbuffer.AudioBytes = length;
	sourceVoice->SubmitSourceBuffer(&xbuffer);
}

void XAPlayer::SetVolume(float v)
{
	masterVoice->SetVolume(v);
}

float XAPlayer::GetVolume()
{
	float v;
	masterVoice->GetVolume(&v);
	return v;
}

int XAPlayer::GetQueuedBuffersNum()
{
	sourceVoice->GetState(&state);
	return state.BuffersQueued;
}

void XAPlayer::WaitForBufferEndEvent()
{
	WaitForSingleObject(xcallback.hBufferEndEvent, INFINITE);
}

int XAPlayer::SetPlaybackSpeed(float speed)
{
	return sourceVoice->SetFrequencyRatio(speed);
}


int PMDPlayer::LoadFromFile(const char *filepath)
{
	if (!pmd_is_pmd(filepath))return -1;
	getlength((char*)filepath, (int*)&length_in_ms, (int*)&loop_in_ms);
	//http://blog.csdn.net/tulip527/article/details/7976471
	std::fstream f(filepath, std::ios::binary | std::ios::in);
	std::string sf((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	return LoadFromMemory((uchar*)sf.data(), (int)sf.size());
}

int PMDPlayer::LoadFromMemory(uchar *pdata, int length)
{
	char curdir[MAX_PATH];
	char* path[2] = { curdir,NULL };
	Unload();
	GetCurrentDirectoryA(MAX_PATH, curdir);
	setpcmdir(path);
	lengthSourceData = length;
	playerstatus = paused;
	int r = music_load2(pdata, length);
	music_start();
	pSourceData = getopenwork()->mmlbuf - 1;
	return r;
}

bool PMDPlayer::LoadRhythmFromDirectory(char* dir)
{
	return loadrhythmsample(dir);
}

bool PMDPlayer::LoadRhythmFromMemory(char* bd, char* sd, char* top, char* hh, char* tom, char* rim)
{
	return loadrhythmsample_mem(bd, sd, top, hh, tom, rim);
}

bool PMDPlayer::Convert(char *srcfile, char *outfile, int loops, int fadetime, bool splittracks)
{
	struct WaveStructure
	{
		char strRIFF[4];
		int chunkSize;
		char strFormat[4];
		char strFmt[4];
		int subchunk1Size;
		short audioFormat;
		short numChannels;
		int sampleRate;
		int byteRate;
		short blockAlign;
		short bpsample;//Bits per sample
		char strData[4];
		int subchunk2Size;//Data size���ֽ�����
	};
	WaveStructure wavfileheader=
	{
		'R','I','F','F',//strRIFF
		0,//chunkSize
		'W','A','V','E',//strFormat
		'f','m','t',' ',//strFmt
		16,//subchunk1Size
		WAVE_FORMAT_PCM,//audioFormat
		(short)m_channels,//numChannels
		m_sampleRate,//sampleRate
		m_sampleRate*m_channels*m_bytesPerVar,//byteRate
		(short)(m_channels*m_bytesPerVar),//blockAlign
		(short)(m_bytesPerVar*8),//bpsample
		'd','a','t','a',//strData
		0//subchunk2Size
	};
	if (LoadFromFile(srcfile))return false;
	if (splittracks)
	{
		char mtname[MAX_PATH];
		for(int i=0;i<10;i++)
		{
			wavfileheader.subchunk2Size = 0;
			sprintf(mtname, "%s-%c.wav", outfile, i == 9 ? 'R' : 'A' + i);
			for (int j = 0; j < 10; j++)
			{
				if (j == 9)
					setrhythmwithssgeffect(i == 9);
				else if (j == 8)
					getopenwork()->effflag = (i != 8);
				else
					j == i ? maskoff(j) : maskon(j);
			}
			setpos2(0);
			std::fstream f(mtname, std::ios::out | std::ios::binary);
			f.seekp(sizeof wavfileheader);
			int currentLoops = GetLoopedTimes();
			while (currentLoops < loops && currentLoops >= 0)
			{
				pmd_renderer(soundbuffer, bytesof_soundbuffer / m_channels / m_bytesPerVar);
				f.write((char*)soundbuffer, bytesof_soundbuffer);
				wavfileheader.subchunk2Size += bytesof_soundbuffer;
				currentLoops = GetLoopedTimes();
			}
			fadingout_end_time_sec = (GetPositionInMs() + fadetime) / 1000;
			if (currentLoops >= 0)fadeout2(fadetime);
			while (GetPositionInMs() / 1000 < fadingout_end_time_sec)
			{
				pmd_renderer(soundbuffer, bytesof_soundbuffer / m_channels / m_bytesPerVar);
				f.write((char*)soundbuffer, bytesof_soundbuffer);
				wavfileheader.subchunk2Size += bytesof_soundbuffer;
			}
			wavfileheader.chunkSize = 36 + wavfileheader.subchunk2Size;
			f.seekp(0);
			f.write((char*)&wavfileheader, sizeof wavfileheader);
		}
	}
	else
	{
		std::fstream f(outfile, std::ios::out | std::ios::binary);
		f.seekp(sizeof wavfileheader);
		int currentLoops = GetLoopedTimes();
		while (currentLoops < loops && currentLoops >= 0)
		{
			pmd_renderer(soundbuffer, bytesof_soundbuffer / m_channels / m_bytesPerVar);
			f.write((char*)soundbuffer, bytesof_soundbuffer);
			wavfileheader.subchunk2Size += bytesof_soundbuffer;
			currentLoops = GetLoopedTimes();
		}
		fadingout_end_time_sec = (GetPositionInMs() + fadetime) / 1000;
		if (currentLoops >= 0)fadeout2(fadetime);
		while (GetPositionInMs() / 1000 < fadingout_end_time_sec)
		{
			pmd_renderer(soundbuffer, bytesof_soundbuffer / m_channels / m_bytesPerVar);
			f.write((char*)soundbuffer, bytesof_soundbuffer);
			wavfileheader.subchunk2Size += bytesof_soundbuffer;
		}
		wavfileheader.chunkSize = 36 + wavfileheader.subchunk2Size;
		f.seekp(0);
		f.write((char*)&wavfileheader, sizeof wavfileheader);
	}
	Unload();
	return true;
}

int PMDPlayer::Play()
{
	if (playerstatus != paused)return -1;
	playerstatus = playing;
	tSubPlayback = std::thread(PMDPlayer::_Subthread_Playback, this);
	return 0;
}

int PMDPlayer::Pause()
{
	if (playerstatus != nofile && playerstatus != playing)return -1;
	playerstatus = paused;
	tSubPlayback.join();
	return 0;
}

int PMDPlayer::SetPlaybackSpeed(float speed)
{
	return x.SetPlaybackSpeed(playbackspeed = speed);
}

float PMDPlayer::GetPlaybackSpeed()
{
	return playbackspeed;
}

void PMDPlayer::SetVolume(int v)
{
	v = min(PMDPLAYER_MAX_VOLUME, v);
	v = max(0, v);
	x.SetVolume((float)v / PMDPLAYER_MAX_VOLUME);
}

int PMDPlayer::GetVolume()
{
	return (int)(x.GetVolume()*PMDPLAYER_MAX_VOLUME);
}

void PMDPlayer::Unload()
{
	playerstatus = nofile;
	if (tSubPlayback.joinable())tSubPlayback.join();
	pmd_stop();
}

int PMDPlayer::FadeoutAndStop(int time_ms)
{
	if (playerstatus != playing)return -1;
	fadingout_end_time_sec = (GetPositionInMs() + time_ms) / 1000;
	fadeout2(time_ms);
	playerstatus = fadingout;
	return 0;
}

const int *PMDPlayer::GetKeysState()
{
	return keyState;
}

const int *PMDPlayer::GetKeyVoice()
{
	return voiceState;
}

const int *PMDPlayer::GetKeyVolume()
{
	return volumeState;
}

int PMDPlayer::GetNotes(char *outstr, int al)
{
	if (playerstatus == nofile)return -1;
	getmemo(outstr, pSourceData, lengthSourceData, al);
	return 0;
}

unsigned PMDPlayer::GetLengthInMs()
{
	return length_in_ms;
}

unsigned PMDPlayer::GetPositionInMs()
{
	return getpos();
}

unsigned PMDPlayer::GetLoopLengthInMs()
{
	return loop_in_ms;
}

int PMDPlayer::GetLoopedTimes()
{
	return getloopcount();
}

int PMDPlayer::GetTempo()
{
	return getopenwork()->tempo_48 * 2;
}

int PMDPlayer::GetPositionInCount()
{
	return getpos2();
}

int PMDPlayer::GetXiaojieLength()
{
	return getopenwork()->syousetu_lng;
}

PMDPlayer::PlayerStatus PMDPlayer::GetPlayerStatus()
{
	return playerstatus;
}

void PMDPlayer::Init(int nChannel, int sampleRate, int bytesPerVar, int buffer_time_ms)
{
	x.Init(nChannel, sampleRate, bytesPerVar);
	m_channels = nChannel;
	m_bytesPerVar = bytesPerVar;
	m_sampleRate = sampleRate;
	if (buffer_time_ms == 0)
		buffer_time_ms = GetPrivateProfileInt(sectionname, varstring_bufferblocktime, 20, profilename);
	bytesof_soundbuffer = sampleRate*bytesPerVar*nChannel*buffer_time_ms / 1000;
	soundbuffer = reinterpret_cast<decltype(soundbuffer)>(new BYTE[bytesof_soundbuffer]);
	playerstatus = nofile;
	pmd_init();
	setppsuse(false);
	setrhythmwithssgeffect(true);
	getopenwork()->effflag = 0;
	setfmcalc55k(true);
	pmd_setrate(sampleRate);
	playbackspeed = 1.0f;
	memset(keyState, -1, sizeof keyState);
	keyState[8] = 0;
	ZeroMemory(voiceState, sizeof voiceState);
	ZeroMemory(volumeState, sizeof volumeState);
}

void PMDPlayer::Release()
{
	Unload();
	delete[]soundbuffer;
	x.Release();
}

void PMDPlayer::_Subthread_Playback(PMDPlayer *param)
{
	param->_LoopPlayback();
}

void PMDPlayer::_LoopPlayback()
{
	while (playerstatus >= playing)
	{
		switch (playerstatus)
		{
		case playing:OnPlay(); break;
		case fadingout:OnFadingOut(); break;
		}
	}
}
void PMDPlayer::OnPlay()
{
	pmd_renderer(soundbuffer, bytesof_soundbuffer / m_channels / m_bytesPerVar);
	x.Play((BYTE*)soundbuffer, bytesof_soundbuffer);
	x.WaitForBufferEndEvent();
	for (int i = 0; i < ARRAYSIZE(keyState); i++)
	{
		//*(unsigned short*)(keyState + i) = (getpartwork(i)->onkai & 0xF) + ((getpartwork(i)->onkai >> 4) * 12);
		//((unsigned char*)(keyState + i))[3] = getpartwork(i)->voicenum;
		//((unsigned char*)(keyState + i))[4] = getpartwork(i)->volume;
		//��Ȼ�����͵ģ����ñ���ֻ����һ���ֽڣ�����λ��ʾһ���˶��ڵİ�����Semitone��https://zh.wikipedia.org/wiki/���� ����
		//����λ��ʾ���ĸ��˶ȣ�Octave��https://zh.wikipedia.org/wiki/���˶� �������ʵ�ʵ������ǵ���λ������λ��12.
		//������⺦����������һ���硭��ԭ����Ҳ����ע����д���ף���ķ��ˣ���(�s�F����)�s��ߩ���
		if (getpartwork(i)->onkai == 255||getpartwork(i)->keyoff_flag==-1)
			keyState[i] = -1;
		else
			keyState[i] = (getpartwork(i)->onkai & 0xF) + ((getpartwork(i)->onkai >> 4) * 12);
		voiceState[i] = getpartwork(i)->voicenum;
		volumeState[i] = getpartwork(i)->volume;
	}
	if (!getopenwork()->effflag)keyState[8] = getopenwork()->kshot_dat;// % 128;//SSG����
}

void PMDPlayer::OnFadingOut()
{
	OnPlay();
	if (GetPositionInMs() / 1000 > fadingout_end_time_sec)
		playerstatus = fadedout;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	extern int pmdplayMain(wchar_t*);
	return pmdplayMain(lpCmdLine);
}
