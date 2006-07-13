#include <assert.h>
#include <stdio.h>
#include "IOP_Cdvdfsv.h"
#include "PS2VM.h"

using namespace IOP;
using namespace Framework;

uint32	CCdvdfsv::m_nStreamPos = 0;

CCdvdfsv::CCdvdfsv(uint32 nID)
{
	m_nID = nID;
}

CCdvdfsv::~CCdvdfsv()
{

}

void CCdvdfsv::Invoke(uint32 nMethod, void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	switch(m_nID)
	{
	case MODULE_ID_1:
		Invoke592(nMethod, pArgs, nArgsSize, pRet, nRetSize);
		break;
	case MODULE_ID_2:
		Invoke593(nMethod, pArgs, nArgsSize, pRet, nRetSize);
		break;
	case MODULE_ID_4:
		Invoke595(nMethod, pArgs, nArgsSize, pRet, nRetSize);
		break;
	case MODULE_ID_6:
		Invoke597(nMethod, pArgs, nArgsSize, pRet, nRetSize);
		break;
	case MODULE_ID_7:
		Invoke59C(nMethod, pArgs, nArgsSize, pRet, nRetSize);
		break;
	}
}

void CCdvdfsv::SaveState(CStream* pStream)
{
	if(m_nID != MODULE_ID_1) return;

	pStream->Write(&m_nStreamPos, 4);
}

void CCdvdfsv::LoadState(CStream* pStream)
{
	if(m_nID != MODULE_ID_1) return;

	pStream->Read(&m_nStreamPos, 4);
}

void CCdvdfsv::Invoke592(uint32 nMethod, void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	switch(nMethod)
	{
	case 0:
		//Init
		uint32 nMode;

		assert(nArgsSize >= 4);
		assert(nRetSize >= 0x10);

		nMode = ((uint32*)pArgs)[0x00];

		Log("Init(mode = %i);\r\n", nMode);

		((uint32*)pRet)[0x03] = 0xFF;

		break;
	default:
		Log("Unknown method invoked (0x%0.8X, 0x%0.8X).\r\n", m_nID, nMethod);
		break;
	}
}

void CCdvdfsv::Invoke593(uint32 nMethod, void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	switch(nMethod)
	{
	case 0x03:
		
		assert(nRetSize >= 4);
		
		Log("GetDiskType();\r\n");

		//Returns PS2DVD for now.
		((uint32*)pRet)[0x00] = 0x14;

		break;

	case 0x04:
		assert(nRetSize >= 4);

		Log("GetError();\r\n");

		((uint32*)pRet)[0x00] = 0x00;
		break;

	case 0x0C:
		//Status
		//Returns
		//0 - Stopped
		//1 - Open
		//2 - Spin
		//3 - Read
		//and more...

		assert(nRetSize >= 4);

		Log("Status();\r\n");

		((uint32*)pRet)[0x00] = 0;

		break;
	case 0x22:
		//Set Media Mode (1 - CDROM, 2 - DVDROM)
		uint32 nMode;

		assert(nArgsSize >= 4);
		assert(nRetSize >= 4);

		nMode = ((uint32*)pArgs)[0x00];

		Log("SetMediaMode(mode = %i);\r\n", nMode); 
		
		((uint32*)pRet)[0x00] = 1;

		break;
	default:
		Log("Unknown method invoked (0x%0.8X, 0x%0.8X).\r\n", m_nID, nMethod);
		break;
	}
}

void CCdvdfsv::Invoke595(uint32 nMethod, void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	switch(nMethod)
	{
	case 1:
		Read(pArgs, nArgsSize, pRet, nRetSize);
		break;
	case 4:
		//GetToc
		uint32 nBuffer;

		assert(nArgsSize >= 4);
		assert(nRetSize >= 4);

		nBuffer = ((uint32*)pArgs)[0x00];

		Log("GetToc(buffer = 0x%0.8X);\r\n", nBuffer);

		((uint32*)pRet)[0x00] = 1;
		break;
	case 9:
		StreamCmd(pArgs, nArgsSize, pRet, nRetSize);
		break;
	case 0x0E:
		//DiskReady (returns 2 if ready, 6 if not ready)

		assert(nRetSize >= 4);

		Log("NDiskReady();\r\n");

		((uint32*)pRet)[0x00] = 2;
		break;
	default:
		Log("Unknown method invoked (0x%0.8X, 0x%0.8X).\r\n", m_nID, nMethod);
		break;
	}
}

void CCdvdfsv::Invoke597(uint32 nMethod, void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	switch(nMethod)
	{
	case 0:
		SearchFile(pArgs, nArgsSize, pRet, nRetSize);
		break;
	default:
		Log("Unknown method invoked (0x%0.8X, 0x%0.8X).\r\n", m_nID, nMethod);
		break;
	}
}

void CCdvdfsv::Invoke59C(uint32 nMethod, void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	switch(nMethod)
	{
	case 0:
		//DiskReady (returns 2 if ready, 6 if not ready)

		uint32 nMode;

		assert(nRetSize >= 4);
		assert(nArgsSize >= 4);

		nMode = ((uint32*)pArgs)[0x00];

		Log("DiskReady(mode = %i);\r\n", nMode);

		((uint32*)pRet)[0x00] = 2;
		break;
	default:
		Log("Unknown method invoked (0x%0.8X, 0x%0.8X).\r\n", m_nID, nMethod);
		break;
	}
}

void CCdvdfsv::Read(void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	uint32 nSector, nCount, nDstAddr, nMode;
	unsigned int i;

	nSector		= ((uint32*)pArgs)[0x00];
	nCount		= ((uint32*)pArgs)[0x01];
	nDstAddr	= ((uint32*)pArgs)[0x02];
	nMode		= ((uint32*)pArgs)[0x04];

	Log("Read(sector = 0x%0.8X, count = 0x%0.8X, addr = 0x%0.8X, mode = 0x%0.8X);\r\n", \
		nSector,
		nCount,
		nDstAddr,
		nMode);

	for(i = 0; i < nCount; i++)
	{
		CPS2VM::m_pCDROM0->ReadBlock(nSector + i, CPS2VM::m_pRAM + (nDstAddr + (i * 0x800)));
	}

	if(nRetSize >= 4)
	{
		((uint32*)pRet)[0] = 0;
	}
}

void CCdvdfsv::StreamCmd(void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	uint32 nSector, nCount, nDstAddr, nCmd, nMode, i;

	nSector		= ((uint32*)pArgs)[0x00];
	nCount		= ((uint32*)pArgs)[0x01];
	nDstAddr	= ((uint32*)pArgs)[0x02];
	nCmd		= ((uint32*)pArgs)[0x03];
	nMode		= ((uint32*)pArgs)[0x04];

	Log("StreamCmd(sector = 0x%0.8X, count = 0x%0.8X, addr = 0x%0.8X, cmd = 0x%0.8X);\r\n", \
		nSector,
		nCount,
		nDstAddr,
		nMode);

	switch(nCmd)
	{
	case 1:
		//Start
		m_nStreamPos = nSector;
		((uint32*)pRet)[0] = 1;
		
		Log("StreamStart(pos = 0x%0.8X);\r\n", \
			nSector);
		break;
	case 2:
		//Read
		nDstAddr &= (CPS2VM::RAMSIZE - 1);

		for(i = 0; i < nCount; i++)
		{
			CPS2VM::m_pCDROM0->ReadBlock(m_nStreamPos, CPS2VM::m_pRAM + (nDstAddr + (i * 0x800)));
			m_nStreamPos++;
		}

		((uint32*)pRet)[0] = nCount;

		Log("StreamRead(count = 0x%0.8X, dest = 0x%0.8X);\r\n", \
			nCount, \
			nDstAddr);
		break;
	case 5:
		//Init
		((uint32*)pRet)[0] = 1;

		Log("StreamInit(bufsize = 0x%0.8X, numbuf = 0x%0.8X, buf = 0x%0.8X);\r\n", \
			nSector, \
			nCount, \
			nDstAddr);
		break;
	case 4:
	case 9:
		//Seek
		m_nStreamPos = nSector;
		((uint32*)pRet)[0] = 1;

		Log("StreamSeek(pos = 0x%0.8X);\r\n", \
			nSector);
		break;
	}
}

void CCdvdfsv::SearchFile(void* pArgs, uint32 nArgsSize, void* pRet, uint32 nRetSize)
{
	char* sPath;
	char* sTemp;
	char sFixedPath[256];
	CISO9660* pISO;
	ISO9660::CDirectoryRecord Record;

	assert(nArgsSize >= 0x128);
	assert(nRetSize == 4);

	pISO = CPS2VM::m_pCDROM0;
	if(pISO == NULL)
	{
		*(uint32*)pRet = 0;
		return;
	}

	//0x12C structure
	//00 - Block Num
	//04 - Size
	//08
	//0C
	//10
	//14
	//18
	//1C
	//20 - Unknown
	//24 - Path

	sPath = (char*)pArgs + 0x24;

	strcpy(sFixedPath, sPath);

	//Fix all slashes
	sTemp = strchr(sFixedPath, '\\');
	while(sTemp != NULL)
	{
		*sTemp = '/';
		sTemp = strchr(sTemp + 1, '\\');
	}

	Log("SearchFile(path = %s);\r\n", sFixedPath);

	if(!pISO->GetFileRecord(&Record, sFixedPath))
	{
		*(uint32*)pRet = 0;
		return;
	}

	((uint32*)pArgs)[0x00] = Record.GetPosition();
	((uint32*)pArgs)[0x01] = Record.GetDataLength();

	*(uint32*)pRet = 1;
}

void CCdvdfsv::Log(const char* sFormat, ...)
{
#ifdef _DEBUG

	if(!CPS2VM::m_Logging.GetIOPLoggingStatus()) return;

	va_list Args;
	printf("IOP_Cdvdfsv: ");
	va_start(Args, sFormat);
	vprintf(sFormat, Args);
	va_end(Args);

#endif
}
