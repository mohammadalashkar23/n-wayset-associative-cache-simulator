#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <ctime>
#include <cmath>
using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(16*1024)

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
//n way set associative cache simulator
cacheResType cacheSim_set_associative(unsigned int addr, vector<vector<unsigned>>& cache, int index_length, int offset_length, int ways, int Data_size, vector<unsigned int>& counters)
{
	//masking the tag ,index 
	unsigned int temp = (pow(2, index_length) - 1);
	unsigned int tag = addr >> (index_length + offset_length);
	unsigned int index = (addr >> offset_length) & temp;
	//boolean to make sure that we do not have coldstart miss
	bool not_coldstart = false;
	// this int is used to know if all valiad bits are true or not
	int number_of_initized_slotsinset = 0;
	//this for loop to initialize the not_coldstart and the number_of_initized_slotsinset
	for (int i = 0; i < ways; i++)
	{
		if (cache[index][i * (Data_size + 2)] == 1)
		{
			not_coldstart = true;
			number_of_initized_slotsinset++;
		}
	}
	//if it is clodstart it is miss and update the valaid bit and the tag
	if (not_coldstart)
	{
		//if the valiad bit is true, check if it is the same tage then it is hit 
		for (int i = 0; i < ways; i++)
		{
			if (cache[index][(i * (Data_size + 2)) + 1] == tag && cache[index][i * (Data_size + 2)] == 1)
			{
				return HIT;
			}
		}
		//if it is not hit, return miss and decide where to put the new tage and put the valiad bit 1
		if (number_of_initized_slotsinset == ways)
		{
			int Num = counters[index] % ways;
			cache[index][(Num * (Data_size + 2)) + 1] = tag;
			cache[index][(Num * (Data_size + 2))] = 1;
			++counters[index];
		}
		else {
			for (int i = 0; i < ways; i++)
			{
				if (cache[index][i * (Data_size + 2)] == 0)
				{
					cache[index][(i * (Data_size + 2)) + 1] = tag;
					cache[index][(i * (Data_size + 2))] = 1;
					++counters[index];
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
		++counters[index];
		return MISS;
	}
	return MISS;
}

string msg[2] = { "Miss","Hit" }; 

#define		NO_OF_Iterations	1000000
int main()
{
	ofstream outfile;
	outfile.open("Results_Experiments.csv");
	outfile << "Hit Ratio,Miss Ratio,memGen(),ways,Cache Line Size" << endl;

	srand(time(nullptr));

	cout << "set Cache Simulator\n";

	int line_sizes[4] = { 16, 32, 64, 128 };
	int num_of_ways[5] = { 1, 2, 4, 8, 16 };
	int Cache_line_size;
	int ways;
	int set_number;
	int Data_size;//data size in one cach line in a set (unit is unsigned integer)
	int cach_width;
	int index_length;
	int offset_length;
	int tag_length;
	unsigned int hit = 0;
	unsigned int addr = 0;
	int funcNum = 1;
	//For Test Case 1
	//ways = 4;
	//Cache_line_size = 32; //5 bits offset
	//set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//Data_size = Cache_line_size / 4;
	//cach_width = ((Data_size + 2) * ways);
	//vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//vector<unsigned int> counters(set_number);
	//index_length = log2(set_number);
	//offset_length = log2(Cache_line_size);
	//tag_length = 26 - index_length - offset_length;
	//hit = 0;
	//cacheResType r;

	//unsigned int address_test[10] = { 0x00000017, 0x000000db,0x00000118,0x0000006f ,0x0000010c ,0x0000012a ,0x00000021 ,0x0000003c ,0x00000099 ,0x00000001  };
	//for (int inst = 0; inst < 10; inst++)
	//{
	//	addr = address_test[inst];
	//	r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//	if (r == HIT) hit++;
	//	cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//}
	//cout << dec << hit << endl;
	//cout << "Hit ratio = " << (100 * (double)hit / 10) << "For memGen1() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;

	//For Test Case 2
	//ways = 16;
	//Cache_line_size = 16; //5 bits offset
	//set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//Data_size = Cache_line_size / 4;
	//cach_width = ((Data_size + 2) * ways);
	//vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//vector<unsigned int> counters(set_number);
	//index_length = log2(set_number);
	//offset_length = log2(Cache_line_size);
	//tag_length = 26 - index_length - offset_length;
	//hit = 0;
	//cacheResType r;
	//unsigned int address_test[10] = { 0x0000003c,0x0000007d,0x000000f8,0x0000006c,0x000000c8,0x000000ec,0x00000033,0x00000037,0x0000009e,0x000000ca };
	//for (int inst = 0; inst < 10; inst++)
	//{
	//	addr = address_test[inst];
	//	r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//	if (r == HIT) hit++;
	//	cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//}
	//cout << dec << hit << endl;
	//cout << "Hit ratio = " << (100 * (double)hit / 10) << "For memGen1() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;

	//For Test Case 3
	//ways = 8;
	//Cache_line_size = 128; //5 bits offset
	//set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//Data_size = Cache_line_size / 4;
	//cach_width = ((Data_size + 2) * ways);
	//vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//vector<unsigned int> counters(set_number);
	//index_length = log2(set_number);
	//offset_length = log2(Cache_line_size);
	//tag_length = 26 - index_length - offset_length;
	//hit = 0;
	//cacheResType r;
	//unsigned int address_test[10] = { 0x0000003f,0x00000044,0x00000019,0x000000f6,0x00000116,0x0000004c,0x00000034,0x00000170,0x000001aa,0x00000015 };
	//for (int inst = 0; inst < 10; inst++)
	//{
	//	addr = address_test[inst];
	//	r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//	if (r == HIT) hit++;
	//	cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//}
	//cout << dec << hit << endl;
	//cout << "Hit ratio = " << (100 * (double)hit / 10) << "For memGen1() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;

	//For Test Case 4
	//ways = 2;
	//Cache_line_size = 64; //5 bits offset
	//set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//Data_size = Cache_line_size / 4;
	//cach_width = ((Data_size + 2) * ways);
	//vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//vector<unsigned int> counters(set_number);
	//index_length = log2(set_number);
	//offset_length = log2(Cache_line_size);
	//tag_length = 26 - index_length - offset_length;
	//hit = 0;
	//cacheResType r;
	//unsigned int address_test[10] = { 0x000001c9 ,0x00000076 ,0x0000015e ,0x00000081 ,0x000000bd ,0x00000079 ,0x00000050 ,0x00000069 ,0x000000ff ,0x00000004 };
	//for (int inst = 0; inst < 10; inst++)
	//{
	//	addr = address_test[inst];
	//	r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//	if (r == HIT) hit++;
	//	cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//}
	//cout << dec << hit << endl;
	//cout << "Hit ratio = " << (100 * (double)hit / 10) << "For memGen1() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;

	//For User Input
	cout << "Enter which function do you want to implement" << endl;
	cin >> funcNum;
	cout << " Enter Cache line size (16, 32, 64, and 128 bytes)" << endl;
	cin >> Cache_line_size;
	cout << " Number of ways (1, 2, 4,8and 16)" << endl;
	cin >> ways;
	
	set_number = ((CACHE_SIZE / ways) / Cache_line_size); //calculating the how many sets do we have
	Data_size = Cache_line_size / 4;
	cach_width = ((Data_size + 2) * ways); //v+tag =2 ,2+data size = width of one way so we multiply it by number of ways to get cach_width
	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));//generating the cache
	vector<unsigned int> counters(set_number); // use this vector to apply FIFO replacment policy
	index_length = log2(set_number);//dividing the address between the tag and the index and the offset
	offset_length = log2(Cache_line_size);
	tag_length = 26 - index_length - offset_length;

	hit = 0;
	cacheResType r;
	//choosing which memgen to excute
	switch (funcNum) {
	case 1: {
		for (int inst = 0; inst < NO_OF_Iterations; inst++)
		{
			addr = memGen1();
			r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
			if (r == HIT) hit++;
			cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
		}
		cout << dec << hit << endl;
		cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen1() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
		outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "1," << ways << "," << Cache_line_size << " bytes" << endl;
	}
		  break;
	case 2: {
		for (int inst = 0; inst < NO_OF_Iterations; inst++)
		{
			addr = memGen2();
			r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
			if (r == HIT) hit++;
			cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
		}
		cout << dec << hit << endl;
		cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen2() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
		outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "2," << ways << "," << Cache_line_size << " bytes" << endl;
	}
		  break;
	case 3: {
		for (int inst = 0; inst < NO_OF_Iterations; inst++)
		{
			addr = memGen3();
			r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
			if (r == HIT) hit++;
			cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
		}
		cout << dec << hit << endl;
		cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen3() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
		outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "3," << ways << "," << Cache_line_size << " bytes" << endl;
	}
		  break;
	case 4: {
		for (int inst = 0; inst < NO_OF_Iterations; inst++)
		{
			addr = memGen4();
			r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
			if (r == HIT) hit++;
			cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
		}
		cout << dec << hit << endl;
		cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen4() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
		outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "4," << ways << "," << Cache_line_size << " bytes" << endl;
	}
		  break;
	case 5: {
		for (int inst = 0; inst < NO_OF_Iterations; inst++)
		{
			addr = memGen5();
			r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
			if (r == HIT) hit++;
			cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
		}
		cout << dec << hit << endl;
		cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen5() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
		outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "5," << ways << "," << Cache_line_size << " bytes" << endl;
	}
		  break;
	case 6: {
		for (int inst = 0; inst < NO_OF_Iterations; inst++)
		{
			addr = memGen6();
			r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
			if (r == HIT) hit++;
			cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
		}
		cout << dec << hit << endl;
		cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen6() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
		outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "6," << ways << "," << Cache_line_size << " bytes" << endl;
	}
		  break;
	default: {
		cout << "Error! No valid function exist" << endl;
		exit(0); 
	}
	}

	//Experiments
	//For memGen1 part 1 
	//ways = 4;
	//for (int x = 0; x < 4; x++) {
	//	Cache_line_size = line_sizes[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen1();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen1() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "1," << ways << "," << Cache_line_size << " bytes" << endl;
	//	
	//}

	////for memGen1 part 2
	//Cache_line_size = 32;
	//for (int x = 0; x < 5; x++) {
	//	ways = num_of_ways[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen1();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen1() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "1," << ways << "," << Cache_line_size << " bytes" << endl;
	//	
	//}

	////	For memGen2 part 1 
	//ways = 4;
	//for (int x = 0; x < 4; x++) {
	//	Cache_line_size = line_sizes[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen2();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen2() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "2," << ways << "," << Cache_line_size << " bytes" << endl;
	//	
	//}

	////for memGen2 part 2
	//Cache_line_size = 32;
	//for (int x = 0; x < 5; x++) {
	//	ways = num_of_ways[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen2();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen2() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "2," << ways << "," << Cache_line_size << " bytes" << endl;
	//	
	//}

	////For memGen3 part 1 
	//ways = 4;
	//for (int x = 0; x < 4; x++) {
	//	Cache_line_size = line_sizes[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen3();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen3() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "3," << ways << "," << Cache_line_size << " bytes" << endl;
	//	
	//}

	////for memGen3 part 2
	//Cache_line_size = 32;
	//for (int x = 0; x < 5; x++) {
	//	ways = num_of_ways[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen3();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen3() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "3," << ways << "," << Cache_line_size << " bytes" << endl;
	//	
	//}

	////For memGen4 part 1 
	//ways = 4;
	//for (int x = 0; x < 4; x++) {
	//	Cache_line_size = line_sizes[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen4();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen4() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "4," << ways << "," << Cache_line_size << " bytes" << endl;
	//
	//}

	////for memGen4 part 2
	//Cache_line_size = 32;
	//for (int x = 0; x < 5; x++) {
	//	ways = num_of_ways[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen4();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen4() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "4," << ways << "," << Cache_line_size << " bytes" << endl;
	//
	//}

	////For memGen5 part 1 
	//ways = 4;
	//for (int x = 0; x < 4; x++) {
	//	Cache_line_size = line_sizes[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen5();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen5() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "5," << ways << "," << Cache_line_size << " bytes" << endl;
	//
	//}

	////for memGen5 part 2
	//Cache_line_size = 32;
	//for (int x = 0; x < 5; x++) {
	//	ways = num_of_ways[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen5();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen5() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "5," << ways << "," << Cache_line_size << " bytes" << endl;
	//
	//}

	////For memGen6 part 1 
	//ways = 4;
	//for (int x = 0; x < 4; x++) {
	//	Cache_line_size = line_sizes[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen6();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//	cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen6() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "6," << ways << "," << Cache_line_size << " bytes" << endl;
	//	
	//}

	////for memGen6 part 2
	//Cache_line_size = 32;
	//for (int x = 0; x < 5; x++) {
	//	ways = num_of_ways[x];
	//	set_number = ((CACHE_SIZE / ways) / Cache_line_size);
	//	Data_size = Cache_line_size / 4;
	//	cach_width = ((Data_size + 2) * ways);
	//	vector<vector<unsigned>> cache(set_number, vector<unsigned>(cach_width));
	//	vector<unsigned int> counters(set_number);
	//	index_length = log2(set_number);
	//	offset_length = log2(Cache_line_size);
	//	tag_length = 26 - index_length - offset_length;

	//	hit = 0;
	//	cacheResType r;

	//	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	//	{
	//		addr = memGen6();
	//		r = cacheSim_set_associative(addr, cache, index_length, offset_length, ways, Data_size, counters);
	//		if (r == HIT) hit++;
	//		//	cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	//	}
	//	cout << dec << hit << endl;
	//	cout << "Hit ratio = " << (100 * (double)hit / NO_OF_Iterations) << "For memGen6() with ways = " << ways << " and Cache Line Size = " << Cache_line_size << " bytes" << endl;
	//	outfile << (100 * (double)hit / NO_OF_Iterations) << "," << 100 - (100 * (double)hit / NO_OF_Iterations) << "," << "6," << ways << "," << Cache_line_size << " bytes" << endl;
	//
	//}
	return 0;
}
