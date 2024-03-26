#include "string.h"

// ʹ��stringstream�ָ��ַ���
vector<string> splitString(const string& str, char delim) 
{
	vector<string> tokens;
	
	string token;

	//�Ȱ�һЩ�ַ��滻Ϊ�ո�
	token = str;
	replace(token.begin(), token.end(), '\t', ' ');
	replace(token.begin(), token.end(), '\r', ' ');
	replace(token.begin(), token.end(), '\n', ' ');

	stringstream ss(token);

	while (getline(ss, token, delim)) 
	{
		if (token == "" || token == " " || token == "\t")
			continue;

		tokens.push_back(token);
	}
	return tokens;
}

void cal_avg_std(const vector<double>* vec, double* p_avg, double* p_std)
{
	double sum = 0.0;
	double sumSquare = 0.0;
	double mean = 0.0;
	double stdDev = 0.0;
	int n = vec->size();

	//��ֵ
	for (double val : *vec)
	{
		sum += val;
	}
	mean = sum / n;

	//std
	sum = 0.0;
	for (double val : *vec)
	{
		sum += (val - mean) * (val - mean);
	}
	stdDev = sqrt(sum / n);


	if (p_avg)
	{
		*p_avg = mean;
	}

	if (p_std)
	{
		*p_std = stdDev;
	}
}

vector<string> splitString1(const string& str, char delim)
{
	vector<string> tokens;
	string token = "";
	for (int i = 0; i < str.size(); ++i)
	{
		if (str[i] == ' ' || str[i] == '\t')
		{
			if (token != "")
			{
				tokens.push_back(token);
			}
			continue;
		}
		else
		{
			token += str[i];
		}
	}
	return tokens;
}

void remove_string(string& str)
{
	for (int i = 0; i < str.size(); ++i) 
	{
		if (str[i] == ' ') 
		{
			str.erase(i, 1);
			--i; // ����������һ���ַ�
		}
	}
}

bool iszero(double val)
{
	return fabs(val) < 1e-36;
}


int ymd2mjd(int iyear, int imonth, int iday)
{
	int doy_of_month[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

	int iyr = 0, modified_julday = 0;
	//check the input data
	if ((iyear < 0 || imonth < 0 || iday < 0 || imonth>12 || iday>366) || (0 != imonth && iday > 31))
	{
		printf("function: modified_julday error\n");
	}

	iyr = iyear;
	if (imonth <= 2)
	{
		iyr = iyr - 1;
	}
	modified_julday = 365 * iyear - 678941 + iyr / 4 - iyr / 100 + iyr / 400 + iday;
	if (0 != imonth)
	{
		modified_julday = modified_julday + doy_of_month[imonth - 1];
	}
	return modified_julday;
}


int ymd2doy(int iyear, int imonth, int iday)
{
	return ymd2mjd(iyear, imonth, iday) - ymd2mjd(iyear, 1, 1) + 1;;
}

bool sigma3_detect_outlier(vector<double>* p_vec, bool* t_flag, bool b_outlier)
{
	double avg1, avg2;
	double std1, std2;
	double sd1, sd2;
	double Tf = 0.0; //F������
	int n = p_vec->size();
	bool flag = false;
	bool ret = false;

	vector<double> vec1, vec2;

	vec1 = *p_vec;
	vec2 = vec1;

	cal_avg_std(p_vec, &avg1, &std1);

	//3��sigma�޳��ֲ�
	if (b_outlier)
	{
		do
		{
			for (auto it = p_vec->begin(); it != p_vec->end();)
			{
				if (fabs(*it - avg1) > 3 * std1)
				{
					it = p_vec->erase(it); // ɾ����ǰԪ�أ���������һ��Ԫ�صĵ�����
					flag = true;
					ret = true;
				}
				else
				{
					++it; // �ƶ�����һ��Ԫ��
				}
			}
			flag = false;
		} while (flag);
	}

	cal_avg_std(p_vec, &avg2, &std2);

	//��ֵ����
	double degreesOfFreedom = 0.0;
	double probability = 0.0;
	double quantileValue = 0.0;

	degreesOfFreedom = p_vec->size();
	probability = 1 - 0.05 / 2;
	quantileValue = 0.0;
	// ����ѧ�� t �ֲ�����
	boost::math::students_t_distribution<double> tDist(degreesOfFreedom);

	// ���� t ����ķ�λ��
	quantileValue = boost::math::quantile(tDist, probability);

	quantileValue *= std2 / sqrt(p_vec->size());

	//������ֵ������ľ�ֵ�㲻�ȣ�˵���ж�·��
	//û�ж�·����ʱ�������ֵΪ��
	if (fabs(avg2) > quantileValue)
	{
		*t_flag = true; // �ж�·��
	}
	else
	{
		*t_flag = false; // �޶�·��
	}

	return ret;
}
