#include <iostream>
#include <fstream>
#include <vector>
#include<Windows.h>
#include<cstdio>

using namespace std;

const int X = 514 + 22;
const int N = 4 + (X % 5);
int global_res1 = 0;
int global_res2 = 0;
int global_size = 0;
int16_t num;


struct header {
	int32_t chunkId;
	int32_t chunkSize;
	int32_t format;
	int32_t subchunk1Id;
	int32_t subchunk1Size;
	int16_t audioFormat;
	int16_t numChannels;
	int32_t sampleRate;
	int32_t byteRate;
	int16_t blockAlign;
	int16_t bitsPerSample;
};

struct task
{
	int from;
	int to;
	int number;
};

int16_t *massiv;

HANDLE th[N];
DWORD tid[N];
task t[N];
CRITICAL_SECTION cs;

DWORD __stdcall f(void* arg)
{

	task* t = (task*)arg;

	int res1 = 0, res2 = 0;
	if (t->number == 0)
		cout << "me";
	for (int i = t->from; i < t->to; i++)
	{
		if (abs(massiv[i]) > num) res1++;
		else res2++;
	}

	EnterCriticalSection(&cs);
		global_res1 += res1;
		global_res2 += res2;
	LeaveCriticalSection(&cs);

	return 0;
}

int main() 
{
	InitializeCriticalSection(&cs);

	num = 16000;
	string patch = "C:\\Users\\SF\\Downloads\\file_example_WAV_10MG.wav";
	header hd;
	int a1 = 0, a2 = 0;
	setlocale(LC_ALL, "Russian");
	//cout << "������� ����� ��� ��������� � WAV-�����: ";
	//cin >> num;
	//cout << "������� ���� �����: ";
	//cin >> patch;
	ifstream inwav(patch, ios::binary);
	if (inwav.is_open()) 
	{
		//�������� �� ������ � 16 ���
		inwav.read((char*) &hd, sizeof(header));
		if (hd.bitsPerSample != 16) 
		{
			cout << "������ �� ����� 16 �����!";
			DeleteCriticalSection(&cs);
			return 1;
		}

		//����� ����� "data"
		char subchunk2Id[5];
		subchunk2Id[4] = '\0';
		do 
		{
			inwav.read((char*)&subchunk2Id, 4 * sizeof(char));
		} while (string(subchunk2Id) != "data");

		//����� ����� "data" ���� ������ ������� ������, ������� ��
		int32_t subchunk2Size;
		inwav.read((char*)&subchunk2Size, 4 * sizeof(char));

		global_size = subchunk2Size / 2;

		//�������� � ������������� ������� ������

		massiv = new int16_t[global_size];
		inwav.read(reinterpret_cast<char*>(massiv), global_size*sizeof(int16_t));
		
		//������ � ��������

		cout << "������: 514\n"
			<< "����� � ������ : 22\n"
			<< "������ ����������������: " << X % 4 << endl
			<< "���������� �������: " << N << endl << endl;
		
		//������ � �������� ������
		//��� ����������
		for (int i = 0; i < N; i++)
		{
			t[i].from = i * global_size / N;
			t[i].to = (i + 1) * global_size / N;
			t[i].number = i;
			th[i] = CreateThread(NULL, 0, f, (void*)&t[i], 0, &tid[i]);
		}
		WaitForMultipleObjects(N, th, true, INFINITE);
		//����� ���������

		//��� ��������� � ������
		inwav.close();
		cout << "|a|>" << num << " : " << global_res1 << endl
			 << "|a|<=" << num << " : " << global_res2;
	}

	DeleteCriticalSection(&cs);
}