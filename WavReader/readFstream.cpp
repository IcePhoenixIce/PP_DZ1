#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

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
	int32_t subchunk2Id;
	int32_t subchunk2Size;
};

int main() 
{
	int16_t num =16000;
	string patch= "C:\\Users\\SF\\Downloads\\file_example_WAV_10MG.wav";
	header hd;
	int a1 = 0, a2 = 0;
	setlocale(LC_ALL, "Russian");
	cout << "¬ведите число дл€ сравнени€ в WAV-файле: ";
	//cin >> num;
	cout << "¬ведите путь файла: "; //"C:\\Users\\SF\\Downloads\\file_example_WAV_10MG.wav"
	//cin >> patch;
	ifstream inwav(patch, ios::binary); //3sept_vo.wav 64 Mb || file_example_WAV_10MG.wav 10 Mb
	if (inwav.is_open()) 
	{
		inwav.read((char*) &hd, sizeof(header));
		if (hd.bitsPerSample != 16) 
		{
			cout << "ќтсчет не равен 16 битам!";
			return 1;
		}
		int size_16_t = hd.subchunk2Size / 2;
		vector<int16_t> vec(size_16_t);
		inwav.read(reinterpret_cast<char*>(vec.data()), vec.size() * sizeof(int16_t));
		for (int i = 0; i < size_16_t; i++)
		{
			if (abs(vec[i]) > num)
				a1++;
			else
				a2++;
		}
		inwav.close();
		vec.clear();
		cout << "|a|>" << num << " : " << a1 << endl
			 << "|a|<" << num << " : " << a2;
	}
		
}