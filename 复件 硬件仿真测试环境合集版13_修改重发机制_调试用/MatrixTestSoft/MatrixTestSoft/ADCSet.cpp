#include "StdAfx.h"
#include "ADCSet.h"
#include "Parameter.h"
#include "resource.h"
CADCSet::CADCSet(void)
: m_pWnd(NULL)
{
}

CADCSet::~CADCSet(void)
{
}

// ADC设置帧头
void CADCSet::ADCSetFrameHead(unsigned int uiIPAim, bool bBroadCast, unsigned short	usCommand, unsigned short usPortAim)
{
	unsigned int	uiIPSource	=	0;
	unsigned int	itmp		=	0;
	unsigned char	ucCommand;
	unsigned int	uiADCBroadcastPort;
	m_cFrameData[0] = 0x11;
	m_cFrameData[1] = 0x22;
	m_cFrameData[2] = 0x33;
	m_cFrameData[3] = 0x44;
	m_cFrameData[4] = 0x00;
	m_cFrameData[5] = 0x00;
	m_cFrameData[6] = 0x00;
	m_cFrameData[7] = 0x00;
	m_cFrameData[8] = 0x00;
	m_cFrameData[9] = 0x00;
	m_cFrameData[10] = 0x00;
	m_cFrameData[11] = 0x00;
	m_cFrameData[12] = 0x00;
	m_cFrameData[13] = 0x00;
	m_cFrameData[14] = 0x00;
	m_cFrameData[15] = 0x00;

	// 得到本机IP地址
	char		name[255]; 
	CString		ip; 
	PHOSTENT	hostinfo; 
	if(   gethostname   (name, sizeof(name)) == 0) 
	{ 
		if((hostinfo = gethostbyname(name)) != NULL) 
		{ 
			ip = inet_ntoa(*(struct in_addr*)*hostinfo-> h_addr_list); 
		} 
	} 

	uiIPSource	=	inet_addr(ip);
	// 源IP地址
	memcpy(&m_cFrameData[16], &uiIPSource, 4);
	// 广播
	/*uiIPAim = 0xffffffff;*/
	// 目标IP地址
	memcpy(&m_cFrameData[20], &uiIPAim, 4);
	// 目标端口号
	memcpy(&m_cFrameData[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_cFrameData[26], &usCommand, 2);

	memcpy(&m_cFrameData[28], &itmp, 4);

	if (bBroadCast == true)
	{
		//广播命令
		ucCommand = 0x17;
		memcpy(&m_cFrameData[32],&ucCommand,1);
		//广播命令端口
		uiADCBroadcastPort = ADCSetBroadcastPort;
		memcpy(&m_cFrameData[33],&uiADCBroadcastPort,4);
	}
}

// 发送ADC设置的18指令
int CADCSet::ADCCommand_18(int iPos, byte * cADCSet, unsigned int uiLength)
{
	for(unsigned int i=0; i<uiLength; i+=4)
	{
		m_cFrameData[iPos] = 0x18;
		iPos++;
		memcpy(&m_cFrameData[iPos], cADCSet+i, 4);
		iPos = iPos + 4;
	}
	return iPos;
}

// 设置正弦输出
void CADCSet::OnSetSine(void)
{
	int iPos = 37;
	byte cSetSine[4] = {0xb2, 0x00, 0x80, 0x00};
	iPos = ADCCommand_18(iPos,cSetSine,4);
	m_cFrameData[iPos] = 0x00;
}

// 设置停止采集
void CADCSet::OnStopSample(void)
{
	int iPos = 37;
	byte cStopSample[4]={0x81, 0x11, 0x00, 0x00};
	iPos = ADCCommand_18(iPos, cStopSample, 4);
	m_cFrameData[iPos] = 0x00;
}

// 打开AD、DA电源（tb位为0）
void CADCSet::OnOpenPowerTBLow(void)
{
	int iPos = 37;
	byte cOnOpenPowerTBLow[4]={0xa3, 0x00, 0x30, 0x00};
	iPos = ADCCommand_18(iPos, cOnOpenPowerTBLow, 4);
	m_cFrameData[iPos] = 0x00;
}

// 打开AD、DA电源（tb位为1）
void CADCSet::OnOpenPowerTBHigh(void)
{
	int iPos = 37;
	byte cOnOpenPowerTBHigh[4]={0xa3, 0x00, 0x30, 0x40};
	iPos = ADCCommand_18(iPos, cOnOpenPowerTBHigh, 4);
	m_cFrameData[iPos] = 0x00;
}

// 打开AD、DA电源及ADC_RESET、ADC_PWR(tb位为0) 
void CADCSet::OnOpenSwitchTBLow(void)
{
	int iPos = 37;
	byte cOnOpenSwitchTBLow[4]={0xa3, 0x00, 0xf0, 0x00};
	iPos = ADCCommand_18(iPos, cOnOpenSwitchTBLow, 4);
	m_cFrameData[iPos] = 0x00;
}

// 打开AD、DA电源及ADC_RESET、ADC_PWR(tb位为1) 
void CADCSet::OnOpenSwitchTBHigh(void)
{
	int iPos = 37;
	byte cOnOpenSwitchTBHigh[4]={0xa3, 0x00, 0xf0, 0x40};
	iPos = ADCCommand_18(iPos, cOnOpenSwitchTBHigh, 4);
	m_cFrameData[iPos] = 0x00;
}

// 读ADC寄存器
void CADCSet::OnADCRegisterRead(void)
{
	int iPos = 37;
	byte cADCRegisterRead[4]={0x82, 0x20, 0x0a, 0x00};
	iPos = ADCCommand_18(iPos, cADCRegisterRead, 4);
	m_cFrameData[iPos] = 0x00;
}

// 写ADC寄存器
void CADCSet::OnADCRegisterWrite(void)
{
	int iPos = 37;
	byte cADCRegisterWrite[16]={0x8d, 0x40, 0x0a, 0x00, 0x52, 0x08, 0x32, 0x03, 0x6f, 0x0c, 0xff, 0x7d, 0x52, 0x40, 0x00, 0x00};
	iPos = ADCCommand_18(iPos, cADCRegisterWrite, 16);
	m_cFrameData[iPos] = 0x00;
}

// ADC设置参数返回
void CADCSet::OnADCSetReturn(void)
{
	int iPos = 37;

	//自动AD返回地址（主机127.0.0.1）
	unsigned int IP= 0x0100007f;
	m_cFrameData[iPos]= 0x07;
	iPos++;
	memmove(m_cFrameData + iPos, &IP, 4);
	iPos = iPos + 4;

	//ad返回端口固定 80035005 ad返回端口递增
	unsigned int Port= 0x00038300;
	m_cFrameData[iPos]= 0x08;
	iPos++;
	memmove(m_cFrameData + iPos, &Port, 4);
	iPos = iPos + 4;

	//端口递增上下限命令
	unsigned int PortMove = 0x83008300;
	m_cFrameData[iPos]= 0x09;
	iPos++;
	memmove(m_cFrameData + iPos, &PortMove, 4);
	iPos = iPos + 4;

	//0-结束发送数据
	m_cFrameData[iPos]= 0;
}

// ADC连续读取数据
void CADCSet::OnADCReadContinuous(bool bBroadCast)
{
	int iPos = 0;
	if (bBroadCast == true)
	{
		iPos = 37;
	}
	else
	{
		iPos = 32;
	}
	byte cADCRegisterWrite[4]={0x81, 0x10, 0x00, 0x00};
	iPos = ADCCommand_18(iPos, cADCRegisterWrite, 4);
	m_cFrameData[iPos] = 0x00;
}

void CADCSet::OnSetTB(unsigned int tnow)
{
	int iPos = 37;
	unsigned int tbh;
	unsigned int tbl;
	unsigned int uitmp = 0x00000000;
	
	tbh= tnow + 0x5000;
	tbl= 250;
	
	//写TB时刻高位
	m_cFrameData[iPos] = 0x0c;
	iPos++;
	memmove(m_cFrameData + iPos, &tbh, 4);
	iPos = iPos + 4;
	//写TB时刻低位
	m_cFrameData[iPos] = 0x0d;
	iPos++;
	memmove(m_cFrameData + iPos, &tbl, 2);
	iPos=iPos+2;

	// 0-结束发送数据
	memmove(m_cFrameData + iPos, &uitmp, 4);
}

// 打开零漂矫正电源及开关
void CADCSet::OnOpenPowerZeroDrift(void)
{
	int iPos = 37;
	byte cOnOpenPowerZeroDrift[4]={0xa3, 0x05, 0xf8, 0x00};
	iPos = ADCCommand_18(iPos, cOnOpenPowerZeroDrift, 4);
	m_cFrameData[iPos] = 0x00;
}

// 查询ADC寄存器
void CADCSet::OnADCRegisterQuery(void)
{
	int iPos = 37;
	byte cOnADCRegisterQuery[4]={0x00, 0x00, 0x00, 0x00};
	iPos = ADCCommand_18(iPos, cOnADCRegisterQuery, 4);
	m_cFrameData[iPos] = 0x00;
}

// ADC数据采集同步
void CADCSet::OnADCSampleSynchronization(void)
{
	int iPos = 37;
	byte cOnADCSampleSynchronization[4]={0x81, 0x04, 0x00, 0x00};
	iPos = ADCCommand_18(iPos, cOnADCSampleSynchronization, 4);
	m_cFrameData[iPos] = 0x00;
}

// ADC零漂偏移矫正
void CADCSet::OnADCZeroDriftCorrection(void)
{
	int iPos = 37;
	byte cOnADCZeroDriftCorrection[4]={0x81, 0x60, 0x00, 0x00};
	iPos = ADCCommand_18(iPos, cOnADCZeroDriftCorrection, 4);
	m_cFrameData[iPos] = 0x00;	
}
// ADC零漂设置（按IP地址）
void CADCSet::OnADCZeroDriftSetFromIP(unsigned char* ucZeroDrift)
{
	int iPos = 32;
	CString str = _T("");
	int iSelect = 0;
	byte hpf0 = 50,hpf1 = 3,pga0 = 0,pga1 = 0,pga2 = 0;
	//sps1=1 1000k sps1=0 250k
	byte mux0 = 0,mux1 = 0,mux2 = 0,sps0 = 0,sps1 = 1,sps2 = 0,filtr0 = 0,filtr1 = 1;
	byte mode = 1,sync = 0,phs = 0,chop = 1;
	byte cOnADCZeroDriftSetFromIP[16]={0x8d, 0x40, 0x0a, 0x00, 0x52, 0x08, 0x32, 0x03, 0x6f, 0x0c, 0xff, 0x7d, 0x52, 0x40, 0x00, 0x00};

	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_SYNC))->GetCurSel();
	switch(iSelect)
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
	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_MODE))->GetCurSel();
	switch(iSelect)
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
	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_SPS))->GetCurSel();
	switch(iSelect)
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
	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_PHS))->GetCurSel();
	switch(iSelect)
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
	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_FILTER))->GetCurSel();
	switch(iSelect)
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
	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_MUX))->GetCurSel();
	switch(iSelect)
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
	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_CHOP))->GetCurSel();
	switch(iSelect)
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
	iSelect = ((CComboBox*)m_pWnd->GetDlgItem(IDC_COMBO_PGA))->GetCurSel();
	switch(iSelect)
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
	m_pWnd->GetDlgItem(IDC_EDIT_HPFLOW)->GetWindowText(str);
	sscanf_s(str, _T("%c"), &hpf0);
	m_pWnd->GetDlgItem(IDC_EDIT_HPFHIGH)->GetWindowText(str);
	sscanf_s(str, _T("%c"), &hpf1);
	cOnADCZeroDriftSetFromIP[4] = (8*sync+4*mode+2*sps2+1*sps1)*16 + (8*sps0+4*phs+2*filtr1+1*filtr0);
	cOnADCZeroDriftSetFromIP[5] = (4*mux2+2*mux1+1*mux0)*16 + (8*chop+4*pga2+2*pga1+1*pga0);
	cOnADCZeroDriftSetFromIP[6] = hpf0;
	cOnADCZeroDriftSetFromIP[7] = hpf1;
	
// 	cOnADCZeroDriftSetFromIP[8] = ucZeroDrift[0];
// 	cOnADCZeroDriftSetFromIP[9] = ucZeroDrift[1];
// 	cOnADCZeroDriftSetFromIP[10] = ucZeroDrift[2];
// 	cOnADCZeroDriftSetFromIP[11] = ucZeroDrift[3];
//
// 	cOnADCZeroDriftSetFromIP[12] = ucZeroDrift[4];
// 	cOnADCZeroDriftSetFromIP[13] = ucZeroDrift[5];

	iPos = ADCCommand_18(iPos, cOnADCZeroDriftSetFromIP, 16);
	m_cFrameData[iPos] = 0x00;	
}