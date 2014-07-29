#include "util.h"

void trim(string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

vector<string> split(string& str, string delim)
{
    string::size_type start = 0;
    string::size_type end = str.find(delim);
	vector<string> result;
    while (end != std::string::npos)
    {
        //std::cout << s.substr(start, end - start) << std::endl;
		result.push_back(str.substr(start,end-start));
		start = end + delim.length();
        end = str.find(delim, start);
    }

	result.push_back(str.substr(start,end));

	return result;
}

string join(vector<string> sarray, string sep)
{
	std::stringstream ss;
	for(size_t i = 0; i < sarray.size(); ++i)
	{
	  if(i != 0)
		ss << sep;
	  ss << sarray[i];
	}
	std::string s = ss.str();

	return ss.str();
}

void replaceAll(string& str, string from,string to)
{
	string::size_type start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
         str.replace(start_pos, from.length(), to);
         start_pos += to.length(); // ...
	}
}

SYSTEMTIME toSystemTime(string timestamp) //timestamp format: 2000-06-14-13-33-20-999
{
	SYSTEMTIME time;
	vector<string> sarray = split(timestamp,"-");
	time.wYear = stringToNumber<WORD>(sarray[0]);
	time.wMonth = stringToNumber<WORD>(sarray[1]);
	time.wDay = stringToNumber<WORD>(sarray[2]);
	time.wHour = stringToNumber<WORD>(sarray[3]);
	time.wMinute = stringToNumber<WORD>(sarray[4]);
	time.wSecond = stringToNumber<WORD>(sarray[5]);
	time.wMilliseconds = stringToNumber<WORD>(sarray[6]);

	return time;
}

double GetTimeDifference( SYSTEMTIME &st1, SYSTEMTIME &st2 )
{
    FILETIME        ft1, ft2;
    LARGE_INTEGER   li1, li2, liDiff;
    DWORD dwDiff;

    SystemTimeToFileTime( &st1, &ft1 );
    SystemTimeToFileTime( &st2, &ft2 );

	ULARGE_INTEGER ul1;
    ul1.LowPart = ft1.dwLowDateTime;
    ul1.HighPart = ft1.dwHighDateTime;

	ULARGE_INTEGER ul2;
    ul2.LowPart = ft2.dwLowDateTime;
    ul2.HighPart = ft2.dwHighDateTime;

	ULARGE_INTEGER uliRetValue;
    uliRetValue.QuadPart = 0;

	if(ul2.QuadPart >  ul1.QuadPart)
	{
		ul2.QuadPart -= ul1.QuadPart;
		uliRetValue = ul2;
		return (double)uliRetValue.QuadPart / 10 / 1000 / 1000;
	}
	else
	{
		ul1.QuadPart -= ul2.QuadPart;
		uliRetValue = ul1;
		return 0-(double)uliRetValue.QuadPart / 10 / 1000 / 1000;
	}

}


