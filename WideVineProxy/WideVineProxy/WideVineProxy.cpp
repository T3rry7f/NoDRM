// WideVineProxy.cpp : 定义控制台应用程序的入口点。
//Author : T3rry7f
#include "stdafx.h"
#include "host_10.h"
#include<thread>
#include <string>
#include <string.h>
#include"guesskey\codelift.h"
#pragma comment (lib,"cryptlib.lib")

#pragma comment(lib, "ws2_32.lib")
#define CDM_MODULE_VERSION 4
#define MaxSize 4096

//using namespace std;
// Build the versioned entrypoint name.
// The extra macros are necessary to expand version to an actual value.
#define INITIALIZE_CDM_MODULE \
  BUILD_ENTRYPOINT(InitializeCdmModule, CDM_MODULE_VERSION)
#define BUILD_ENTRYPOINT(name, version) \
  BUILD_ENTRYPOINT_NO_EXPANSION(name, version)
#define BUILD_ENTRYPOINT_NO_EXPANSION(name, version) name##_##version



void* GetCdmHost(int host_interface_version, void* user_data)
{

	printf("Host interface version : %d\n", host_interface_version);

	
	return user_data;
}


extern  "C" {

	 void INITIALIZE_CDM_MODULE();

	 void DeinitializeCdmModule();

	// Returns a pointer to the requested CDM Host interface upon success.
	// Returns NULL if the requested CDM Host interface is not supported.
	// The caller should cast the returned pointer to the type matching
	// |host_interface_version|.
	typedef void* (*GetCdmHostFunc)(int host_interface_version, void* user_data);


	// Returns a pointer to the requested CDM upon success.
	// Returns NULL if an error occurs or the requested |cdm_interface_version| or
	// |key_system| is not supported or another error occurs.
	// The caller should cast the returned pointer to the type matching
	// |cdm_interface_version|.
	// Caller retains ownership of arguments and must call Destroy() on the returned
	// object.
	 void* CreateCdmInstance(int cdm_interface_version,
		const char* key_system,
		uint32_t key_system_size,
		GetCdmHostFunc get_cdm_host_func,
		void* user_data);

	 const char* GetCdmVersion();

}
HANDLE hMutex;

void Recv(Host_10 *host)
{
	
	char revBuf[MaxSize+1];
	while (1)
	{
		
		memset(revBuf,0, MaxSize);
		int len = 0;
		WaitForSingleObject(hMutex, INFINITE);

		len = recv((*host).sClient, revBuf, MaxSize, 0);

		//printf("length :%d\n", len);
		if (len <= 0)
		{
			
			break;
		}


		else if (512 == len) //encrypted session msg
		{
			std::string enHexKey;

			const char *session_key = tryUsingDecoder(revBuf);


			send((*host).sClient, session_key, 32, 0);

			printf("%s\n", session_key);
		}
		else
		{
			(*host).mod->CreateSessionAndGenerateRequest(2, (SessionType)1, (InitDataType)0, (uint8_t *)revBuf, (len));

		}

		printf("%s\n", revBuf);

		Sleep(1000);
		ReleaseMutex(hMutex);

	}
	closesocket((*host).sClient);
}






void dump_memory(void *ptr, uint32_t len) {
	auto bytes = (uint8_t*)ptr;
	for (uint32_t i = 0; i < len; i++) {
		fprintf(stderr, "%02x", bytes[i]);
	}
	fprintf(stderr, "\n");
}


int GenerteRequestData(Host_10 *host)
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}


	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}
	else {

		printf("Listening on 0.0.0.0.:8888 \n");
	}

	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	struct sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	//char revData[255];

	//printf("listening...\n");



	while (1)
	{
	 (*host).sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);


	 HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Recv, (LPVOID)host, 0, 0);		
	}
	closesocket((*host).sClient);
	closesocket(slisten);
	WSACleanup();

	return 0;
		 



}




void bytesToHex(const uint8_t* source, char* dest, int sourceLen)
{
	short i;
	unsigned char highByte, lowByte;

	for (i = 0; i < sourceLen; i++)
	{
		highByte = source[i] >> 4;
		lowByte = source[i] & 0x0f;

		highByte += 0x30;

		if (highByte > 0x39)
			dest[i * 2] = highByte + 0x07;
		else
			dest[i * 2] = highByte;

		lowByte += 0x30;
		if (lowByte > 0x39)
			dest[i * 2 + 1] = lowByte + 0x07;
		else
			dest[i * 2 + 1] = lowByte;
	}
	return;
}





int main()
{

	HINSTANCE  m_hDll = LoadLibrary(_T("widevinecdm.dll"));

	/*
	unsigned char pssh_bin[] = {
		0x00, 0x00, 0x00, 0xb5, 0x70, 0x73, 0x73, 0x68, 0x00, 0x00, 0x00, 0x00,
		0xed, 0xef, 0x8b, 0xa9, 0x79, 0xd6, 0x4a, 0xce, 0xa3, 0xc8, 0x27, 0xdc,
		0xd5, 0x1d, 0x21, 0xed, 0x00, 0x00, 0x00, 0x95, 0x12, 0x10, 0x76, 0xbb,
		0xc0, 0x8c, 0x47, 0x5f, 0x36, 0xcb, 0xb9, 0x1f, 0x65, 0x67, 0x10, 0x1f,
		0xe2, 0x59, 0x22, 0x80, 0x01, 0x7b, 0x22, 0x66, 0x69, 0x6c, 0x65, 0x5f,
		0x69, 0x64, 0x22, 0x3a, 0x22, 0x74, 0x64, 0x30, 0x32, 0x37, 0x73, 0x65,
		0x72, 0x6f, 0x30, 0x30, 0x33, 0x30, 0x38, 0x22, 0x2c, 0x22, 0x73, 0x65,
		0x72, 0x76, 0x69, 0x63, 0x65, 0x5f, 0x69, 0x64, 0x22, 0x3a, 0x22, 0x64,
		0x69, 0x67, 0x69, 0x74, 0x61, 0x6c, 0x22, 0x2c, 0x22, 0x70, 0x61, 0x79,
		0x6c, 0x6f, 0x61, 0x64, 0x22, 0x3a, 0x22, 0x65, 0x79, 0x4a, 0x77, 0x61,
		0x57, 0x51, 0x69, 0x4f, 0x69, 0x4a, 0x30, 0x5a, 0x44, 0x41, 0x79, 0x4e,
		0x33, 0x4e, 0x6c, 0x63, 0x6d, 0x38, 0x77, 0x4d, 0x44, 0x4d, 0x77, 0x4e,
		0x32, 0x52, 0x73, 0x4e, 0x69, 0x49, 0x73, 0x49, 0x6d, 0x52, 0x6c, 0x62,
		0x47, 0x6c, 0x32, 0x5a, 0x58, 0x4a, 0x35, 0x58, 0x33, 0x52, 0x35, 0x63,
		0x47, 0x55, 0x69, 0x4f, 0x69, 0x4a, 0x6b, 0x62, 0x43, 0x4a, 0x39, 0x22,
		0x7d
	};
	unsigned int pssh_bin_len = 181;
	*/

	if (m_hDll == NULL)
	{
		printf_s("cannot load cdm library\n");

		return -1;
	}
	typedef	  void* (*CreateCdmInstance)(int cdm_interface_version,
		const char* key_system,
		uint32_t key_system_size,
		GetCdmHostFunc get_cdm_host_func,
		void* user_data);

	typedef char* (*GetCdmVersion)();

	CreateCdmInstance cdm;

	GetCdmVersion getversion;

	GetCdmHostFunc hostfunc = GetCdmHost;

	Host_10 host;

	cdm = (CreateCdmInstance)GetProcAddress(m_hDll, "CreateCdmInstance");


	getversion = (GetCdmVersion)GetProcAddress(m_hDll, "GetCdmVersion");

	if (cdm == NULL)
	{
		FreeLibrary(m_hDll);
		printf("GetProcAddress failed\n");
	}

	

	host.mod =(ContentDecryptionModule_10 *)cdm(10,"com.widevine.alpha",19, hostfunc, (void *)&host);

	printf("Current CDM version :%s\n", getversion());

	host.mod->Initialize(0, 1, 0);

	//host.mod->SetServerCertificate(2,cert,711);

	GenerteRequestData(&host);


    return 0;
}

