#include "RecordManager.h"
#include<fstream>

RecordManager::RecordManager(void)
{
}


RecordManager::~RecordManager(void)
{
}

void RecordManager::readFromMouseLog(string logDir,vector<EventRecord>& records)
{
	string filePath = logDir + "mouse.txt";
	ifstream mouseFile(filePath);

	string line;
	int recordNum = 0;
	Mat preImg;
	vector<string> afkTime;
	int totalAfk = 0;
	int totalNoAfk = 0;
	while(getline(mouseFile,line))
	{
		EventRecord r;
		r.type = "MOUSE";

		trim(line);
		r.timestamp = line;

		//mouse event
		getline(mouseFile,line);
		if(line == "NOT LOGGED APPLICATION")
		{
			r.mouseEvent = "Unknow";
			r.processName = "Unknow";
			r.windowName = "Unknow";
			records.push_back(r);
			continue;
		}

		vector<string> sarray = split(line," ");
		int len = sarray.size();
		r.mousePosition.x = stringToNumber<int>(sarray[len-2]);
		r.mousePosition.y = stringToNumber<int>(sarray[len-1]);
		r.mouseEvent = join(subvector(sarray, 0, len-2)," ");

		cout<<r.mouseEvent<<' '<<r.mousePosition.x<<' '<<r.mousePosition.y<<endl;

		//window
		getline(mouseFile,line);
		string::size_type pos1 = line.find_first_of(",");
		r.windowHwnd = line.substr(0,pos1);
		string::size_type pos2 = line.find_last_of("(");
		string::size_type pos3 = line.find_last_of(")");
		string s = line.substr(pos2+1,pos3-pos2-1);
		sarray = split(s," ");
		int left = stringToNumber<int>(sarray[0]);
		int top = stringToNumber<int>(sarray[1]);
		int right = stringToNumber<int>(sarray[2]);
		int bottom = stringToNumber<int>(sarray[3]);
		if(left<0)
		{
			r.windowRect.x = 0;
			r.windowRect.width = right + left;
		}
		else
		{
			r.windowRect.x = left;
			r.windowRect.width = right - left;
		}
		if(top<0)
		{
			r.windowRect.y = 0;
			r.windowRect.height = bottom + top;
		}
		else
		{
			r.windowRect.y = top;
			r.windowRect.height = bottom - top;
		}
		r.windowName = line.substr(pos1+1,pos2-pos1-1);
		trim(r.windowName);
		//cout<<r.windowName<<endl;

		//process
		getline(mouseFile,line);
		string::size_type pos = line.find_last_of(" ");
		r.processName = line.substr(0,pos);
		r.processId = stringToNumber<long>(line.substr(pos+1));
		//cout<<r.processName<<endl;

		//parent
		getline(mouseFile,line);
		r.parentName = line;
		//cout<<r.parentName<<endl;

		recordNum++;
		records.push_back(r);

		//if(recordNum>2000) break;
	}

	mouseFile.close();
}

void RecordManager::test()
{
	vector<EventRecord> rs;
	readFromMouseLog("C:\\Users\\LingFeng\\Desktop\\hengtian\\log\\",rs);
}