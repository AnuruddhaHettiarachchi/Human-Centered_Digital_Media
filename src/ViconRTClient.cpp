#include "ViconRTClient.h"



void ViconRTClient::showMarkers(int toShow) {
 
	
	std::vector< MarkerChannel >::iterator iMarker;
	std::vector< MarkerData >::iterator iMarkerData;

	#ifdef VC_VERBOSE
	std::cout << "ViconRTClient: Showing Channel Data" << std::endl;
	#endif VC_VERBOSE

	for(	iMarker = MarkerChannels.begin(), iMarkerData = markerPositions.begin(); 
			iMarker != MarkerChannels.end(); 
			iMarker++, iMarkerData++) {

		if (toShow == BRIEF) {
			cout << iMarker->Name << endl; 
		} else {
		cout << iMarker->Name << " "; 
		cout << iMarkerData->X << ", " << iMarkerData->Y << ", " <<	iMarkerData->Z << endl;
		}
	}
}

void ViconRTClient::showBodies(int toShow) {

	
	std::vector< BodyChannel >::iterator iBody;
	std::vector< BodyData >::iterator iBodyData;


	std::cout << "ViconRTClient: Showing Body Data" << std::endl;

	for(	iBody = BodyChannels.begin(), 
			iBodyData = bodyPositions.begin(); 
			iBody != BodyChannels.end(); iBody++, iBodyData++)
		{

		if (toShow == BRIEF) {
			cout << iBody->Name << endl; 
		} else {
		cout << iBody->Name << " pos("; 
		cout << iBodyData->TX << ", " << iBodyData->TY << ", " <<	iBodyData->TZ << ") ";
		cout << " rot(" << iBodyData->EulerX << ", " << iBodyData->EulerY << ", " <<	iBodyData->EulerZ << ") " << endl;
		}
	}
}



int ViconRTClient::initialize(char* server_ip_addr) {

	
	#ifdef VC_VERBOSE
	std::cout << "ViconRTClient: Initializing Winsock" << std::endl;
	#endif VC_VERBOSE

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 0 ); 
	if(WSAStartup( wVersionRequested, &wsaData ) != 0)
	{
		std::cout << "ViconRTClient: Socket Initialization Error" << std::endl;
		return 1;
	}
	
	// Create Socket

	SocketHandle = INVALID_SOCKET;
	
	struct protoent*	pProtocolInfoEntry;
	char*	protocol;
	int		type;

	protocol = "tcp";
	type = SOCK_STREAM;

	pProtocolInfoEntry = getprotobyname(protocol);
	assert(pProtocolInfoEntry);

	if(pProtocolInfoEntry)
		SocketHandle = socket(PF_INET, type, pProtocolInfoEntry->p_proto);

	if(SocketHandle == INVALID_SOCKET)
	{
		std::cout << "ViconRTClient: Socket Creation Error" << std::endl;
		return 1;
	}


	#ifdef VC_VERBOSE
	std::cout << "ViconRTClient: Initializing Socket Connection to " << server_ip_addr << std::endl;
	#endif VC_VERBOSE

	//	Find Endpoint

	struct hostent*		pHostInfoEntry;
	struct sockaddr_in	Endpoint;

	static const int port = 800;

	memset(&Endpoint, 0, sizeof(Endpoint));
	Endpoint.sin_family	= AF_INET;
	Endpoint.sin_port	= htons(port);
	
	pHostInfoEntry = gethostbyname(server_ip_addr);

	if(pHostInfoEntry)
		memcpy(&Endpoint.sin_addr,	pHostInfoEntry->h_addr, 
										pHostInfoEntry->h_length);
	else
		Endpoint.sin_addr.s_addr = inet_addr(server_ip_addr);

	if(Endpoint.sin_addr.s_addr == INADDR_NONE)
	{
		std::cout << "ViconRTClient: Bad Address" << std::endl;
		return 1;
	}

	//	Create Socket

	int result = connect(	SocketHandle, (struct sockaddr*) & Endpoint, sizeof(Endpoint));

	if(result == SOCKET_ERROR)
	{
		std::cout << "ViconRTClient: Failed to create Socket" << std::endl;
		int e = WSAGetLastError();
		return 1;
	}

	return 0;

}


int ViconRTClient::close(void) {


	if(closesocket(SocketHandle) == SOCKET_ERROR)
	{
		std::cout << "ViconRTClient: Failed to close Socket" << std::endl;
		return 1;
	}

	WSACleanup();
	#ifdef VC_VERBOSE
	std::cout << "ViconRTClient: De-Initialized WinSock" << std::endl;
	#endif VC_VERBOSE

	return 0;

}


//-----------------------------------------------------------------------------
//	The recv call may return with a half-full buffer.
//	revieve keeps going until the buffer is actually full.

bool receive(SOCKET Socket, char * pBuffer, int BufferSize)  
{
	char * p = pBuffer;
	char * e = pBuffer + BufferSize;

	int result;

	while(p != e)
	{
		result = recv(	Socket, p, e - p, 0 );

		if(result == SOCKET_ERROR)
			return false;
		
		p += result;
	}

	return true;
}

//	There are also some helpers to make the code a little less ugly.

bool receive(SOCKET Socket, long int & Val)
{
	return receive(Socket, (char*) & Val, sizeof(Val));
}

bool receive(SOCKET Socket, unsigned long int & Val)
{
	return receive(Socket, (char*) & Val, sizeof(Val));
}

bool receive(SOCKET Socket, double & Val)
{
	return receive(Socket, (char*) & Val, sizeof(Val));
}


void ViconRTClient::getChannelInfo(void) {

	#ifdef VC_VERBOSE
	cout << "ViconRTClient: Getting Channel Info" << endl;
	#endif VC_VERBOSE



	try
		{

		//- Get Info - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//	Request the channel information

		pBuff = buff;

		* ((long int *) pBuff) = ClientCodes::EInfo;
		pBuff += sizeof(long int);
		* ((long int *) pBuff) = ClientCodes::ERequest;
		pBuff += sizeof(long int);

		if(send(SocketHandle, buff, pBuff - buff, 0) == SOCKET_ERROR)
			throw std::string("Error Requesting");

		long int packet;
		long int type;


		if(!receive(SocketHandle, packet))
			throw std::string("Error Recieving");



		if(!receive(SocketHandle, type))
			throw std::string("Error Recieving");

		if(type != ClientCodes::EReply)
			throw std::string("Bad Packet");

		if(packet != ClientCodes::EInfo)
			throw std::string("Bad Reply Type");
			
		if(!receive(SocketHandle, size))
			throw std::string();

		info.resize(size);

		std::vector< std::string >::iterator iInfo;

		for(iInfo = info.begin(); iInfo != info.end(); iInfo++)
		{
			long int s;
			char c[255];
			char * p = c;

			if(!receive(SocketHandle, s)) 
				throw std::string();

			if(!receive(SocketHandle, c, s)) 
				throw std::string();
			
			p += s;
			
			*p = 0;
			
			*iInfo = std::string(c);
		}

		//- Parse Info - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//	The info packets now contain the channel names.
		//	Identify the channels with the various dof's.
		

		for(iInfo = info.begin(); iInfo != info.end(); iInfo++)
		{
			//	Extract the channel type

			int openBrace = iInfo->find('<');
			
			if(openBrace == iInfo->npos) 
				throw std::string("Bad Channel Id");
		
			int closeBrace = iInfo->find('>');
			
			if(closeBrace == iInfo->npos) 
				throw std::string("Bad Channel Id");

			closeBrace++;

			std::string Type = iInfo->substr(openBrace, closeBrace-openBrace);

			// Sometimes the .v files use lower case characters in the channel names, but
			// we need them all to be uppercase to make this consistent
			transform(	Type.begin(), Type.end(),	// source 
						Type.begin(),				// destination
						toupper);					// uppercase the character

			//	Extract the Name
			std::string Name = iInfo->substr(0, openBrace);

			int space = Name.rfind(' ');
			
			if(space != Name.npos) 
				Name.resize(space);

			std::vector< MarkerChannel >::iterator iMarker;
			std::vector< BodyChannel >::iterator iBody;
			std::vector< std::string >::const_iterator iTypes;

			iMarker = std::find(	MarkerChannels.begin(), 
									MarkerChannels.end(), Name);

			iBody = std::find(BodyChannels.begin(), BodyChannels.end(), Name);

			if(iMarker != MarkerChannels.end())
			{
				//	The channel is for a marker we already have.
				iTypes = std::find(	ClientCodes::MarkerTokens.begin(), ClientCodes::MarkerTokens.end(), Type);
				if(iTypes != ClientCodes::MarkerTokens.end())
					iMarker->operator[](iTypes - ClientCodes::MarkerTokens.begin()) = iInfo - info.begin();
			}
			else
			if(iBody != BodyChannels.end())
			{
				//	The channel is for a body we already have.
				iTypes = std::find(ClientCodes::BodyTokens.begin(), ClientCodes::BodyTokens.end(), Type);
				if(iTypes != ClientCodes::BodyTokens.end())
					iBody->operator[](iTypes - ClientCodes::BodyTokens.begin()) = iInfo - info.begin();
			}
			else
			if((iTypes = std::find(ClientCodes::MarkerTokens.begin(), ClientCodes::MarkerTokens.end(), Type))
					!= ClientCodes::MarkerTokens.end())
			{
				//	Its a new marker.
				MarkerChannels.push_back(MarkerChannel(Name));
				MarkerChannels.back()[iTypes - ClientCodes::MarkerTokens.begin()] = iInfo - info.begin();
			}
			else
			if((iTypes = std::find(ClientCodes::BodyTokens.begin(), ClientCodes::BodyTokens.end(), Type))
					!= ClientCodes::BodyTokens.end())
			{
				//	Its a new body.
				BodyChannels.push_back(BodyChannel(Name));
				BodyChannels.back()[iTypes - ClientCodes::BodyTokens.begin()] = iInfo - info.begin();
			}
			else
			if(Type == "<F>")
			{
				FrameChannel = iInfo - info.begin();
			}
			else
			{
				//	It could be a new channel type.
				std::cout << "ViconRTClient: WARNING, Unknown Channel Type: " << Type << std::endl;
			}

		}

		markerPositions.resize(MarkerChannels.size());
		bodyPositions.resize(BodyChannels.size());
	}

	catch(const std::string & rMsg)
	{
		cout << "ViconRTClient: ERROR in getChannelInfo --> ";
		if(rMsg.empty())
			std::cout << "Unknown Error" << std::endl;
		else
			std::cout << rMsg.c_str() << std::endl;
	}
}


void ViconRTClient::getFrame(void) {

	//	A connection with the Vicon Realtime system is now open.
	//	The following section implements the new Computer Graphics Client interface.

	#ifdef VC_VERBOSE
	cout << "ViconRTClient: Getting Frame" << endl;
	#endif VC_VERBOSE

	try
	{

		//- Get Data - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
		//	Get the data using the request/reply protocol.

	
		std::vector< double > data;
		data.resize(info.size());

		double timestamp;



		pBuff = buff;

		* ((long int *) pBuff) = ClientCodes::EData;
		pBuff += sizeof(long int);
		* ((long int *) pBuff) = ClientCodes::ERequest;
		pBuff += sizeof(long int);

		if(send(SocketHandle, buff, pBuff - buff, 0) == SOCKET_ERROR)
			throw std::string("Error Requesting");

		long int packet;
		long int type;

		//	Get and check the packet header.

		// timer loop to avoid deadlocking on receive
		if (VC_PAUSE > 0) 
		{
			DWORD dwStart = timeGetTime();
			while ( timeGetTime() - dwStart < 30 );
		}

		if(!receive(SocketHandle, packet))
			throw std::string("Error Recieving");

		if(!receive(SocketHandle, type))
			throw std::string("Error Recieving");

		if(type != ClientCodes::EReply)
			throw std::string("Bad Packet");

		if(packet != ClientCodes::EData)
			throw std::string("Bad Reply Type");

		if(!receive(SocketHandle, size))
			throw std::string();

		if(size != info.size())
			throw std::string("Bad Data Packet");

		//	Get the data.
		
		std::vector< double >::iterator iData;

		for(iData = data.begin(); iData != data.end(); iData++)
		{	
			if(!receive(SocketHandle, *iData)) 
				throw std::string();
		}

		//- Look Up Channels - - - - - - - - - - - - - - - - - - - - - - - 
		//  Get the TimeStamp

		timestamp = data[FrameChannel];

		//	Get the channels corresponding to the markers.
		//	Y is up
		//	The values are in millimeters

		std::vector< MarkerChannel >::iterator iMarker;
		std::vector< MarkerData >::iterator iMarkerData;

		for(	iMarker = MarkerChannels.begin(), 
				iMarkerData = markerPositions.begin(); 
				iMarker != MarkerChannels.end(); iMarker++, iMarkerData++)
		{
			iMarkerData->X = data[iMarker->X];
			iMarkerData->Y = data[iMarker->Y];
			iMarkerData->Z = data[iMarker->Z];
			if(data[iMarker->O] > 0.5)
				iMarkerData->Visible = false;
			else
				iMarkerData->Visible = true;
		}

		

		//	Get the channels corresponding to the bodies.
		//=================================================================
		//	The bodies are in global space
		//	The world is Z-up
		//	The translational values are in millimeters
		//	The rotational values are in radians
		//=================================================================

		std::vector< BodyChannel >::iterator iBody;
		std::vector< BodyData >::iterator iBodyData;

		for(	iBody = BodyChannels.begin(), 
				iBodyData = bodyPositions.begin(); 
				iBody != BodyChannels.end(); iBody++, iBodyData++)
		{

			iBodyData->TX = data[iBody->TX];
			iBodyData->TY = data[iBody->TY];
			iBodyData->TZ = data[iBody->TZ];
			
			iBodyData->RX = data[iBody->RX];
			iBodyData->RY = data[iBody->RY];
			iBodyData->RZ = data[iBody->RZ];

			//	The channel data is in the angle-axis form.
			//	The following converts this to a quaternion.
			//=============================================================
			//	An angle-axis is vector, the direction of which is the axis
			//	of rotation and the length of which is the amount of 
			//	rotation in radians. 
			//=============================================================

			double len, tmp;

			len = sqrt(	data[iBody->RX] * data[iBody->RX] + 
						data[iBody->RY] * data[iBody->RY] + 
						data[iBody->RZ] * data[iBody->RZ]);

			iBodyData->QW = cos(len / 2.0);
			tmp = sin(len / 2.0);
			if (len < 1e-10) 
			{
				iBodyData->QX = data[iBody->RX];
				iBodyData->QY = data[iBody->RY];
				iBodyData->QZ = data[iBody->RZ];
			} 
			else 
			{
				iBodyData->QX = data[iBody->RX] * tmp/len;
				iBodyData->QY = data[iBody->RY] * tmp/len;
				iBodyData->QZ = data[iBody->RZ] * tmp/len;
			}

			//	The following converts angle-axis to a rotation matrix.

			double c, s, x, y, z;

			if (len < 1e-15)
			{
				// make it the identity matrix
				iBodyData->GlobalRotation[0][0] = iBodyData->GlobalRotation[1][1] = iBodyData->GlobalRotation[2][2] = 1.0;
				iBodyData->GlobalRotation[0][1] = iBodyData->GlobalRotation[0][2] = iBodyData->GlobalRotation[1][0] = 0.0;
				iBodyData->GlobalRotation[1][2]	= iBodyData->GlobalRotation[2][0] = iBodyData->GlobalRotation[2][1] = 0.0;
			}
			else
			{
				x = data[iBody->RX]/len;
				y = data[iBody->RY]/len;
				z = data[iBody->RZ]/len;

				c = cos(len);
				s = sin(len);

				iBodyData->GlobalRotation[0][0] = c + (1-c)*x*x;
				iBodyData->GlobalRotation[0][1] =     (1-c)*x*y + s*(-z);
				iBodyData->GlobalRotation[0][2] =     (1-c)*x*z + s*y;
				iBodyData->GlobalRotation[1][0] =     (1-c)*y*x + s*z;
				iBodyData->GlobalRotation[1][1] = c + (1-c)*y*y;
				iBodyData->GlobalRotation[1][2] =     (1-c)*y*z + s*(-x);
				iBodyData->GlobalRotation[2][0] =     (1-c)*z*x + s*(-y);
				iBodyData->GlobalRotation[2][1] =     (1-c)*z*y + s*x;
				iBodyData->GlobalRotation[2][2] = c + (1-c)*z*z;
			}

			// now convert rotation matrix to nasty Euler angles (yuk)
			// you could convert direct from angle-axis to Euler if you wish

			//	'Look out for angle-flips, Paul...'
			//  Algorithm: GraphicsGems II - Matrix Techniques VII.1 p 320
			assert(fabs(iBodyData->GlobalRotation[0][2]) <= 1);
			iBodyData->EulerY = asin(-iBodyData->GlobalRotation[2][0]);

			//if(abs(cos(y)) > std::numeric_limits<double>::epsilon() ) 	// cos(y) != 0 Gimbal-Lock
			//{
				iBodyData->EulerX = atan2(iBodyData->GlobalRotation[2][1], iBodyData->GlobalRotation[2][2]);
				iBodyData->EulerZ = atan2(iBodyData->GlobalRotation[1][0], iBodyData->GlobalRotation[0][0]);
//			}
/////*			else
//			{
//				iBodyData->EulerZ = 0;
//				iBodyData->EulerX = atan2(iBodyData->GlobalRotation[0][1], iBodyData->GlobalRotation[1][1]);
//			}
}


		//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//  The marker and body data now resides in the arrays 
		//	markerPositions & bodyPositions.
		#ifdef VC_VERBOSE
		std::cout << "ViconRTClient: Frame: " << timestamp << std::endl;
		#endif VC_VERBOSE

	}

	catch(const std::string & rMsg)
	{
		cout << "ViconRTClient: ERROR in getFrame --> ";
		if(rMsg.empty())
			std::cout << "Unknown Error" << std::endl;
		else
			std::cout << rMsg.c_str() << std::endl;
	}

}

//////////////////////////////////////////////////////////////////////
//
// getBodyData
//
//////////////////////////////////////////////////////////////////////
// returns body data for requested body name
//
BodyData ViconRTClient::getBodyData(std::string Name) 
{
	std::vector< BodyChannel >::iterator iBody;

	iBody = std::find(BodyChannels.begin(), BodyChannels.end(), Name);

	if(iBody != BodyChannels.end()) 
	{
		return bodyPositions[iBody - BodyChannels.begin()];
		//cout << iBody->Name << " " << iBody << endl;
		//cout << BodyChannels[i].Name;
		//printf(" (%4.0f, %4.0f, %4.0f)\n",bodyPositions[i].TX, bodyPositions[i].TY, bodyPositions[i].TZ);
		//cout << " (" << bodyPositions[i].TX << ", " << bodyPositions[i].TY << ", " <<	bodyPositions[i].TZ << ") ";

	} else {
		cout << "ViconRTClient: ERROR, Body " << Name << " not found." << endl;
		return bodyPositions[0]; // this is not a great return value, but I can't figure out a nicer way
		//return (BodyData)1;  // something like this would be preferred
	}
}

//////////////////////////////////////////////////////////////////////
//
// getMarkerData
//
//////////////////////////////////////////////////////////////////////
// returns marker data for requested marker name
//
MarkerData ViconRTClient::getMarkerData(std::string Name) 
{
	std::vector< MarkerChannel >::iterator iMarker;

	// find the marker name in the channels vector
	iMarker = std::find(MarkerChannels.begin(), MarkerChannels.end(), Name);

	if(iMarker != MarkerChannels.end()) 
	{
		return markerPositions[iMarker - MarkerChannels.begin()];
	
	} else {
		cout << "ViconRTClient: ERROR, Marker " << Name << " not found." << endl;
		return markerPositions[0]; // this is not a great return value, but I can't figure out a nicer way
		//return (MarkerData)1;  // something like this would be preferred
	}
}

