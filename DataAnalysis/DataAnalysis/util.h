#pragma once
#include<Windows.h>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<hash_map>
#include<hash_set>
#include<algorithm>
using namespace std;

void trim(string& str);

vector<string> split(string& str, string delim);

string join(vector<string> sarray,string sep);

template<typename T>
T stringToNumber(const string& numberAsString)
{
    T val;

    std::stringstream stream(numberAsString);
    stream >> val;
    if (stream.fail()) {
        std::runtime_error e(numberAsString);
        throw e;
    }

    return val;
}

template<typename T>
vector<T> subvector(vector<T> v, int first, int last)
{
	vector<T>::const_iterator first_it = v.begin() + first;
	vector<T>::const_iterator last_it = v.begin() + last;

	vector<T> newV(first_it,last_it);
	return newV;
}

void replaceAll(string& str, string from,string to);

SYSTEMTIME toSystemTime(string timestamp);

double GetTimeDifference( SYSTEMTIME &st1, SYSTEMTIME &st2 );

template<typename T>
double calcMedianOfVector(vector<T> v)
{
  T median;
  size_t size = v.size();

  sort(v.begin(), v.end());

  if (size  % 2 == 0)
  {
      median = (v[size / 2 - 1] + v[size / 2]) / 2;
  }
  else 
  {
      median = v[size / 2];
  }

  return median;
}