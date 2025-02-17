#include "StdAfx.h"
#include "ADCSet.h"
#include "Parameter.h"
#include "resource.h"
#include <math.h>
#define PI	3.1415926
CADCSet::CADCSet(void)
: m_pTabADCSettings(NULL)
, m_pInstrumentList(NULL)
, m_uiIPSource(0)
, m_pLogFile(NULL)
, m_ADCSetSocket(INVALID_SOCKET)
, m_uiADCSetOperationNb(0)
, m_pWnd(NULL)
, m_uiTnow(0)
, m_pADCDataRecThread(NULL)
, m_uiADCSetOptPreviousNb(0)
, m_uiADCSetOptCount(0)
{
}

CADCSet::~CADCSet(void)
{
}
void CADCSet::OnReceive(void)
{
	// TODO: 在此添加专用代码和/或调用基类
	int ret=0;
	sockaddr_in SenderAddr;
	int n = sizeof(SenderAddr);
	ret = recvfrom(m_ADCSetSocket, reinterpret_cast<char *>(&udp_buf), sizeof(udp_buf), 0, reinterpret_cast<sockaddr *>(&SenderAddr), &n);
	if(ret == RcvFrameSize) 
	{
		unsigned short usCommand = 0;
		byte	ucCommand = 0;
		unsigned int uiIPAim = 0;
		int iPos = 0;
		unsigned short usCRC16 = 0;
		memcpy(&usCRC16, &udp_buf[RcvFrameSize - CRCSize], CRCSize);
		if (usCRC16 != get_crc_16(&udp_buf[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
		{
			//	return FALSE;
		}
		iPos = 16;
		memcpy(&uiIPAim, &udp_buf[iPos], FramePacketSize4B);
		iPos = 26;
		memcpy(&usCommand, &udp_buf[iPos], FramePacketSize2B);
		iPos += FramePacketSize2B;
		if (usCommand == SendQueryCmd)
		{
			memcpy(&ucCommand, &udp_buf[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (ucCommand == CmdBroadCastPortSet)
			{
				// ADC零漂校正查询应答
				OnProcADCZeroDriftReturn(uiIPAim);
				OnProcADCSetReturn(uiIPAim);
			}
		}
		else if (usCommand == SendSetCmd)
		{
			OnProcADCSetReturn(uiIPAim);
		}
	}
	else if (ret == SOCKET_ERROR)
	{
		int iError = 0;
		CString str = _T("");
		iError = WSAGetLastError();
		if (iError != WSAEWOULDBLOCK)
		{
			str.Format(_T("ADC参数设置应答接收帧错误，错误号为%d！"), iError);
			m_pLogFile->OnWriteLogFile(_T("CADCSet::OnReceive"), str, ErrorStatus);
		}	
	}
	else
	{
		CString str = _T("");
		str.Format(_T("ADC参数设置应答接收帧帧长错误，帧长为%d！"), ret);
		m_pLogFile->OnWriteLogFile(_T("CADCSet::OnReceive"), str, ErrorStatus);
	}
}
// 处理零漂校正查询应答
//************************************
// Method:    OnProcADCZeroDriftReturn
// FullName:  CSysTimeSocket::OnProcADCZeroDriftReturn
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnProcADCZeroDriftReturn(unsigned int uiIPAim)
{
	CString str = _T("");
	int iPos = 0;
	byte	ucCommand = 0;
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	CInstrument* pInstrument = NULL;
	if (m_pInstrumentList->GetInstrumentFromIPMap(uiIPAim, pInstrument))
	{
 		if (pInstrument->m_uiInstrumentType == InstrumentTypeFDU)
		{
			unsigned int uiLocation = 0;
			uiLocation = pInstrument->m_uiFDUIndex;
			iPos = 33;
			memcpy(&ucCommand, &udp_buf[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (ucCommand == CmdADCSet)
			{
				iPos = 41;
				memcpy(&m_ucZeroDrift[uiLocation][0], &udp_buf[iPos], FramePacketSize2B);
				iPos += FramePacketSize2B;
				iPos += FrameCmdSize1B;
				memcpy(&m_ucZeroDrift[uiLocation][2], &udp_buf[iPos], FramePacketSize2B);
				iPos += FramePacketSize2B;
				memcpy(&m_ucZeroDrift[uiLocation][4], &udp_buf[iPos], FramePacketSize2B);
				str.Format(_T("接收零漂矫正查询-仪器IP地址：%d！"), uiIPAim);
				m_pLogFile->OnWriteLogFile(_T("CADCSet::OnProcADCZeroDriftReturn"), str, SuccessStatus);
			}
			else if (ucCommand == CmdTBHigh)
			{
				unsigned int uiTBHigh = 0;
				unsigned int uiTBLow = 0;
				unsigned int uiSysTime = 0;
				memcpy(&uiTBHigh, &udp_buf[iPos], FramePacketSize4B);
				iPos += FramePacketSize4B;

				iPos += FrameCmdSize1B;
				memcpy(&uiTBLow, &udp_buf[iPos], FramePacketSize4B);
				iPos += FramePacketSize4B;

				iPos += FrameCmdSize1B;
				memcpy(&uiSysTime, &udp_buf[iPos], FramePacketSize4B);
				iPos += FramePacketSize4B;

				str.Format(_T("接收TB时刻查询帧-仪器IP地址：%d，TB高位为0x%04x，TB低位为0x%04x, 本地时间为0x%04x！"), uiIPAim, uiTBHigh, uiTBLow, uiSysTime);
				m_pLogFile->OnWriteLogFile(_T("CADCSet::OnProcADCZeroDriftReturn"), str, SuccessStatus);
			}
		}
	}
}

// ADC设置帧头
//************************************
// Method:    ADCSetFrameHead
// FullName:  CADCSet::ADCSetFrameHead
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned int uiIPAim
// Parameter: bool bBroadCast
// Parameter: unsigned short usCommand
// Parameter: unsigned short usPortAim
//************************************
int CADCSet::ADCSetFrameHead(unsigned int uiIPAim, unsigned short	usCommand, unsigned int uiBroadCastPort)
{
	unsigned int	uiIPSource	=	0;
	unsigned int	itmp		=	0;
	unsigned char	ucCommand = 0;
	unsigned short usPortAim = ADSetReturnPort;
	int iPos = 0;
	memset(m_ucFrameData, SndFrameBufInit, SndFrameSize);
	m_ucFrameData[0] = FrameHeadCheck0;
	m_ucFrameData[1] = FrameHeadCheck1;
	m_ucFrameData[2] = FrameHeadCheck2;
	m_ucFrameData[3] = FrameHeadCheck3;
	memset(&m_ucFrameData[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	uiIPSource	=	m_uiIPSource;
	iPos = 16;
	// 源IP地址
	memcpy(&m_ucFrameData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标IP地址
	memcpy(&m_ucFrameData[iPos], &uiIPAim, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标端口号
	memcpy(&m_ucFrameData[iPos], &usPortAim, FramePacketSize2B);
	iPos += FramePacketSize2B;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_ucFrameData[iPos], &usCommand, FramePacketSize2B);
	iPos += FramePacketSize2B;
	memcpy(&m_ucFrameData[iPos], &itmp, FramePacketSize4B);
	iPos += FramePacketSize4B;
// 	if ((uiIPAim == BroadCastPort) || (uiIPAim == IPSetAddrStart))
// 	{
// 		//广播命令
// 		ucCommand = CmdBroadCastPortSet;
// 		memcpy(&m_ucFrameData[iPos], &ucCommand, FrameCmdSize1B);
// 		iPos += FrameCmdSize1B;
// 		//广播命令端口
// 		uiADCBroadcastPort = ADCSetBroadcastPort;
// 		memcpy(&m_ucFrameData[iPos], &uiADCBroadcastPort, FramePacketSize4B);
// 		iPos += FramePacketSize4B;
// 	}
	if (uiIPAim == IPBroadcastAddr)
	{
		//广播命令
		ucCommand = CmdBroadCastPortSet;
		memcpy(&m_ucFrameData[iPos], &ucCommand, FrameCmdSize1B);
		iPos += FrameCmdSize1B;
		//广播命令端口
		memcpy(&m_ucFrameData[iPos], &uiBroadCastPort, FramePacketSize4B);
		iPos += FramePacketSize4B;
	}
	return iPos;
}

// 发送ADC设置的18指令
//************************************
// Method:    ADCCommand_18
// FullName:  CADCSet::ADCCommand_18
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: int iPos
// Parameter: byte* cADCSet
// Parameter: unsigned int uiLength
//************************************
int CADCSet::ADCCommand_18(int iPos, byte* cADCSet, unsigned int uiLength)
{
	for(unsigned int i=0; i<uiLength; i+=FramePacketSize4B)
	{
		m_ucFrameData[iPos] = CmdADCSet;
		iPos += FrameCmdSize1B;
		memcpy(&m_ucFrameData[iPos], cADCSet+i, FramePacketSize4B);
		iPos += FramePacketSize4B;
	}
	return iPos;
}

// 设置正弦输出
//************************************
// Method:    OnSetSine
// FullName:  CADCSet::OnSetSine
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnSetSine(int iPos)
{
	byte cSetSine[] = SetADCSetSine;
	iPos = ADCCommand_18(iPos, cSetSine, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 设置停止采集
//************************************
// Method:    OnStopSample
// FullName:  CADCSet::OnStopSample
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnStopSample(int iPos)
{
	byte cStopSample[] = SetADCStopSample;
	iPos = ADCCommand_18(iPos, cStopSample, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 打开AD、DA电源（tb位为0）
//************************************
// Method:    OnOpenPowerTBLow
// FullName:  CADCSet::OnOpenPowerTBLow
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnOpenPowerTBLow(int iPos)
{
	byte cOnOpenPowerTBLow[] = SetADCOpenTBPowerLow;
	iPos = ADCCommand_18(iPos, cOnOpenPowerTBLow, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 打开AD、DA电源（tb位为1）
//************************************
// Method:    OnOpenPowerTBHigh
// FullName:  CADCSet::OnOpenPowerTBHigh
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnOpenPowerTBHigh(int iPos)
{
	byte cOnOpenPowerTBHigh[] = SetADCOpenTBPowerHigh;
	iPos = ADCCommand_18(iPos, cOnOpenPowerTBHigh, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 打开AD、DA电源及ADC_RESET、ADC_PWR(tb位为0) 
//************************************
// Method:    OnOpenSwitchTBLow
// FullName:  CADCSet::OnOpenSwitchTBLow
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnOpenSwitchTBLow(int iPos)
{
	byte cOnOpenSwitchTBLow[] = SetADCOpenSwitchTBLow;
	iPos = ADCCommand_18(iPos, cOnOpenSwitchTBLow, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 打开AD、DA电源及ADC_RESET、ADC_PWR(tb位为1) 
//************************************
// Method:    OnOpenSwitchTBHigh
// FullName:  CADCSet::OnOpenSwitchTBHigh
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnOpenSwitchTBHigh(int iPos)
{
	byte cOnOpenSwitchTBHigh[] = SetADCOpenSwitchTBHigh;
	iPos = ADCCommand_18(iPos, cOnOpenSwitchTBHigh, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 读ADC寄存器
//************************************
// Method:    OnADCRegisterRead
// FullName:  CADCSet::OnADCRegisterRead
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnADCRegisterRead(int iPos)
{
	byte cADCRegisterRead[] = SetADCRegisterRead;
	iPos = ADCCommand_18(iPos, cADCRegisterRead, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 写ADC寄存器
void CADCSet::OnADCRegisterWrite(int iPos, bool bSwitch)
{
	byte cADCRegisterWrite[] = SetADCRegisterWrite;
	if (bSwitch == true)
	{
		cADCRegisterWrite[5] = 0x18;
	}
	iPos = ADCCommand_18(iPos, cADCRegisterWrite, 16);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// ADC设置参数返回
//************************************
// Method:    OnADCSetReturn
// FullName:  CADCSet::OnADCSetReturn
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnADCSetReturn(int iPos)
{
	unsigned int uiIPSource = 0;
	unsigned int uiReturnPort = 0;
	unsigned int uiReturnPortMove = 0;

	//自动AD返回地址
	uiIPSource = m_uiIPSource;
	m_ucFrameData[iPos] = CmdADCDataReturnAddr;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;

	// ad返回端口：返回固定端口则ad_cmd(7) = 0,
	// 例如0x00035005,其中03为ADC数据返回命令，5005为返回端口号
	// 返回端口递增则ad_cmd(7) = 1,例如0x80035005
	uiReturnPort = (SendADCCmd << 16) + ADRecPort;	// 0x00038300
	m_ucFrameData[iPos] = CmdADCDataReturnPort;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uiReturnPort, FramePacketSize4B);
	iPos += FramePacketSize4B;

	//端口递增上下限命令
	uiReturnPortMove = static_cast<unsigned int>((ADRecPort << 16) + ADRecPort);
	m_ucFrameData[iPos] = CmdADCDataReturnPortLimit;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uiReturnPortMove, FramePacketSize4B);
	iPos += FramePacketSize4B;

	//0-结束发送数据
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// ADC连续读取数据
//************************************
// Method:    OnADCReadContinuous
// FullName:  CADCSet::OnADCReadContinuous
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnADCReadContinuous(int iPos)
{
	byte cADCReadContinuous[] = SetADCReadContinuous;
	iPos = ADCCommand_18(iPos, cADCReadContinuous, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

//************************************
// Method:    OnSetTB
// FullName:  CADCSet::OnSetTB
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
// Parameter: unsigned int tnow
// Parameter: bool bSwitch
//************************************
void CADCSet::OnSetTB(int iPos, unsigned int tbh, unsigned int tbl, bool bSwitch)
{
	if (bSwitch == false)
	{
		//写TB时刻高位
		m_ucFrameData[iPos] = CmdTBHigh;
		iPos += FrameCmdSize1B;
		memcpy(&m_ucFrameData[iPos], &tbh, FramePacketSize4B);
		iPos += FramePacketSize4B;
	}
	//写TB时刻低位
	m_ucFrameData[iPos] = CmdTbLow;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &tbl, FramePacketSize4B);
	iPos += FramePacketSize4B;

	// 0-结束发送数据
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 打开零漂矫正电源及开关
//************************************
// Method:    OnOpenPowerZeroDrift
// FullName:  CADCSet::OnOpenPowerZeroDrift
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnOpenPowerZeroDrift(int iPos)
{
	byte cOnOpenPowerZeroDrift[] = SetADCOpenPowerZeroDrift;
	iPos = ADCCommand_18(iPos, cOnOpenPowerZeroDrift, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 查询ADC寄存器
//************************************
// Method:    OnADCRegisterQuery
// FullName:  CADCSet::OnADCRegisterQuery
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnADCRegisterQuery(int iPos)
{
	byte cOnADCRegisterQuery[] = SetADCRegisterQuery;
	iPos = ADCCommand_18(iPos, cOnADCRegisterQuery, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// ADC数据采集同步
//************************************
// Method:    OnADCSampleSynchronization
// FullName:  CADCSet::OnADCSampleSynchronization
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnADCSampleSynchronization(int iPos)
{
	byte cOnADCSampleSynchronization[] = SetADCSampleSynchronization;
	iPos = ADCCommand_18(iPos, cOnADCSampleSynchronization, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// ADC零漂偏移矫正
//************************************
// Method:    OnADCZeroDriftCorrection
// FullName:  CADCSet::OnADCZeroDriftCorrection
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnADCZeroDriftCorrection(int iPos)
{
	byte cOnADCZeroDriftCorrection[] = SetADCZeroDriftCorrection;
	iPos = ADCCommand_18(iPos, cOnADCZeroDriftCorrection, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// ADC零漂设置（按IP地址）
void CADCSet::OnADCZeroDriftSetFromIP(int iPos, unsigned char* ucZeroDrift)
{
	CString str = _T("");
	byte pga0 = 0,pga1 = 0,pga2 = 0;
	//sps1=1 1000k sps1=0 250k
	byte mux0 = 0,mux1 = 0,mux2 = 0,sps0 = 0,sps1 = 1,sps2 = 0,filtr0 = 0,filtr1 = 1;
	unsigned int uispsSelect = 0;
	byte mode = 1,sync = 0,phs = 0,chop = 1;
//	byte cOnADCZeroDriftSetFromIP[16] = {0x8d, 0x40, 0x0a, 0x00, 0x52, 0x08, 0x32, 0x03, 0x6f, 0x0c, 0xff, 0x7d, 0x52, 0x40, 0x00, 0x00};
	// 不设置零漂校正值
	byte cOnADCZeroDriftSetFromIP[8] = SetADCZeroDriftSetFromIP;
	unsigned int hpf = 0;
	double fhp = 0;
	double sps = 0;
	double wn = 0;


	m_pTabADCSettings->m_uiADCSync = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_SYNC))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCSync)
	{
	case 0:
		sync = 0;
		break;
	case 1:
		sync = 1;
		break;
	default:
		break;
	}
	m_pTabADCSettings->m_uiADCMode = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_MODE))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCMode)
	{
	case 0:
		mode = 0;
		break;
	case 1:
		mode = 1;
		break;
	default:
		break;
	}
	m_pTabADCSettings->m_uiADCSps = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_SPS))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCSps)
	{
	case 0:
		sps0 = 0;
		sps1 = 0;
		sps2 = 1;
		sps = 4000;
		break;
	case 1:
		sps0 = 1;
		sps1 = 1;
		sps2 = 0;
		sps = 2000;
		break;
	case 2:
		sps0 = 0;
		sps1 = 1;
		sps2 = 0;
		sps = 1000;
		break;
	case 3:
		sps0 = 1;
		sps1 = 0;
		sps2 = 0;
		sps = 500;
		break;
	case 4:
		sps0 = 0;
		sps1 = 0;
		sps2 = 0;
		sps = 250;
		break;
	default:
		break;
	}
	uispsSelect = sps0 + sps1*2 + sps2*4;
	// 幂运算用pow(X,Y)函数，x^y以X为底的Y次幂，需要math.h
/*	m_pADCDataRecThread->m_uispsSelect = pow(2, uispsSelect);*/
	m_pADCDataRecThread->m_uispsSelect = (0x01)<<uispsSelect;
	m_pTabADCSettings->m_uiADCPhs = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_PHS))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCPhs)
	{
	case 0:
		phs = 0;
		break;
	case 1:
		phs = 1;
		break;
	default:
		break;
	}
	m_pTabADCSettings->m_uiADCFilter = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_FILTER))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCFilter)
	{
	case 0:
		filtr0 = 0;
		filtr1 = 0;
		break;
	case 1:
		filtr0 = 1;
		filtr1 = 0;
		break;
	case 2:
		filtr0 = 0;
		filtr1 = 1;
		break;
	case 3:
		filtr0 = 1;
		filtr1 = 1;
		break;
	default:
		break;
	}
	m_pTabADCSettings->m_uiADCMux = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_MUX))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCMux)
	{
	case 0:
		mux0 = 0;
		mux1 = 0;
		mux2 = 0;
		break;
	case 1:
		mux0 = 1;
		mux1 = 0;
		mux2 = 0;
		break;
	case 2:
		mux0 = 0;
		mux1 = 1;
		mux2 = 0;
		break;
	case 3:
		mux0 = 1;
		mux1 = 1;
		mux2 = 0;
		break;
	case 4:
		mux0 = 0;
		mux1 = 0;
		mux2 = 1;
		break;
	default:
		break;
	}
	m_pTabADCSettings->m_uiADCChop = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_CHOP))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCChop)
	{
	case 0:
		chop = 0;
		break;
	case 1:
		chop = 1;
		break;
	default:
		break;
	}
	m_pTabADCSettings->m_uiADCPga = ((CComboBox*)m_pTabADCSettings->GetDlgItem(IDC_COMBO_PGA))->GetCurSel();
	switch(m_pTabADCSettings->m_uiADCPga)
	{
	case 0:
		pga0 = 0;
		pga1 = 0;
		pga2 = 0;
		break;
	case 1:
		pga0 = 1;
		pga1 = 0;
		pga2 = 0;
		break;
	case 2:
		pga0 = 0;
		pga1 = 1;
		pga2 = 0;
		break;
	case 3:
		pga0 = 1;
		pga1 = 1;
		pga2 = 0;
		break;
	case 4:
		pga0 = 0;
		pga1 = 0;
		pga2 = 1;
		break;
	case 5:
		pga0 = 1;
		pga1 = 0;
		pga2 = 1;
		break;
	case 6:
		pga0 = 0;
		pga1 = 1;
		pga2 = 1;
		break;
	default:
		break;
	}
	m_pTabADCSettings->GetDlgItem(IDC_EDIT_HP)->GetWindowText(str);
	fhp = _tstof(str);
	wn = 2 * PI * fhp/sps;
	hpf = static_cast<unsigned int>(65536 * (1 - sqrt(1 - 2* (cos(wn) + sin(wn) - 1)/cos(wn))));
	m_pTabADCSettings->m_ucHpfLow = static_cast<unsigned char>(hpf & 0xff);
	str.Format(_T("%d"), m_pTabADCSettings->m_ucHpfLow);
	m_pTabADCSettings->GetDlgItem(IDC_EDIT_HPFLOW)->SetWindowText(str);
	m_pTabADCSettings->m_ucHpfHigh = static_cast<unsigned char>(hpf >> 8);
	str.Format(_T("%d"), m_pTabADCSettings->m_ucHpfHigh);
	m_pTabADCSettings->GetDlgItem(IDC_EDIT_HPFHIGH)->SetWindowText(str);
	cOnADCZeroDriftSetFromIP[4] = (8*sync+4*mode+2*sps2+1*sps1)*16 + (8*sps0+4*phs+2*filtr1+1*filtr0);
	cOnADCZeroDriftSetFromIP[5] = (4*mux2+2*mux1+1*mux0)*16 + (8*chop+4*pga2+2*pga1+1*pga0);
	cOnADCZeroDriftSetFromIP[6] = m_pTabADCSettings->m_ucHpfLow;
	cOnADCZeroDriftSetFromIP[7] = m_pTabADCSettings->m_ucHpfHigh;
	
// 	cOnADCZeroDriftSetFromIP[8] = ucZeroDrift[0];
// 	cOnADCZeroDriftSetFromIP[9] = ucZeroDrift[1];
// 	cOnADCZeroDriftSetFromIP[10] = ucZeroDrift[2];
// 	cOnADCZeroDriftSetFromIP[11] = ucZeroDrift[3];
// 
// 	cOnADCZeroDriftSetFromIP[12] = ucZeroDrift[4];
// 	cOnADCZeroDriftSetFromIP[13] = ucZeroDrift[5];

	iPos = ADCCommand_18(iPos, cOnADCZeroDriftSetFromIP, 8);
	m_ucFrameData[iPos] = SndFrameBufInit;	

	OnCRCCal();
}
// 计算CRC校验值
//************************************
// Method:    OnCRCCal
// FullName:  CADCSet::OnCRCCal
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CADCSet::OnCRCCal(void)
{
	unsigned short usCRC16 = 0;
	usCRC16 = get_crc_16(&m_ucFrameData[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_ucFrameData[SndFrameSize - CRCSize], &usCRC16, CRCSize);
}

// 打开TB开关
//************************************
// Method:    OnSetTBSwitchOpen
// FullName:  CADCSet::OnSetTBSwitchOpen
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnSetTBSwitchOpen(int iPos)
{
	byte cOnOpenPowerZeroDrift[] = SetADCTBSwitchOpen;
	iPos = ADCCommand_18(iPos, cOnOpenPowerZeroDrift, FramePacketSize4B);
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}
// 查询采集站错误代码
//************************************
// Method:    OnQueryErrorCodeFdu
// FullName:  CADCSet::OnQueryErrorCodeFdu
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnQueryErrorCodeFdu(int iPos)
{
	unsigned int uitmp = 0;
	m_ucFrameData[iPos] = CmdFDUErrorCode;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uitmp, FramePacketSize4B);
	iPos += FramePacketSize4B;
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 查询交叉站故障计数
//************************************
// Method:    OnQueryErrorCodeLAUX
// FullName:  CADCSet::OnQueryErrorCodeLAUX
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iPos
//************************************
void CADCSet::OnQueryErrorCodeLAUX(int iPos)
{
	unsigned int uitmp = 0;
	m_ucFrameData[iPos] = CmdLAUXErrorCode1;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uitmp, FramePacketSize4B);
	iPos += FramePacketSize4B;
	m_ucFrameData[iPos] = CmdLAUXErrorCode2;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uitmp, FramePacketSize4B);
	iPos += FramePacketSize4B;
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 查询仪器误码
//************************************
// Method:    OnQueryErrorCode
// FullName:  CADCSet::OnQueryErrorCode
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CADCSet::OnQueryErrorCode(void)
{
	int iPos = 0;

	// 广播查询采集站
//	iPos = ADCSetFrameHead(BroadCastPort, SendQueryCmd, QueryErrorCodePort);
	OnQueryErrorCodeFdu(iPos);
	sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
	// 按IP地址查询交叉站
//	iPos = ADCSetFrameHead(IPSetAddrStart, SendQueryCmd, QueryErrorCodePort);
	OnQueryErrorCodeLAUX(iPos);
	sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
}

// ADC参数设置
//************************************
// Method:    OnADCSet
// FullName:  CADCSet::OnADCSet
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CADCSet::OnADCSet(void)
{
	m_pLogFile->OnWriteLogFile(_T("CADCSet::OnADCSet"), _T("广播发送ADC参数设置命令！"), SuccessStatus);
	m_uiADCSetOperationNb = 1;
	SetTimer(m_pWnd->m_hWnd, TabSampleADCSetReturnTimerNb, TabSampleADCSetReturnTimerSet, NULL);
	OnSendADCSetCmd();
}

// ADC数据采集停止
//************************************
// Method:    OnADCSampleStop
// FullName:  CADCSet::OnADCSampleStop
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CADCSet::OnADCSampleStop(void)
{
	m_pLogFile->OnWriteLogFile(_T("CADCSet::OnADCSampleStop"), _T("广播发送ADC数据采集停止命令！"), SuccessStatus);
	m_uiADCSetOperationNb = 35;
	SetTimer(m_pWnd->m_hWnd, TabSampleADCSetReturnTimerNb, TabSampleADCSetReturnTimerSet, NULL);
	OnSendADCSetCmd();
}

// ADC零漂校正
//************************************
// Method:    OnADCZeroDrift
// FullName:  CADCSet::OnADCZeroDrift
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CADCSet::OnADCZeroDrift(void)
{
	m_pLogFile->OnWriteLogFile(_T("CADCSet::OnADCZeroDrift"), _T("广播发送ADC零漂校正命令！"), SuccessStatus);
	m_uiADCSetOperationNb = 12;
	SetTimer(m_pWnd->m_hWnd, TabSampleADCSetReturnTimerNb, TabSampleADCSetReturnTimerSet, NULL);
	OnSendADCSetCmd();
}

// ADC设置TB时刻开始采集
//************************************
// Method:    OnADCStartSample
// FullName:  CADCSet::OnADCStartSample
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned int uiIPAim
// Parameter: unsigned int tnow
//************************************
void CADCSet::OnADCStartSample(unsigned int tnow)
{
	m_uiADCSetOperationNb = 28;
	m_uiTnow = tnow;
	SetTimer(m_pWnd->m_hWnd, TabSampleADCSetReturnTimerNb, TabSampleADCSetReturnTimerSet, NULL);
	OnSendADCSetCmd();
}
// 防止程序在循环中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CSysTimeSocket::ProcessMessages
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CADCSet::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
// 广播查询采集站TB时刻
void CADCSet::OnQueryTBTime(int iPos)
{
	CString str = _T("");
	str.Format(_T("查询采集站的TB时刻和本地时间！"));
	m_pLogFile->OnWriteLogFile(_T("CADCSet::OnQueryTBTime"), str, SuccessStatus);

	unsigned int uitmp = 0;
	m_ucFrameData[iPos] = CmdTBHigh;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uitmp, FramePacketSize4B);
	iPos += FramePacketSize4B;
	m_ucFrameData[iPos] = CmdTbLow;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uitmp, FramePacketSize4B);
	iPos += FramePacketSize4B;
	m_ucFrameData[iPos] = CmdLocalSysTime;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uitmp, FramePacketSize4B);
	iPos += FramePacketSize4B;
	m_ucFrameData[iPos] = SndFrameBufInit;
	OnCRCCal();
}

// 关闭UDP套接字
void CADCSet::OnCloseUDP(void)
{
	shutdown(m_ADCSetSocket, SD_BOTH);
	closesocket(m_ADCSetSocket);
	m_ADCSetSocket = INVALID_SOCKET;
}

// 处理ADC设置应答帧
void CADCSet::OnProcADCSetReturn(unsigned int uiIP)
{
	CInstrument* pInstrument = NULL;
	if (m_uiADCSetOperationNb == 0)
	{
		return;
	}
	if (TRUE == m_pInstrumentList->GetInstrumentFromIPMap(uiIP, pInstrument))
	{
		if (pInstrument->m_uiInstrumentType == InstrumentTypeFDU)
		{
			pInstrument->m_uiADCSetOperationNb = m_uiADCSetOperationNb;
			// 检查是否收到所有采集站的ADC命令应答
			if(TRUE == OnCheckADCSetReturn())
			{
				// 命令应答接收完全后的操作
				OnADCSetNextOpt();
			}
		} 
	}
	else
	{
		CString str = _T("");
		CString strtemp = _T("");
		str = _T("ADC命令应答的IP地址不在索引表中！\r\n");
		for (int i=0; i<RcvFrameSize; i++)
		{
			strtemp.Format(_T("%02x "), udp_buf[i]);
			str += strtemp;
		}
		m_pLogFile->OnWriteLogFile(_T("CADCSet::OnProcADCSetReturn"), str, ErrorStatus);
	}
}

// 检查是否收到所有采集站的ADC命令应答
BOOL CADCSet::OnCheckADCSetReturn(void)
{
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	CString str = _T("");
	for(iter=m_pInstrumentList->m_oInstrumentIPMap.begin(); iter!=m_pInstrumentList->m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if ((iter->second->m_iSelectObject == BST_CHECKED)&&(iter->second->m_bIPSetOK == true)
				&&(iter->second->m_uiInstrumentType == InstrumentTypeFDU))
			{
				if (iter->second->m_uiADCSetOperationNb != m_uiADCSetOperationNb)
				{
					return FALSE;
				}
			}
		}
	}
	// 收到全部应答后重置设备的应答变量
	OnResetADCOperationNb();
	str.Format(_T("ADC设置命令序号为%d的命令应答接收完全"), m_uiADCSetOperationNb);
	m_pLogFile->OnWriteLogFile(_T("CADCSet::OnCheckADCSetReturn"), str, SuccessStatus);
	return TRUE;
}

// 发送ADC命令设置帧
void CADCSet::OnSendADCSetCmd(void)
{
	int iPos = 0;
	CString str = _T("");
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	// 记录已经发送的路由方向
	hash_map<unsigned int, unsigned int> ::iterator  iter2;
	if (m_uiADCSetOptPreviousNb == m_uiADCSetOperationNb)
	{
		m_uiADCSetOptCount++;
	}
	else
	{
		m_uiADCSetOptPreviousNb = m_uiADCSetOperationNb;
		m_uiADCSetOptCount = 0;
	}
	// 1~11为ADC参数设置命令
	// 12~27为ADC零漂校正指令
	switch (m_uiADCSetOperationNb)
	{
	case 1:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			// 广播发送
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnSetTB(iPos, 0, 0, true);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 2:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnSetSine(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 3:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 4:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnOpenPowerTBLow(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 5:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnOpenPowerTBHigh(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 6:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnOpenSwitchTBLow(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 7:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnOpenSwitchTBHigh(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 8:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 9:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCRegisterWrite(iPos, false);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 10:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCRegisterRead(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 11:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCSetReturn(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 12:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnOpenPowerZeroDrift(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 13:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 14:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 15:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCRegisterWrite(iPos, true);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 16:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCRegisterRead(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 17:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendQueryCmd, iter2->second);
			OnADCRegisterQuery(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 18:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 19:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCSampleSynchronization(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 20:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCReadContinuous(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 21:
		Sleep(ADCReadContinuousSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 22:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 23:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCZeroDriftCorrection(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 24:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCReadContinuous(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 25:
		Sleep(ADCReadContinuousSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 26:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 27:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnADCRegisterRead(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 28:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnSetTBSwitchOpen(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 29:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 30:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnSetTB(iPos, 0, 0, true);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 31:
		Sleep(ADCOperationSleepTime);
		// 按照IP地址发送零漂矫正帧写寄存器
		for(iter=m_pInstrumentList->m_oInstrumentIPMap.begin(); iter!=m_pInstrumentList->m_oInstrumentIPMap.end(); iter++)
		{
			if (NULL != iter->second)
			{
				if ((iter->second->m_bIPSetOK == true)&&(iter->second->m_uiInstrumentType == InstrumentTypeFDU))
				{
					iPos = ADCSetFrameHead(iter->second->m_uiIPAddress, SendSetCmd, 0);
					OnADCZeroDriftSetFromIP(iPos, m_ucZeroDrift[iter->second->m_uiFDUIndex]);
					sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
				}
			}
		}
		break;
	case 32:
		Sleep(ADCOperationSleepTime);

		for(iter=m_pInstrumentList->m_oInstrumentIPMap.begin(); iter!=m_pInstrumentList->m_oInstrumentIPMap.end(); iter++)
		{
			//		ProcessMessages();
			if (NULL != iter->second)
			{
				if ((iter->second->m_uiInstrumentType == InstrumentTypeFDU) && (iter->second->m_iSelectObject == BST_CHECKED)
					&&(iter->second->m_bIPSetOK == true))
				{
					iPos = ADCSetFrameHead(iter->second->m_uiIPAddress, SendSetCmd, 0);
					OnADCReadContinuous(iPos);
					str.Format(_T("向IP地址为%d的仪器发送连续ADC数据采样的命令！"), iter->second->m_uiIPAddress);
					m_pLogFile->OnWriteLogFile(_T("CADCSet::OnADCStartSample"), str, SuccessStatus);
					sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
				}
			}
		}
		break;
	case 33:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnSetTB(iPos, m_uiTnow + TBSleepTimeHigh, TBSleepTimeLow + CmdTBCtrl, false);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		str.Format(_T("设置ADC数据采样TB开始时间为0x%x！"), m_uiTnow + TBSleepTimeHigh);
		m_pLogFile->OnWriteLogFile(_T("CADCSet::OnADCStartSample"), str, SuccessStatus);
		break;
	case 34:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			// 广播查询采集站TB时刻
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendQueryCmd, iter2->second);
			OnQueryTBTime(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 35:
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 36:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 37:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnStopSample(iPos);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	case 38:
		Sleep(ADCOperationSleepTime);
		for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
		{
			iPos = ADCSetFrameHead(IPBroadcastAddr, SendSetCmd, iter2->second);
			OnSetTB(iPos, 0, 0, true);
			sendto(m_ADCSetSocket, reinterpret_cast<const char*>(&m_ucFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		}
		break;
	default:
		break;
	}
}

// 重置ADC参数设置操作序号
void CADCSet::OnResetADCOperationNb(void)
{
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	for(iter=m_pInstrumentList->m_oInstrumentIPMap.begin(); iter!=m_pInstrumentList->m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if (iter->second->m_bIPSetOK == true)
			{
				if (iter->second->m_uiInstrumentType == InstrumentTypeFDU)
				{
					iter->second->m_uiADCSetOperationNb = 0;
				}
			}
		}
	}
}
// ADC参数设置下一步操作
void CADCSet::OnADCSetNextOpt(void)
{
	// 关闭应答监视定时器
	KillTimer(m_pWnd->m_hWnd, TabSampleADCSetReturnTimerNb);
	// 执行下一步ADC命令发送并开启应答监视定时器
	m_uiADCSetOperationNb++;
	// 完成ADC参数设置
	if (m_uiADCSetOperationNb == 12)
	{
		m_uiADCSetOperationNb = 0;
		//************************暂不做零漂校正********************************
		//				SetTimer(m_pwnd->m_hWnd, TabSampleADCZeroDriftTimerNb, TabSampleADCZeroDriftTimerSet, NULL);
		m_pWnd->GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(TRUE);
		m_pWnd->GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(FALSE);
		m_pWnd->GetDlgItem(IDC_BUTTON_SETBYHAND)->EnableWindow(TRUE);
		//************************暂不做零漂校正********************************
		return;
	}
	// 完成ADC零漂校正
	else if (m_uiADCSetOperationNb == 28)
	{
		m_pWnd->GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(TRUE);
		m_pWnd->GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(FALSE);
		m_pWnd->GetDlgItem(IDC_BUTTON_SETBYHAND)->EnableWindow(TRUE);
		m_uiADCSetOperationNb = 0;
		return;
	}
	// 完成ADC开始数据采集
	else if (m_uiADCSetOperationNb == 35)
	{
		m_uiADCSetOperationNb = 0;
		SetTimer(m_pWnd->m_hWnd, TabSampleStartSampleTimerNb, TabSampleStartSampleTimerSet, NULL);
		m_pWnd->GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(TRUE);
		return;
	}
	// 完成ADC停止数据采集
	else if (m_uiADCSetOperationNb == 39)
	{
		m_uiADCSetOperationNb = 0;
		SetTimer(m_pWnd->m_hWnd, TabSampleStopSampleTimerNb, TabSampleStopSampleTimerSet, NULL);
		KillTimer(m_pWnd->m_hWnd, TabSampleQueryErrorCountTimerNb);
		return;
	}
	SetTimer(m_pWnd->m_hWnd, TabSampleADCSetReturnTimerNb, TabSampleADCSetReturnTimerSet, NULL);
	OnSendADCSetCmd();
}
