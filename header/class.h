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

//MHM�ĸ�����
class Grid
{
public:
	//������ķ�λ��
	double azi1;
	double azi2;

	double muti_code; //α��ྶ��ģֵ
	double muti_phase;//�ز��ྶ��ģֵ

	vector<double> code_res;//������α��в�
	vector<double> phase_res;//�������ز��в�
public:
};

//MHMģ��һȦ�ĸ�����
class Grid_cycle
{
public:
	//������ķ�λ��
	double elev1;
	double elev2;
	
	double dlt_azi;//��λ�Ƿֱ���

	vector<Grid> grid; //�����㼯��
};

//MHMģ��
class MHM_modle
{
public:

	vector<Grid_cycle> vec_cycle; //MHMģ��ÿ��
	
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

//�����ļ�
class Configration
{
public:

	string res_path;		//�ļ�·��
	string result_path;		//MHM�ļ�·��
	string stn_name;		//��վ��
	string res_name_format; //��վ�ļ�����������ʽ
	string cnst;			//���뽨ģ������ GCE

	//ʱ��
	int iyear = 0, imon = 0, iday = 0;
	int day_num = 0;	//��ģ��������
	int cut_off = 0;	//�����߶Ƚ�
	int cnst_num = 0;   //���뽨ģ����������
	int model_type = 0; //ģ�����ͣ�1�Ƚ�ģ�ͣ�2�����ģ��

	bool b_avg_test = false;	//�Ƿ���о�ֵ����
	bool b_outlier_test = false;//�Ƿ���дֲ�̽��

	//����߶ȽǷֱ���
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