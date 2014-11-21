#ifndef __VICONRTCLIENT_HH__
#define __VICONRTCLIENT_HH__

//-----------------------------------------------------------------------------
//	Vicon RT Client
//-----------------------------------------------------------------------------

#include <winsock2.h>

#include <iostream>
//#include <cassert>
#include "assert.h"
#include <string>
#include <vector>
#include <algorithm>	
#include <functional>
#include <limits>
#include <math.h>
#include "ClientCodes.h"


using namespace std;

#define PR(x) cout << #x"=" << x << endl;

// by defining VC_VERBOSE debug info will go to stdout
//#define VC_VERBOSE

// to avoid TCP deadlocks, insert a number of milliseconds to pause 
// if you have a lot of OpegGL code and you're not running the realtime emulator, then
// you can get away with a 0 second pause, otherwise, 30 works well
#define VC_PAUSE 0

enum {BRIEF, VERBOSE};


//////////////////////////////////////////////////////////////////////
//
// ViconRTClient
//
//////////////////////////////////////////////////////////////////////
// Object to manage connection with Vicon Realtime
//
class ViconRTClient {

public:

	ViconRTClient(void) {};
	~ViconRTClient(void) {};

	int initialize(char* server_ip_addr);
	int close(void);

	void getChannelInfo(void);
	void getFrame(void); 

	void showMarkers(int);
	void showBodies(int);

	BodyData getBodyData(string);
	MarkerData getMarkerData(string);

	std::vector< BodyData > ViconRTClient::getAllBodyData(void) { return bodyPositions; }
	std::vector< MarkerData > ViconRTClient::getAllMarkerData(void) { return markerPositions; }
	std::vector< BodyChannel >		BodyChannels;
	std::vector< MarkerChannel >	MarkerChannels;

private:

	std::vector< std::string > info;
	std::vector< MarkerData > markerPositions;
	std::vector< BodyData > bodyPositions;
	int	FrameChannel;

	//static const int bufferSize = 2040;
	char buff[2040];
	char * pBuff;

	SOCKET	SocketHandle;

	long int size;

};


#endif
