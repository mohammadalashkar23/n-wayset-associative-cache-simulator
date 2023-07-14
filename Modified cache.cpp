#include <iostream>
#include  <iomanip>
#include <vector>
#include <random>
#include <ctime>
using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(16*1024)//Change

enum cacheResType { MISS = 0, HIT = 1 };

/* The following implements a random number generator */
unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
	static unsigned int addr = 0;
	return (addr++) % (DRAM_SIZE);
}

unsigned int memGen2()
{
	static unsigned int addr = 0;
	return  rand_() % (24 * 1024);
}

unsigned int memGen3()
{
	return rand_() % (DRAM_SIZE);
}

unsigned int memGen4()
{
	static unsigned int addr = 0;
	return (addr++) % (4 * 1024);
}

unsigned int memGen5()
{
	static unsigned int addr = 0;
	return (addr++) % (1024 * 64);
}

unsigned int memGen6()
{
	static unsigned int addr = 0;
	return (addr += 32) % (64 * 4 * 1024);
}

//// Direct Mapped Cache Simulator
//cacheResType cacheSimDM(unsigned int addr)
//{
//	// This function accepts the memory address for the memory transaction and 
//	// returns whether it caused a cache miss or a cache hit
//
//	// The current implementation assumes there is no cache; so, every transaction is a miss
//	return MISS;
//}
//
//
//// Fully Associative Cache Simulator
//cacheResType cacheSimFA(unsigned int addr)
//{
//	// This function accepts the memory address for the read and 
//	// returns whether it caused a cache miss or a cache hit
//
//	// The current implementation assumes there is no cache; so, every transaction is a miss
//	return MISS;
//}
cacheResType cacheSim_set_associative(unsigned int addr, vector<vector<unsigned>>& cache, int index_length, int tag_length, int offset_length, int ways, int Data_size)
{
	unsigned int temp = (pow(2, index_length) - 1);
	unsigned int tag = addr >> (index_length + offset_length);
	unsigned int index = (addr >> offset_length) & temp;
	bool not_coldstart = false;
	int number_of_initized_slotsinset = 0;
	for (int i = 0; i < ways; i++)
	{
		if (cache[index][i * (Data_size + 2)] == 1)
		{
			not_coldstart = true;
			number_of_initized_slotsinset++;
		}
	}
	if (not_coldstart)
	{
		for (int i = 0; i < ways; i++)
		{
			if (cache[index][(i * (Data_size + 2)) + 1] == tag && cache[index][i * (Data_size + 2)] == 1)
			{
				return HIT;
			}
		}
		if (number_of_initized_slotsinset == ways)
		{
			std::srand(std::time(nullptr));

			int randomNum = std::rand() % ways;
			cache[index][(randomNum * (Data_size + 2)) + 1] = tag;
			cache[index][(randomNum * (Data_size + 2))] = 1;
		}
		else {
			for (int i = 0; i < ways; i++)
			{
				if (cache[index][i * (Data_size + 2)] == 0)
				{
					cache[index][(i * (Data_size + 2)) + 1] = tag;
					cache[index][(i * (Data_size + 2))] = 1;
					break;
				}
			}
		}
		return MISS;
	}
	else
	{
		cache[index][1] = tag;
		cache[index][0] = 1;
		return MISS;
	}
	return MISS;
}

char msg[2] = { 'M','H' };

#define		NO_OF_Iterations	10000	// CHange to 1,000,000
int main()
{
	int Cache_line_size;
	int ways;
	int set_number;
	int Data_size;//data size in one cach line in a set (unit is unsigned integer)
	int cach_width;
	cout << " Enter Cache line size (16, 32, 64, and 128 bytes)" << endl;
	cin >> Cache_line_size;
	cout << " Number of ways (1, 2, 4,8and 16)" << endl;
	cin >> ways;

	if (Cache_line_size != 16 && Cache_line_size != 32 && Cache_line_size != 64 && Cache_line_size != 128)
	{
		cout << "wrong Cache line size";
		exit(0);
	}
	if (ways != 1 && ways != 2 && ways != 4 && ways != 8 && ways != 16)
	{
		cout << "wrong Cache line size";
		exit(0);
	}
	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	Data_size = Cache_line_size / 4;
	cach_width = ((Data_size + 2) * ways);
	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	int index_length = log2(set_number);
	int offset_length = log2(Cache_line_size);
	int tag_length = 26 - index_length - offset_length;


	//unsigned int hit = 0;
	//cacheResType r;

	//unsigned int addr;
	//cout << "Direct Mapped Cache Simulator\n";

	//for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//{
	//	addr = memGen2();
	//	r = cacheSimDM(addr);
	//	if (r == HIT) hit++;
	//	cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//}
	//cout << "Hit ratio = " << (100 * hit / NO_OF_Iterations) << endl;

	unsigned int hit = 0;
	cacheResType r;

	unsigned int addr;
	cout << "set Cache Simulator\n";

	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	{
		addr = memGen6();
		r = cacheSim_set_associative(addr, cache, index_length, tag_length, offset_length, ways, Data_size);
		if (r == HIT) hit++;
		cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	}
	cout << dec << hit << endl;
	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << endl;
}