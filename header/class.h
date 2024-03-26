#pragma once

#include "string.h"

using namespace std;

class Risedual
{
public:

	string prn;
	double code;
	double phase;
	double elev;
	double azi;
};

//MHM的格网点
class Grid
{
public:
	//格网点的方位角
	double azi1;
	double azi2;

	double muti_code; //伪距多径建模值
	double muti_phase;//载波多径建模值

	vector<double> code_res;//网格内伪距残差
	vector<double> phase_res;//网格内载波残差
public:
};

//MHM模型一圈的格网点
class Grid_cycle
{
public:
	//格网点的方位角
	double elev1;
	double elev2;
	
	double dlt_azi;//方位角分辨率

	vector<Grid> grid; //各网点集合
};

//MHM模型
class MHM_modle
{
public:

	vector<Grid_cycle> vec_cycle; //MHM模型每周
	
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="resol">MHM elevation resolution</param>
	/// <param name="cut_off">Elevation angle cut-off</param>
	/// <param name="model_type">Model type,1 is equal angle,2 is equal area</param>
	void init(double resol, int cut_off, int model_type);

	/// <summary>
	/// outlier detect
	/// </summary>
	/// <param name="b_avg_test">Whether to conduct mean value testing</param>
	/// <param name="b_outlier_test">whether to detect outliers</param>
	void detect_outlier(bool b_avg_test, bool b_outlier_test);

	/// <summary>
	/// 
	/// </summary>
	void establish_model();

};

//配置文件
class Configration
{
public:

	string res_path;		//文件路径
	string result_path;		//MHM文件路径
	string stn_name;		//测站名
	string res_name_format; //测站文件名的正则表达式
	string cnst;			//参与建模的星座 GCE

	//时间
	int iyear = 0, imon = 0, iday = 0;
	int day_num = 0;	//建模数据天数
	int cut_off = 0;	//截至高度角
	int cnst_num = 0;   //参与建模的星座数量
	int model_type = 0; //模型类型，1等角模型，2等面积模型

	bool b_avg_test = false;	//是否进行均值检验
	bool b_outlier_test = false;//是否进行粗差探测

	//网格高度角分辨率
	double resolution = 0.0;

public:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="cfg_path">MHM elevation resolution</param>
	bool read_cfg(string cfg_path);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="vec_res">residual vector</param>
	bool read_res(vector<Risedual>* vec_res);
};

/// <summary>
/// 
/// </summary>
/// <param name="file_name">path of MHM modeling result</param>
/// <param name="p_mhm">pointer of MHM_modle</param>
/// <param name="p_cfg">pointer of Configration</param>
bool output_mhm(string file_name, MHM_modle* p_mhm, Configration* p_cfg);