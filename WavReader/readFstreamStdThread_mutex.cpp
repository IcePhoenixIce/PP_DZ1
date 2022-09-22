#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>

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
task t[N];
mutex mut;

void __stdcall f(void* arg)
{

	task* t = (task*)arg;

	int res1 = 0, res2 = 0;
	for (int i = t->from; i < t->to; i++)
	{
		if (abs(massiv[i]) > num) res1++;
		else res2++;
	}
	mut.lock();
		global_res1 += res1;
		global_res2 += res2;
	mut.unlock();
	return;
}

int main() 
{
	num = 16000;
	string patch = "C:\\Users\\SF\\Downloads\\file_example_WAV_10MG.wav";
	header hd;
	int a1 = 0, a2 = 0;
	setlocale(LC_ALL, "Russian");
	//cout << "Введите число для сравнения в WAV-файле: ";
	//cin >> num;
	//cout << "Введите путь файла: ";
	//cin >> patch;
	ifstream inwav(patch, ios::binary);
	if (inwav.is_open()) 
	{
		//Проверка на отсчет в 16 бит
		inwav.read((char*) &hd, sizeof(header));
		if (hd.bitsPerSample != 16) 
		{
			cout << "Отсчет не равен 16 битам!";
			return 1;
		}

		//Поиск чанка "data"
		char subchunk2Id[5];
		subchunk2Id[4] = '\0';
		do 
		{
			inwav.read((char*)&subchunk2Id, 4 * sizeof(char));
		} while (string(subchunk2Id) != "data");

		//После чанка "data" идет размер области данных, считаем ее
		int32_t subchunk2Size;
		inwav.read((char*)&subchunk2Size, 4 * sizeof(char));

		global_size = subchunk2Size / 2;

		//Создание и инициализация вектора данных

		massiv = new int16_t[global_size];
		inwav.read(reinterpret_cast<char*>(massiv), global_size*sizeof(int16_t));
		
		//Данные о варианте

		cout << "Группа: 514\n"
			<< "Номер в списке : 22\n"
			<< "Способ распараллеливаия: " << X % 4 << endl
			<< "Количество потоков: " << N << endl << endl;
		
		//Работа с вектором данных
		//Тут параллелим
		vector<thread> th;
		for (int i = 0; i < N; i++)
		{
			t[i].from = i * global_size / N;
			t[i].to = (i + 1) * global_size / N;
			t[i].number = i;
			th.push_back(thread(f, (void*)&t[i]));
		}
		//Конец параллели
		for (int i = 0; i < N; i++)
			th[i].join();
		//все закрываем и чистим
		inwav.close();
		delete[] massiv;
		cout << "|a|>" << num << " : " << global_res1 << endl
			 << "|a|<=" << num << " : " << global_res2;
	}

}