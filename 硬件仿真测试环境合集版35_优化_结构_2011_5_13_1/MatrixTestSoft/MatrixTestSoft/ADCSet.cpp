#include "StdAfx.h"
#include "ADCSet.h"
#include "Parameter.h"
#include "resource.h"
CADCSet::CADCSet(void)
: m_pTabADCSettings(NULL)
, m_pInstrumentList(NULL)
, m_csIPSource(_T(""))
, m_uiSendPort(0)
, m_pSelectObject(NULL)
{
}

CADCSet::~CADCSet(void)
{
}
void CADCSet::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	int ret=0;
	ret = Receive(udp_buf, RcvFrameSize);
	if(ret == RcvFrameSize) 
	{
		unsigned short usCommand = 0;
		byte	ucCommand = 0;
		int iPos = 26;
		unsigned short usCRC16 = 0;
		memcpy(&usCRC16, &udp_buf[RcvFrameSize - CRCSize], CRCSize);
		if (usCRC16 != get_crc_16(&udp_buf[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
		{
			//	return FALSE;
		}
		memcpy(&usCommand, &udp_buf[iPos], FramePacketSize2B);
		iPos += FramePacketSize2B;
		if (usCommand == SendQueryCmd)
		{
			memcpy(&ucCommand, &udp_buf[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (ucCommand == CmdBroadCastPortSet)
			{
				// ADC零漂校正查询应答
				iPos = 16;
				OnProcADCZeroDriftReturn(iPos);
			}
		}
	}
	CSocket::OnReceive(nErrorCode);
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
void CADCSet::OnProcADCZeroDriftReturn(int iPos)
{
	unsigned int uiIPAim = 0;
	memcpy(&uiIPAim, &udp_buf[iPos], FramePacketSize4B);
	TRACE1("接收零漂矫正查询-仪器IP地址：%d\r\n", uiIPAim);
	for (int i=0; i<InstrumentNum; i++)
	{
		ProcessMessages();
		if (uiIPAim == IPSetAddrStart + (i + 1) * IPSetAddrInterval)
		{
			iPos = 0x29;
			memcpy(&m_ucZeroDrift[i][0], &udp_buf[iPos], FramePacketSize2B);
			iPos += FramePacketSize2B;
			iPos += FrameCmdSize1B;
			memcpy(&m_ucZeroDrift[i][2], &udp_buf[iPos], FramePacketSize2B);
			iPos += FramePacketSize2B;
			memcpy(&m_ucZeroDrift[i][4], &udp_buf[iPos], FramePacketSize2B);
			break;
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
int CADCSet::ADCSetFrameHead(unsigned int uiIPAim, unsigned short	usCommand, unsigned short usPortAim)
{
	unsigned int	uiIPSource	=	0;
	unsigned int	itmp		=	0;
	unsigned char	ucCommand = 0;
	unsigned int	uiADCBroadcastPort = 0;
	int iPos = 0;
	memset(m_ucFrameData, SndFrameBufInit, SndFrameSize);
	m_ucFrameData[0] = FrameHeadCheck0;
	m_ucFrameData[1] = FrameHeadCheck1;
	m_ucFrameData[2] = FrameHeadCheck2;
	m_ucFrameData[3] = FrameHeadCheck3;
	memset(&m_ucFrameData[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	uiIPSource	=	inet_addr(m_csIPSource);
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
	if ((uiIPAim == BroadCastPort) || (uiIPAim == IPSetAddrStart))
	{
		//广播命令
		ucCommand = CmdBroadCastPortSet;
		memcpy(&m_ucFrameData[iPos], &ucCommand, FrameCmdSize1B);
		iPos += FrameCmdSize1B;
		//广播命令端口
		uiADCBroadcastPort = ADCSetBroadcastPort;
		memcpy(&m_ucFrameData[iPos], &uiADCBroadcastPort, FramePacketSize4B);
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
// Parameter: byte * cADCSet
// Parameter: unsigned int uiLength
//************************************
int CADCSet::ADCCommand_18(int iPos, byte * cADCSet, unsigned int uiLength)
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
	uiIPSource = inet_addr(m_csIPSource);
	m_ucFrameData[iPos] = CmdADCDataReturnAddr;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;

	// ad返回端口：返回固定端口则ad_cmd(7) = 0,
	// 例如0x00035005,其中03为ADC数据返回命令，5005为返回端口号
	// 返回端口递增则ad_cmd(7) = 1,例如0x80035005
	uiReturnPort = (SendADCRetransmissionCmd << 16) + ADRecPort;	// 0x00038300
	m_ucFrameData[iPos] = CmdADCDataReturnPort;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &uiReturnPort, FramePacketSize4B);
	iPos += FramePacketSize4B;

	//端口递增上下限命令
	uiReturnPortMove = (ADRecPort << 16) + ADRecPort;
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
void CADCSet::OnSetTB(int iPos, unsigned int tnow, bool bSwitch)
{
	unsigned int tbh = 0;
	unsigned int tbl = 0;
	
	if (bSwitch == false)
	{
		tbh = tnow + TBSleepTimeHigh;
		tbl = TBSleepTimeLow;
	}
	else
	{
		tbh = 0;
		tbl = 0;
	}
	TRACE(_T("TB高位%d\r\n"), tbh);
	
	//写TB时刻高位
	m_ucFrameData[iPos] = CmdTBHigh;
	iPos += FrameCmdSize1B;
	memcpy(&m_ucFrameData[iPos], &tbh, FramePacketSize4B);
	iPos += FramePacketSize4B;
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
	byte mode = 1,sync = 0,phs = 0,chop = 1;
//	byte cOnADCZeroDriftSetFromIP[16] = {0x8d, 0x40, 0x0a, 0x00, 0x52, 0x08, 0x32, 0x03, 0x6f, 0x0c, 0xff, 0x7d, 0x52, 0x40, 0x00, 0x00};
	// 不设置零漂校正值
	// 新版硬件程序
	/*byte cOnADCZeroDriftSetFromIP[8] = {0x87, 0x40, 0x04, 0x00, 0x52, 0x08, 0x32, 0x03};*/
	// 旧版硬件程序
	byte cOnADCZeroDriftSetFromIP[8] = SetADCZeroDriftSetFromIP;
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
		break;
	case 1:
		sps0 = 1;
		sps1 = 1;
		sps2 = 0;
		break;
	case 2:
		sps0 = 0;
		sps1 = 1;
		sps2 = 0;
		break;
	case 3:
		sps0 = 1;
		sps1 = 0;
		sps2 = 0;
		break;
	case 4:
		sps0 = 0;
		sps1 = 0;
		sps2 = 0;
		break;
	default:
		break;
	}
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
	m_pTabADCSettings->GetDlgItem(IDC_EDIT_HPFLOW)->GetWindowText(str);
	m_pTabADCSettings->m_ucHpfLow = atoi(str);
	m_pTabADCSettings->GetDlgItem(IDC_EDIT_HPFHIGH)->GetWindowText(str);
	m_pTabADCSettings->m_ucHpfHigh = atoi(str);
	cOnADCZeroDriftSetFromIP[4] = (8*sync+4*mode+2*sps2+1*sps1)*16 + (8*sps0+4*phs+2*filtr1+1*filtr0);
	cOnADCZeroDriftSetFromIP[5] = (4*mux2+2*mux1+1*mux0)*16 + (8*chop+4*pga2+2*pga1+1*pga0);
	cOnADCZeroDriftSetFromIP[6] = m_pTabADCSettings->m_ucHpfLow;
	cOnADCZeroDriftSetFromIP[7] = m_pTabADCSettings->m_ucHpfHigh;
// 	
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
	iPos = ADCSetFrameHead(BroadCastPort, SendQueryCmd, QueryErrorCodePort);
	OnQueryErrorCodeFdu(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	// 按IP地址查询交叉站
	iPos = ADCSetFrameHead(IPSetAddrStart, SendQueryCmd, QueryErrorCodePort);
	OnQueryErrorCodeLAUX(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
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
	int iPos = 0;
	iPos = ADCSetFrameHead(BroadCastPort, SendSetCmd, ADSetReturnPort);
	OnSetSine(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	OnOpenPowerTBLow(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	OnOpenPowerTBHigh(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	
	Sleep(ADCOperationSleepTime);

	OnOpenSwitchTBLow(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

 	OnOpenSwitchTBHigh(iPos);
 	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
 	Sleep(ADCOperationSleepTime);

 	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);
	
 	OnADCRegisterWrite(iPos, false);
 	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
 	Sleep(ADCOperationSleepTime);
 
 	OnADCRegisterRead(iPos);
 	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
 	Sleep(ADCOperationSleepTime);

	OnADCSetReturn(iPos);
 	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
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
	int iPos = 0;
	iPos = ADCSetFrameHead(BroadCastPort, SendSetCmd, ADSetReturnPort);

	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
	Sleep(ADCOperationSleepTime);
 
	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
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
	int iPos = 0;
	iPos = ADCSetFrameHead(BroadCastPort, SendSetCmd, ADSetReturnPort);
	OnOpenPowerZeroDrift(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 	
	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	
	Sleep(ADCOperationSleepTime);
	
	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
	Sleep(ADCOperationSleepTime);
 	
	// 新版硬件程序
	/*	m_oADCSet.OnADCRegisterWrite(iPos, true);*/
	// 旧版硬件程序
	OnADCRegisterWrite(iPos, false);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	 
	OnADCRegisterRead(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	
	Sleep(ADCOperationSleepTime);
	iPos = ADCSetFrameHead(BroadCastPort, SendQueryCmd, ADSetReturnPort);
	OnADCRegisterQuery(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	 
	Sleep(ADCOperationSleepTime);
	iPos = ADCSetFrameHead(BroadCastPort, SendSetCmd, ADSetReturnPort);
	OnStopSample(iPos);
 	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
	Sleep(ADCOperationSleepTime);
 
	OnADCSampleSynchronization(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
	Sleep(ADCOperationSleepTime);
	
	OnADCReadContinuous(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 	
	Sleep(ADCOperationSleepTime);
	
	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	
	Sleep(ADCOperationSleepTime);

	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);
 
	OnADCZeroDriftCorrection(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);
 
	OnADCReadContinuous(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
 
	Sleep(ADCOperationSleepTime);
 
	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);
 
	OnADCRegisterRead(iPos);
 	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

 	Sleep(ADCOperationSleepTime);
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
	CInstrument* pInstrument = NULL;	// 仪器对象指针
	POSITION pos = NULL;				// 位置	
	unsigned int uiKey = 0;					// 索引键	
	int iPos = 0;
	iPos = ADCSetFrameHead(BroadCastPort, SendSetCmd, ADSetReturnPort);

	OnSetTBSwitchOpen(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	Sleep(ADCOperationSleepTime);

	OnStopSample(iPos);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	OnSetTB(iPos, 0, true);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
	Sleep(ADCOperationSleepTime);


	// 按照IP地址发送零漂矫正帧写寄存器
	unsigned int icount = m_pInstrumentList->m_oInstrumentMap.GetCount();
	if (icount == 0)
	{
		return;
	}
	pos = m_pInstrumentList->m_oInstrumentMap.GetStartPosition();	// 得到索引表起始位置
	while(NULL != pos)
	{
		ProcessMessages();
		pInstrument = NULL;		
		m_pInstrumentList->m_oInstrumentMap.GetNextAssoc(pos, uiKey, pInstrument);	// 得到仪器对象
		// @@@@@@@需要改进
		if(NULL != pInstrument)	
		{
			if (pInstrument->m_bIPSetOK == true)
			{
				for (int i=0; i<InstrumentNum; i++)
				{
					ProcessMessages();
					if (pInstrument->m_uiIPAddress == IPSetAddrStart + (i + 1) * IPSetAddrInterval)
					{
						iPos = ADCSetFrameHead(pInstrument->m_uiIPAddress, SendSetCmd, ADSetReturnPort);
						OnADCZeroDriftSetFromIP(iPos, m_ucZeroDrift[i]);
						SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
						TRACE1("向寄存器写入零漂矫正-仪器IP地址：%d\r\n", pInstrument->m_uiIPAddress);
						break;
					}
				}
			}
		}
	}
	//  	m_pADCSet->OnADCRegisterWrite();
	//  	SendTo(m_pADCSet->m_cFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);


	for (int i=0; i<InstrumentNum; i++)
	{
		if (m_pSelectObject[i] == 1)
		{
			unsigned int uiIPAim = 0;
			uiIPAim	= IPSetAddrStart + IPSetAddrInterval * (i + 1);
			iPos = ADCSetFrameHead(uiIPAim, SendSetCmd, ADSetReturnPort);
			OnADCReadContinuous(iPos);
			SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
			TRACE(_T("ADC连续读命令IP%d\r\n"), uiIPAim);
		}
	}
	// 	m_pADCSet->ADCSetFrameHead(uiIPAim, true, SendSetCmd, ADSetReturnPort);
	// 	m_pADCSet->OnADCReadContinuous(true);
	// 	SendTo(m_pADCSet->m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	iPos = ADCSetFrameHead(BroadCastPort, SendSetCmd, ADSetReturnPort);
	OnSetTB(iPos, tnow, false);
	SendTo(m_ucFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
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