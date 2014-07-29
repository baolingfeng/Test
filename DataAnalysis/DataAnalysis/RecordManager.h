#pragma once
#include<iostream>
#include<cv.h>
#include"util.h"

using namespace std;
using namespace cv;

class EventRecord
{
public:
	EventRecord() {};

	int index;

	string type;
	string timestamp;
	
	string windowName;
	string processName;
	string parentName;

	double duration;

	//mouse
	string mouseEvent;
	Point mousePosition;
	string windowHwnd;
	Rect windowRect;
	long processId;

	//keyboard
	string key;
};

class RecordManager
{
public:
	RecordManager(void);
	~RecordManager(void);

	void readFromMouseLog(string logDir,vector<EventRecord>& records);
	
	void test();
};

