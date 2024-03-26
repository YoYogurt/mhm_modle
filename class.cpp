#include "class.h"

bool Configration::read_cfg(string cfg_path)
{
	char line[256];
	string str = "";
	vector<string> str_parts;

	fstream cfg_file;

	//打开配置文件
	cfg_file.open(cfg_path, ios::in);
	if (!cfg_file)
		return false;


	//逐行读取文件
	while (!cfg_file.eof())
	{
		cfg_file.getline(line, 256);	
		if (line == "")//空行
		{
			continue;
		}
		str = line;

		//"!" 分割注释与配置选项
		str_parts = splitString(str, '!');
		str = str_parts[0];

		//"=" 分割名称与配置的值
		str_parts = splitString(str, '=');
		str = str_parts[1];

		remove_string(str_parts[0]);//删除字符串中的空格

		if (str_parts[0] == "res_path")
		{
			remove_string(str_parts[1]);
			this->res_path = str_parts[1];
		}
		else if (str_parts[0] == "result_path")
		{
			remove_string(str_parts[1]);
			this->result_path = str_parts[1];
		}
		else if (str_parts[0] == "time_start")
		{
			str_parts = splitString(str, ' ');		//" " 分割配置值
			this->iyear = stoi(str_parts[0]);
			this->imon = stoi(str_parts[1]);
			this->iday = stoi(str_parts[2]);
		}
		else if (str_parts[0] == "days_of_data")
		{
			str_parts = splitString(str, ' ');		//" " 分割配置值
			this->day_num = stoi(str_parts[0]);
		}
		else if(str_parts[0] == "mhm_resolution")
		{			
			str_parts = splitString(str, ' ');		//" " 分割配置值
			this->resolution = stod(str_parts[0]);
		}
		else if (str_parts[0] == "constellation")
		{
			str_parts = splitString(str, ' ');
			this->cnst = str_parts[0];
			this->cnst_num = this->cnst.length();
		}
		else if (str_parts[0] == "cut_off")
		{
			str_parts = splitString(str, ' ');
			this->cut_off = stoi(str_parts[0]);
		}
		else if (str_parts[0] == "stn_name")
		{
			str_parts = splitString(str, ' ');
			this->stn_name = str_parts[0];
		}
		else if (str_parts[0] == "res_name_format")
		{
			str_parts = splitString(str, ' ');
			this->res_name_format = str_parts[0];
		}
		else if (str_parts[0] == "model_type")
		{
			str_parts = splitString(str, ' ');
			this->model_type = stoi(str_parts[0]);
		}
		else if (str_parts[0] == "avg_test")
		{
			str_parts = splitString(str, ' ');
			this->b_avg_test = str_parts[0] == "YES";
		}
		else if (str_parts[0] == "outlier_test")
		{
			str_parts = splitString(str, ' ');
			this->b_outlier_test = str_parts[0] == "YES";
		}
	}

	cfg_file.close();

	return true;
}

bool Configration::read_res(vector<Risedual>* vec_res)
{
	char line[128];
	string str = "";
	string res_path = ""; //残差文件路径

	vector<string> str_parts;

	fstream res_file;
	Risedual res;

	int doy = 0;


	//G01 C19 E01的正则表达式 "(G|C|E)\\d{2}"
	regex pattern("(G|C|E)\\d{2}");
	smatch match;

	for (int iday = 0; iday < this->day_num; iday++)
	{
		res_path = this->res_path;
		
		//整理文件路径	
		if (this->res_name_format == "1")
		{
			doy = ymd2doy(this->iyear, this->imon, this->iday) + iday;

			sprintf_s(line, sizeof(line), "\\%03d", doy);
			res_path += line;

			sprintf_s(line, sizeof(line), "\\res_%4d%03d_%s", this->iyear, doy, this->stn_name.c_str());
			res_path += line;
		}
		else if (this->res_name_format == "2")
		{
			doy = ymd2doy(this->iyear, this->imon, this->iday) + iday;
			sprintf_s(line, sizeof(line), "\\res_%4d%03d_%s", this->iyear, doy, this->stn_name.c_str());
			res_path += line;
		}

		//打开配置文件
		res_file.open(res_path, ios::in);
		if (!res_file)
		{
			printf("%s\t open failed! \n", res_path.c_str());
			return false;
		}		

		printf("%s\t open successful! \n", res_path.c_str());

		//读取文件头
		while (!res_file.eof())
		{
			res_file.getline(line, 128);
			str = line;
			if (str.find("END OF HEADER") != string::npos)
			{
				break;
			}
		}

		//读取文件体
		while (!res_file.eof())
		{
			res_file.getline(line, 128);
			str = line;
			if (str.find("TIM") != string::npos
				|| str.find("CST") != string::npos
				|| str == "")
			{
				continue;
			}

			str_parts = splitString(str, ' ');

			//PRIDE_AR 最后有一行空行
			if (str_parts.size() == 0)
				continue;

			res.prn = str_parts[0];
			//筛选系统
			if (this->cnst.find(str_parts[0][0]) == string::npos)
			{
				continue;
			}

			res.phase = stod(str_parts[1]);
			res.code = stod(str_parts[2]);
			res.elev = stod(str_parts[6]);
			res.azi = stod(str_parts[7]);
			res.azi = res.azi < 0.0 ? res.azi + 360 : res.azi;

			vec_res->push_back(res);
		}

		res_file.close();

		printf("%s\t read complete! \n", res_path.c_str());

	}

	return true;
}

void MHM_modle::init(double resol,int cut_off,int model_type)
{
	Grid_cycle cycle;
	Grid grid;
	int n_cycle = 0; //MHM一圈的等面积网格数
	double n = 0.0;
	double elev = 0.0;

	//等角网格
	if (model_type==EQUAL_ANGLE_GRID)
	{
		cycle.dlt_azi = resol;
		//方位角
		for (int iazi = 0; iazi < 360; iazi++)
		{
			grid.azi1 = iazi;
			grid.azi2 = iazi + 1.0;
			cycle.grid.push_back(grid);
		}

		//高度角
		for (int ielev = cut_off; ielev < 90; ielev++)
		{
			cycle.elev1 = ielev;
			cycle.elev2 = ielev + 1.0;

			this->vec_cycle.push_back(cycle);
		}
	}
	//等面积网格
	else if (model_type == EQUAL_AREA_GRID)
	{
		//高度角
		for (int ielev = cut_off; ielev < 90; ielev++)
		{
			cycle.grid.clear();
			elev = ielev * D2R;
			n = resol / cos(elev);
			n_cycle = abs(round(360 / n));

			cycle.dlt_azi = 360.0 / n_cycle;

			cycle.elev1 = ielev;
			cycle.elev2 = ielev + 1.0;

			for (int iazi = 0; iazi < n_cycle; iazi++)
			{
				grid.azi1 = 360.0 / n_cycle * iazi;
				grid.azi2 = 360.0 / n_cycle * (iazi + 1.0);
				cycle.grid.push_back(grid);
			}
			this->vec_cycle.push_back(cycle);
		}
	}

	return;
}

void MHM_modle::detect_outlier(bool b_avg_test, bool b_outlier_test)
{
	Grid* p_grid;

	double avg1, avg2;
	double std1, std2;
	double sd1, sd2;

	bool outlier_flag;
	bool avg_flag;

	int avg_delete_phase = 0; //由于均值不显著被删除的网格数
	int avg_delete_code = 0;
	int min_num = 0;

	vector<int> vec_i;


	//15个以内的数值也要输出结果 to do
	for (int ielev = 0; ielev < this->vec_cycle.size(); ielev++)
	{
		for (int iazi = 0; iazi < this->vec_cycle[ielev].grid.size(); iazi++)
		{
			p_grid = &(this->vec_cycle[ielev].grid[iazi]);

			//残差数小于阈值，不再做处理
			if (p_grid->phase_res.size() < MIN_RES_IN_GRID
				|| p_grid->code_res.size() < MIN_RES_IN_GRID)
			{
				if (p_grid->phase_res.size() > 0
					&& p_grid->code_res.size() > 0)
				{
					min_num++;
				}
				continue;
			}

			//3sigma粗差探测，均值检验
			//伪距
			outlier_flag = sigma3_detect_outlier(&(p_grid->code_res), &avg_flag, b_outlier_test);
			//均值不显著，说明没有多路径，不进行堆叠
			if (!avg_flag && b_avg_test)
			{
				p_grid->code_res.clear();
				avg_delete_code++;
			}

			//载波
			avg_flag = true;
			outlier_flag = sigma3_detect_outlier(&(p_grid->phase_res), &avg_flag, b_outlier_test);
			//均值不显著，说明没有多路径，不进行堆叠
			if (!avg_flag && b_avg_test)
			{
				p_grid->phase_res.clear();
				avg_delete_phase++;
			}
		}
	}

	printf("均值不显著清除的网格数，相位 %d\t，伪距 %d\n", avg_delete_phase, avg_delete_code);
	printf("建模数据小于阈值的网格数，%d\n", min_num);
	return;
}

void MHM_modle::establish_model()
{
	for (int ielev = 0; ielev < this->vec_cycle.size(); ielev++)//方位角
	{
		for (int iazi = 0; iazi < this->vec_cycle[ielev].grid.size(); iazi++)//高度角
		{
			if (this->vec_cycle[ielev].grid[iazi].code_res.size() == 0
				|| this->vec_cycle[ielev].grid[iazi].phase_res.size() == 0)
			{
				this->vec_cycle[ielev].grid[iazi].code_res.clear();
				this->vec_cycle[ielev].grid[iazi].phase_res.clear();
				continue;
			}

			//计算均值
			this->vec_cycle[ielev].grid[iazi].muti_code
				= accumulate(this->vec_cycle[ielev].grid[iazi].code_res.begin(), this->vec_cycle[ielev].grid[iazi].code_res.end(), 0.0);
			this->vec_cycle[ielev].grid[iazi].muti_phase
				= accumulate(this->vec_cycle[ielev].grid[iazi].phase_res.begin(), this->vec_cycle[ielev].grid[iazi].phase_res.end(), 0.0);

			this->vec_cycle[ielev].grid[iazi].muti_code /= this->vec_cycle[ielev].grid[iazi].code_res.size();
			this->vec_cycle[ielev].grid[iazi].muti_phase /= this->vec_cycle[ielev].grid[iazi].phase_res.size();
		}
	}

}

bool output_mhm(string file_name, MHM_modle* p_mhm, Configration* p_cfg) 
{
	FILE* fp;
	string str;

	if (fopen_s(&fp, file_name.c_str(), "w+") != 0)
	{
		printf("%s open failed\n", file_name.c_str());
		return false;
	}

	//输出文件头，关于模型类型的一些描述

	//STATION NAME
	fprintf(fp, "%40s", p_cfg->stn_name.c_str());
	fprintf(fp, "%20s", " ");
	fprintf(fp, "%-20s\n", "STATION NAME");
	fflush(fp);

	//MODEL_TYPE
	if (p_cfg->model_type == EQUAL_ANGLE_GRID)
	{
		fprintf(fp, "%40s", "EQUAL_ANGLE_GRID");
	}
	else if (p_cfg->model_type == EQUAL_AREA_GRID)
	{
		fprintf(fp, "%40s", "EQUAL_AREA_GRID");
	}
	fprintf(fp, "%20s", " ");
	fprintf(fp, "%-20s\n", "MODEL_TYPE");

	//Cut_off / Resol
	fprintf(fp, "%20.2lf%20.2lf", p_cfg->cut_off*1.0, p_cfg->resolution);
	fprintf(fp, "%20s", " ");
	fprintf(fp, "%-20s\n", "Cut_off / Resol");
	fflush(fp);

	//constellation
	for (int i = 0; i < p_cfg->cnst.size(); i++)
	{
		if (p_cfg->cnst[i] == 'G')
		{
			fprintf(fp, "%10s", "GPS");
		}
		else if (p_cfg->cnst[i] == 'C')
		{
			fprintf(fp, "%10s", "BDS3");
		}
		else if (p_cfg->cnst[i] == 'E')
		{
			fprintf(fp, "%10s", "GAL");
		}
	}
	str = "%" + to_string(60 - p_cfg->cnst.size() * 10) + "s";
	fprintf(fp, str.c_str(), " ");
	fprintf(fp, "%-20s\n", "CONSTELLATION");

	//AVG_TEST & OUTLIER_TEST
	if (p_cfg->b_avg_test)
		fprintf(fp, "%20s", "AVG_TEST_ON");
	else
		fprintf(fp, "%20s", "AVG_TEST_OFF");

	if (p_cfg->b_outlier_test)
		fprintf(fp, "%20s", "OUTLIER_TEST_ON");
	else
		fprintf(fp, "%20s", "OUTLIER_TEST_OFF");

	fprintf(fp, "%20s", " ");
	fprintf(fp, "%-20s\n", "DESCRIPTION");


	//END OF HEADER
	fprintf(fp, "%60s", " ");
	fprintf(fp, "%-20s\n", "END OF HEADER");
	fflush(fp);

	//输出模型改正值
	for (int ielev = 0; ielev < p_mhm->vec_cycle.size(); ielev++)//方位角
	{
		for (int iazi = 0; iazi < p_mhm->vec_cycle[ielev].grid.size(); iazi++)//高度角
		{
			//没有改正值也输出，方便索引观测值
			if (iszero(p_mhm->vec_cycle[ielev].grid[iazi].muti_code)
				|| iszero(p_mhm->vec_cycle[ielev].grid[iazi].muti_phase))
			{
				continue;
			}

			fprintf(fp, "%10.5lf%10.5lf%12.5lf%12.5lf%8.3lf%8.3lf%5d%5d\n"
				, p_mhm->vec_cycle[ielev].elev1
				, p_mhm->vec_cycle[ielev].elev2
				, p_mhm->vec_cycle[ielev].grid[iazi].azi1
				, p_mhm->vec_cycle[ielev].grid[iazi].azi2
				, p_mhm->vec_cycle[ielev].grid[iazi].muti_code
				, p_mhm->vec_cycle[ielev].grid[iazi].muti_phase
				, p_mhm->vec_cycle[ielev].grid[iazi].code_res.size()
				, p_mhm->vec_cycle[ielev].grid[iazi].phase_res.size());
		}
	}

	fclose(fp);
	fp = nullptr;
	return true;
}
