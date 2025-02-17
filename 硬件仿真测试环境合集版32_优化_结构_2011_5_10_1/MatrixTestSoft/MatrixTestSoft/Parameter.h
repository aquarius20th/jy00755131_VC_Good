
//__________________通讯端口设置__________________
// 心跳帧发送创建端口
#define HeartBeatRec					0x7000
// 首包接收端口
#define HeadFramePort				0x8000
// 用于接收IP地址设置应答端口
#define IPSetPort						0x9100
// 尾包接收端口
#define TailFramePort				0x9000
// 用于接收仪器尾包时刻查询的端口号
#define TailTimeFramePort		0x9200
// 用于发送仪器时延设置的端口号
#define TimeSetPort					0x9300
// 数据采样设置应答帧端口
#define ADSetReturnPort			0x8204
// AD数据采集接收端口
#define ADRecPort						0x8300
// 采集站本地时刻查询端口
#define CollectSysTimePort		0x8205
// 状态查询帧应答端口
#define GetStatusPort					0x8206
// 查询硬件设备出错计数返回端口
#define QueryErrorCodePort		0x8207 
// ADC设置广播端口
#define ADCSetBroadcastPort	0x66666666
// 广播端口
#define BroadCastPort				0xffffffff

//___________________缓冲区设置____________________
// 最多连接的设备数
#define InstrumentMaxCount			49	
// 绘图控件个数
#define InstrumentNum					48
// 接收数据包的大小
#define ReceiveDataSize				74
// 缓冲区个数
#define RcvBufNum						8
// 串口转端口中端口缓冲区大小
#define UartToUdpRcvSize			1024
// ADC数据缓冲区大小
#define ADCBufSize						655360
// 端口监视缓冲区大小
#define PortMonitoringBufSize		655360
// 串口转UDP端口缓冲区大小
#define UartToUDPBufSize			655360
// UDP通讯ADC采样接收缓冲区大小
#define ADCDataBufSize				655360
// 心跳发送缓冲区大小
#define HeartBeatSndBufSize		1280
// 接收的网络数据包大小
#define RcvFrameSize					256
// 发送的网络数据包大小
#define SndFrameSize						128
// 发送帧缓冲区初值设定
#define SndFrameBufInit				0x00
// ADC处理过程中预设的缓冲区大小
#define ADCDataTempDataSize	2960
//__________________串口端口定义__________________
#define COMNum	10

//_________________串口波特率定义_________________
#define BAUD0		300
#define BAUD1		600
#define BAUD2		1200
#define BAUD3		2400
#define BAUD4		4800
#define BAUD5		9600
#define BAUD6		19200
#define BAUD7		38400
#define BAUD8		43000
#define BAUD9		56000
#define BAUD10	57600
#define BAUD11	115200

//_____________________常量设置____________________
// 首包计数
#define HeadFrameCount						4
// IP地址重设次数
#define IPAddrResetTimes						3
// 尾包计数
#define TailFrameCount							15
// 仪器类型-采集站
#define InstrumentTypeFDU					3
// 仪器类型-交叉站
#define InstrumentTypeLAUX				1
// ADC数据文件文件头行数
#define ADCFileHeadRow						3
// ADC数据小数点后位数
#define DecimalPlaces							9
// ADC数据帧重发次数
#define ADCFrameRetransmissionNum	1
// 线程关闭等待时间
#define WaitForThreadCloseTime			550
// IP地址设置的起始地址
#define IPSetAddrStart							71
// IP地址设置的间隔
#define IPSetAddrInterval						10
// ADC数据帧指针翻转周期
#define ADCFramePointLength				2048
// 交叉站上方路由设置
#define RoutIPTop									0x55555555
// 交叉站下方路由设置
#define RoutIPDown								0x66666666
// 交叉站左方路由设置
#define RoutIPLeft									0x77777777
// 交叉站右方路由设置
#define RoutIPRight								0x88888888
// 点击横向滚动条左侧按钮的移动量
#define ScrollBarLineLeftMove				5
// 点击横向滚动条右侧按钮的移动量
#define ScrollBarLineRightMove			5
// 横向滚动条向左翻页的移动量
#define ScrollBarPageLeftMove			30
// 横向滚动条向右翻页的移动量
#define ScrollBarPageRightMove			30
// ADC零漂校正需要写寄存器的位数
#define ADCZeroDriftWritebytesNum	6
// 查询设备误码种类个数
#define QueryErrorCodeNum					4
// ADC操作过程延时
#define ADCOperationSleepTime			100
// 端口监视发送线程Sleep时间
#define PortMonitoringSendThreadSleepTime	1
// 端口监视接收线程Sleep时间
#define PortMonitoringRecThreadSleepTime	1
// 心跳线程Sleep时间
#define HeartBeatThreadSleepTime					500
// ADC数据接收线程Sleep时间
#define ADCDataRecThreadSleepTime				1
// TB设置延时高位
#define TBSleepTimeHigh									0x5000
// TB设置延时低位
#define TBSleepTimeLow									0xfa
// ADC数据显示界面每行显示的个数
#define ADCDataShowPerLineNum					6
// IP广播地址
#define IPBroadcastAddr		_T("255.255.255.255")
// 默认的本机IP地址
#define IPHostAddr				_T("127.0.0.1")
// XML配置文件名
#define XMLFileName			_T("MatrixTestSoft.xml")

//____________________命令字设置__________________
// 发送设置命令
#define SendSetCmd								0x0001
// 发送查询命令
#define SendQueryCmd							0x0002
// 发送ADC采样数据重发命令
#define SendADCRetransmissionCmd	0x0003
// 串号
#define CmdSn										0x01
// 首包时间
#define CmdHeadFrameTime					0x02
// 本地IP地址
#define CmdLocalIPAddr						0x03
// 本地系统时间
#define CmdLocalSysTime						0x04
// 本地时间修正高位
#define CmdLocalFixedTimeHigh			0x05
// 本地时间修正低位
#define CmdLocalFixedTimeLow			0x06
// 自动数据返回地址
#define CmdADCDataReturnAddr			0x07
// 自动数据返回端口和命令
#define CmdADCDataReturnPort			0x08
// 端口递增下限和上限
#define CmdADCDataReturnPortLimit	0x09
// 设置网络等待端口和命令
#define CmdSetBroadCastPort				0x0a
// 系统硬件状态拷贝
#define CmdFDUErrorCode					0x0b
// TB时刻高位
#define CmdTBHigh								0x0c
// TB时刻低位
#define CmdTbLow								0x0d
// 尾包接收\发送时刻低位
#define CmdTailRecSndTimeLow			0x16
// 广播命令等待端口匹配
#define CmdBroadCastPortSet				0x17
// 设置ADC控制命令命令字
#define CmdADCSet								0x18
// 网络时刻
#define CmdNetTime								0x19
// 交叉站大线尾包接收时刻
#define CmdTailRecTimeLAUX			0x1b
// 交叉站故障1
#define CmdLAUXErrorCode1				0x1d
// 交叉站故障2
#define CmdLAUXErrorCode2				0x1e
// 交叉站路由分配
#define CmdLAUXSetRout					0x1f
// 返回路由
#define CmdReturnRout							0x3f
//__________________ADC设置命令__________________
// ADC设置正弦波命令
#define SetADCSetSine							{0xb2, 0x00, 0x80, 0x00}
// ADC设置停止采样命令
#define SetADCStopSample					{0x81, 0x11, 0x00, 0x00}
// ADC设置打开TB电源低位
#define SetADCOpenTBPowerLow		{0xa3, 0x00, 0x30, 0x00}
// ADC设置打开TB电源高位
#define SetADCOpenTBPowerHigh		{0xa3, 0x00, 0x30, 0x40}
// ADC设置打开TB开关低位
#define SetADCOpenSwitchTBLow		{0xa3, 0x00, 0xf0, 0x00}
// ADC设置打开TB开关高位
#define SetADCOpenSwitchTBHigh		{0xa3, 0x00, 0xf0, 0x40}
// ADC设置读寄存器
#define SetADCRegisterRead					{0x82, 0x20, 0x0a, 0x00}
// ADC设置写寄存器
#define SetADCRegisterWrite				{0x8d, 0x40, 0x0a, 0x00, 0x52, 0x08, 0x32, 0x03, 0x6f, 0x0c, 0xff, 0x7d, 0x52, 0x40, 0x00, 0x00}
// ADC设置连续采样
#define SetADCReadContinuous			{0x81, 0x10, 0x00, 0x00}
// ADC设置打开零漂校正电源
#define SetADCOpenPowerZeroDrift	{0xa3, 0x05, 0xf8, 0x00}
// ADC设置查询ADC寄存器
#define SetADCRegisterQuery				{0x00, 0x00, 0x00, 0x00}
// ADC设置数据采集同步
#define SetADCSampleSynchronization	{0x81, 0x04, 0x00, 0x00}
// ADC设置零漂偏移矫正
#define SetADCZeroDriftCorrection		{0x81, 0x60, 0x00, 0x00}
// ADC设置打开TB开关
#define SetADCTBSwitchOpen				{0xa3, 0x05, 0xf8, 0x40}
// ADC设置零漂
#define SetADCZeroDriftSetFromIP		{0x8d, 0x40, 0x04, 0x00, 0x52, 0x08, 0x32, 0x03}

//___________________帧格式设置___________________
// 帧头长度
#define FrameHeadSize				16
// 帧头校验字长度
#define FrameHeadCheckSize	4
// 帧头第一个字节
#define FrameHeadCheck0		0x11
// 帧头第二个字节
#define FrameHeadCheck1		0x22
// 帧头第三个字节
#define FrameHeadCheck2		0x33
// 帧头第四个字节
#define FrameHeadCheck3		0x44
// 命令字长度1字节
#define FrameCmdSize1B			1
// 命令包长度2字节
#define FramePacketSize2B		2
// 命令包长度4字节
#define FramePacketSize4B		4
// ADC数据所占字节数
#define ADCDataSize3B			3

// CRC校验值字节数
#define CRCSize			2
// CRC校验预留字节数
#define CRCCheckSize	4
//__________________绘图参数设置___________________
// 设置Y轴下限
#define DrawGraphYAxisLower							-2.5
// 设置Y轴上限
#define DrawGraphYAxisUpper								2.5
// 设置Y轴坐标轴标签
#define DrawGraphYAxisLabel								_T("Y")
// 设置X轴坐标轴标签
#define DrawGraphXAxisLabel								_T("X")
// 设置背景颜色
#define DrawGraphSetBackgroundColor				RGB(0, 0, 64)
// 设置栅格颜色
#define DrawGraphSetGridColor							RGB(192, 192, 255)
// 设置绘点颜色
#define DrawGraphSetPlotColor							RGB(255, 255, 255)
//___________________定时器设置____________________
// TabUartToUdp界面串口接收定时器序号
#define TabUartToUdpRecTimerNb						1
// TabUartToUdp界面串口接收定时器延时设置
#define TabUartToUdpRecTimerSet						100
// TabSample界面开始采样定时器序号
#define TabSampleStartSampleTimerNb				1
// TabSample界面开始采样定时器延时设置
#define TabSampleStartSampleTimerSet				4000
// TabSample界面停止采样定时器序号
#define TabSampleStopSampleTimerNb				2
// TabSample界面停止采样定时器延时设置
#define TabSampleStopSampleTimerSet				1000
// TabSample界面查询硬件设备出错计数定时器序号
#define TabSampleQueryErrorCountTimerNb		3
// TabSample界面查询硬件设备出错计数定时器延时设置
#define TabSampleQueryErrorCountTimerSet		1000
// TabSample界面尾包监测定时器序号
#define TabSampleTailMonitorTimerNb				4
// TabSample界面尾包监测定时器延时设置
#define TabSampleTailMonitorTimerSet				1000
// TabSample界面IP地址设置监测定时器序号
#define TabSampleIPSetMonitorTimerNb				5
// TabSample界面IP地址设置监测定时器延时设置
#define TabSampleIPSetMonitorTimerSet				5000
// TabSample界面初始化仪器本地时间的定时器序号
#define TabSampleInitSysTimeTimerNb				6
// TabSample界面初始化仪器本地时间的定时器延时设置
#define TabSampleInitSysTimeTimerSet				1000
// TabSample界面做时统设置的定时器序号
#define TabSampleTimeCalTimerNb						7
// TabSample界面做时统设置的定时器延时设置
#define TabSampleTimeCalTimerSet						2000
// TabSample界面做ADC参数设置的定时器序号
#define TabSampleADCSetTimerNb						8
// TabSample界面做ADC参数设置的定时器延时设置
#define TabSampleADCSetTimerSet						2500
// TabSample界面做ADC零漂校正的定时器序号
#define TabSampleADCZeroDriftTimerNb			9
// TabSample界面做ADC零漂校正的定时器延时设置
#define TabSampleADCZeroDriftTimerSet			2000
// TabPortMonitoring界面刷新接收发送帧数的定时器序号
#define TabPortMonitoringFrameNumTimerNb	1
// TabPortMonitoring界面刷新接收发送帧数的定时器延时设置
#define TabPortMonitoringFrameNumTimerSet	1000
// TabADCDataShow界面刷新接收的ADC数值的定时器序号
#define TabADCDataShowADCTimerNb				1
// TabADCDataShow界面刷新接收的ADC数值的定时器延时设置
#define TabADCDataShowADCTimerSet				3000
// PortMonitoringRec界面刷新接收帧的定时器序号
#define PortMonitoringRecTimerNb						1
// PortMonitoringRec界面刷新接收帧的定时器延时设置
#define PortMonitoringRecTimerSet						1000
// PortMonitoringSend界面刷新接收帧的定时器序号
#define PortMonitoringSendTimerNb						1
// PortMonitoringSend界面刷新接收帧的定时器延时设置
#define PortMonitoringSendTimerSet					1000

//__________________全局变量设置___________________
// 发送尾包时刻查询帧计数
extern unsigned int m_uiSendTailTimeFrameCount;
// 接收到的尾包时刻查询帧计数
extern unsigned int m_uiRecTailTimeFrameCount;
// CRC16校验
extern unsigned short get_crc_16(unsigned char* buf, int buf_size);
