#pragma once

#define MIN_RES_IN_GRID 15
#define EQUAL_ANGLE_GRID 1  //等角网格
#define EQUAL_AREA_GRID 2  //等面积网格



#define PI 3.141592653589793
#define D2R (PI/180.0)
#define R2D (1.0/D2R)

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <numeric> // for std::accumulate
#include <iomanip>

#include <math.h>
#include <stdio.h>

#include <boost\math\distributions\students_t.hpp>

using namespace std;

vector<string> splitString1(const string& str, char delim);


vector<string> splitString(const string& str, char delim);

/// <summary>
/// 
/// </summary>
/// <param name="vec">data vector </param>
/// <param name="p_avg">mean</param>
/// <param name="p_std">STD</param>
void cal_avg_std(const vector<double>* vec, double* p_avg, double* p_std);

/// <summary>
/// delete ' ' in string
/// </summary>
/// <param name="val"></param>
/// <returns></returns>
void remove_string(string& str);

bool iszero(double val);

/// <summary>
/// year month day to MJD
/// </summary>
/// <param name="iyear"></param>
/// <param name="imonth"></param>
/// <param name="iday"></param>
/// <returns>MJD</returns>
int ymd2mjd(int iyear, int imonth, int iday);

/// <summary>
/// year month day to DOY
/// </summary>
/// <param name="iyear"></param>
/// <param name="imonth"></param>
/// <param name="iday"></param>
/// <returns> DOY </returns>
int ymd2doy(int iyear, int imonth, int iday);


/// <summary>
/// 
/// </summary>
/// <param name="p_vec"> data vector </param>
/// <param name="t_flag">flag of mean test through T test</param>
/// <param name="b_outlier">是否进行粗差探测</param>
/// <returns></returns>
bool sigma3_detect_outlier(vector<double>* p_vec, bool* t_flag, bool b_outlier);

