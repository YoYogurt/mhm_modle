// multipath_model.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "class.h"

using namespace std;

int main()
{
	string path = "F:\\working\\202403_multipath_model\\mhm_cfg.txt";
	string str;
	string mhm_file;
	char msg[128];

	Configration cfg;
	MHM_modle mhm;
	Grid_cycle cycle;
	Grid grid;

	vector<Risedual> vec_res;

	int irow, icol, doy;

	double elev;
	double azi;

	//读取配置文件
	if (cfg.read_cfg(path))
	{
		printf("***** Reading configuration is completed *****\n");
	}
	else
	{
		printf("***** Reading configuration failed *****\n");
		return false;
	}
	
	//读取残差文件
	if (cfg.read_res(&vec_res))
	{
		printf("***** Reading residuals is completed *****\n");
	}
	else
	{
		printf("***** Reading residuals failed *****\n");
		return false;
	}
	

	//MHM模型格网划分
	mhm.init(cfg.resolution, cfg.cut_off, cfg.model_type);
	printf("***** The MHM model grid division is completed *****\n");

	//MHM模型格网内分入残差
	printf("***** Put residues into the related grid of MHM model *****\n");
	printf("Totolly %d residuals need to be sorted out\n", vec_res.size());
	for (int i = 0; i < vec_res.size(); i++)
	{
		double azi_i;

		elev = vec_res[i].elev;
		azi = vec_res[i].azi;

		if (elev < cfg.cut_off)
			continue;

		irow = floor((elev - cfg.cut_off) / cfg.resolution);
		icol = floor(azi / mhm.vec_cycle[irow].dlt_azi);
		
		mhm.vec_cycle[irow].grid[icol].code_res.push_back(vec_res[i].code);
		mhm.vec_cycle[irow].grid[icol].phase_res.push_back(vec_res[i].phase);

		if (i % 10000 == 0)
		{
			printf("%d residuals complete\n", i);
		}
	}
	vec_res.clear();

	//残差粗差探测
	mhm.detect_outlier(cfg.b_avg_test, cfg.b_outlier_test);

	/********** MHM模型建立 开始**********/
	mhm.establish_model();
	/********** MHM模型建立 结束**********/


	/**********  MHM模型文件输出 开始  **********/

	//文件名组成
	doy = ymd2doy(cfg.iyear, cfg.imon, cfg.iday);
	sprintf_s(msg, sizeof(msg), "%4s_%03d", cfg.stn_name.c_str(),doy);
	doy += cfg.day_num - 1;
	sprintf_s(msg, sizeof(msg), "%s_%03d.txt", msg, doy);
	mhm_file = cfg.result_path + '\\' + msg;

	if (output_mhm(mhm_file, &mhm, &cfg))
	{
		printf("%s output completed\n", mhm_file.c_str());
	}
	else
	{
		printf("%s output failed\n", mhm_file.c_str());
	}
	/***** MHM模型文件输出 结束*****/


	
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
