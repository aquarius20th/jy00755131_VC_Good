/**
文件名：	segd.file.format.h
版本：	1.1.22.45.2011.10.08
参考：	该头文件主要标记了segd2.1格式信息，具体说明可参考
		SEG-DRev2.1 SEG Field Tape Standards Jan，2006

		同时也有部分结构体为sercel 428XL的SEG-D 格式，见用
		户手册（v2.0 第二册）

bug:
	1.1.22.45.2011.10.08	调整程序,加入对于外部头段和跟踪到4-7道头的读取,修改错误有关读取外部头段
	
*/

#ifndef __SEGD_FILE_FORMAT_H__
#define __SEGD_FILE_FORMAT_H__

//#define __SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__
//#define __SEG_D_FILE_FORMAT_REV_2_1_MODE__

enum __enuSEGD_FILE_CREATE_TYPE
{	
	SEGD_FILE_NULL			=	0x0,
	SEGD_FILE_CREATE		=	0x1,
	SEGD_FILE_OPEN_EXISTING	=	0x2,
	SEGD_FILE_CREATE_MASK	=	0xf,
};

/**
@enum	__enuSEGD_FILE_IO_TYPE
@detail 文件IO读取和写入方式 
*/
enum __enuSEGD_FILE_IO_TYPE
{	
	SEGD_FILE_IO_NON_OVERLAPPED				=	0x10, //!< 使用简单文件操作方式
	SEGD_FILE_IO_OVERLAPPED					=	0x20, //!< 使用文件overlapped io操作方式
	SEGD_FILE_IO_OVERLAPPED_NO_BUFFERING	=	0x30, //!< 使用文件overlapped io + FILE_FLAG_NO_BUFFERING操作方式
	SEGD_FILE_IO_MASK						=	0xf0, //!< 掩码
};

enum __enuSEGD_FILE_MOVE_METHOD
{	
	SEGD_FILE_BEGIN,
	SEGD_FILE_CURRENT,
	SEGD_FILE_END,
};

typedef unsigned long	SEGD_ULONG;
typedef unsigned int	SEGD_UINT;
typedef unsigned char	SEGD_UCHAR;
typedef unsigned short	SEGD_USHORT;
typedef	double			SEGD_DOUBLE;
typedef	float			SEGD_FLOAT;
typedef ULONGLONG		SEGD_ULONGLONG;

typedef struct __tagSEGD_FILE_STORAGE_UNIT_LABEL
{
	SEGD_UCHAR	storage_unit_sequence_number_[4];
	SEGD_UCHAR	segd_revision_[5];
	SEGD_UCHAR	storage_unit_structure_[6];
	SEGD_UCHAR	binding_edition_[4];
	SEGD_UCHAR	maximum_block_size_[10];
	SEGD_UCHAR	aip_producer_organization_code_[10];
	SEGD_UCHAR	createion_date_[11];
	SEGD_UCHAR	serial_number_[12];
	SEGD_UCHAR	reserved_[6];
	SEGD_UCHAR	storage_set_identifier_[60];
	SEGD_UCHAR	external_label_name_[12];
	SEGD_UCHAR	recording_entity_name_[24];
	SEGD_UCHAR	user_defined_[14];
	SEGD_UCHAR	max_shot_records_per_field_record_[10];
}SEGD_FILE_STORAGE_UNIT_LABEL, *SEGD_FILE_STORAGE_UNIT_LABEL_PTR;

typedef struct __tagSEGD_GENERAL_HEADER_1_PRIVATE
{
	SEGD_USHORT	file_number_;
	SEGD_USHORT format_code_;
	SEGD_UCHAR	general_constants_[6];
	SEGD_UCHAR	year_;
	SEGD_UCHAR	additional_blocks_in_general_header_; //high four bit
	SEGD_UCHAR	day_; // additional_blk low four bit + day
	SEGD_UCHAR	hour_;
	SEGD_UCHAR	minute_;
	SEGD_UCHAR	second_;
	SEGD_UCHAR	manufacturer_code_;
	SEGD_UCHAR	manufacturer_serial_number_[2];

#if defined(__SEG_D_FILE_FORMAT_REV_2_1_MODE__)
		SEGD_UCHAR	undefined_1_[3];
#elif defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)
		SEGD_UCHAR	bytes_per_scan_[3];
#else //default rev2.1
		SEGD_UCHAR	undefined_1_[3];
#endif 

	SEGD_UCHAR	base_scan_interval_;
	SEGD_UCHAR	polarity_;//high four bit
	SEGD_UCHAR	undefined_2_;
	SEGD_UCHAR	record_type_; // high four bit
	SEGD_UCHAR	record_length_; // record_type_ low 4 bit + record_length_
	SEGD_UCHAR	scan_types_record_;
	SEGD_UCHAR	channal_sets_scan_type_;
	SEGD_UCHAR	skew_blocks_;
	SEGD_UCHAR	extended_header_block_;
	SEGD_UCHAR	external_header_block_;
}SEGD_GENERAL_HEADER_1_PRIVATE, *SEGD_GENERAL_HEADER_1_PRIVATE_PTR;

typedef struct __tagSEGD_GENERAL_HEADER_1
{
	SEGD_USHORT	file_number_;
	SEGD_USHORT format_code_;
	SEGD_UCHAR	general_constants_[6];
	SEGD_UCHAR	year_;
	SEGD_UCHAR	additional_blocks_in_general_header_;
	SEGD_USHORT	day_;
	SEGD_UCHAR	hour_;
	SEGD_UCHAR	minute_;	
	SEGD_UCHAR	second_;
	SEGD_UCHAR	manufacturer_code_;
	SEGD_USHORT	manufacturer_serial_number_;

#if defined(__SEG_D_FILE_FORMAT_REV_2_1_MODE__)
		SEGD_UINT	undefined_1_;
#elif defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)
		SEGD_UINT	bytes_per_scan_;
#else //default rev2.1
		SEGD_UINT	undefined_1_;
#endif 

	SEGD_UCHAR	base_scan_interval_;
	SEGD_UCHAR	polarity_;
	SEGD_UCHAR	undefined_2_;
	SEGD_UCHAR	record_type_;
	SEGD_USHORT	record_length_;
	SEGD_UCHAR	scan_types_record_;
	SEGD_UCHAR	channal_sets_scan_type_;
	SEGD_UCHAR	skew_blocks_;
	SEGD_UCHAR	extended_header_block_;
	SEGD_UCHAR	external_header_block_;
}SEGD_GENERAL_HEADER_1, *SEGD_GENERAL_HEADER_1_PTR;

typedef struct __tagSEGD_GENERAL_HEADER_2_PRIVATE
{
	SEGD_UCHAR	expanded_file_number_[3];
	SEGD_UCHAR	extended_channel_sets_scan_type_[2];
	SEGD_UCHAR	extended_header_blocks_[2];
	SEGD_UCHAR	external_header_blocks_[2];
	SEGD_UCHAR	undefined_1_;
	SEGD_UCHAR	major_segd_revision_number_;
	SEGD_UCHAR	minor_segd_revision_number_;
	SEGD_USHORT	number_of_blocks_of_general_trailer_;
	SEGD_UCHAR	extended_record_length_[3];
	SEGD_UCHAR	undefined_2_;
	SEGD_UCHAR	general_header_block_number_;

#if defined(__SEG_D_FILE_FORMAT_REV_2_1_MODE__)
		SEGD_UCHAR	undefined_3_;
		SEGD_USHORT	sequence_number_;
		SEGD_UCHAR	undefined_4_[10];
#elif defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)
		SEGD_UCHAR	undefined_3_[13];
#else //default rev2.1
		SEGD_UCHAR	undefined_3_;
		SEGD_USHORT	sequence_number_;
		SEGD_UCHAR	undefined_4_[10];	
#endif
}SEGD_GENERAL_HEADER_2_PRIVATE, *SEGD_GENERAL_HEADER_2_PRIVATE_PTR;

typedef struct __tagSEGD_GENERAL_HEADER_2
{
	SEGD_UINT	expanded_file_number_;
	SEGD_USHORT	extended_channel_sets_scan_type_;
	SEGD_USHORT	extended_header_blocks_;
	SEGD_USHORT	external_header_blocks_;
	SEGD_UCHAR	undefined_1_;
	SEGD_UCHAR	major_segd_revision_number_;
	SEGD_UCHAR	minor_segd_revision_number_;
	SEGD_USHORT	number_of_blocks_of_general_trailer_;
	SEGD_UINT	extended_record_length_;
	SEGD_UCHAR	undefined_2_;
	SEGD_UCHAR	general_header_block_number_;

#if defined(__SEG_D_FILE_FORMAT_REV_2_1_MODE__)
		SEGD_UCHAR	undefined_3_;
		SEGD_USHORT	sequence_number_;
		SEGD_UCHAR	undefined_4_[10];
#elif defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)
		SEGD_UCHAR	undefined_3_[13];
#else //default rev2.1
		SEGD_UCHAR	undefined_3_;
		SEGD_USHORT	sequence_number_;
		SEGD_UCHAR	undefined_4_[10];	
#endif

}SEGD_GENERAL_HEADER_2, *SEGD_GENERAL_HEADER_2_PTR;

typedef struct __tagSEGD_GENERAL_HEADER_N_PRIVATE
{
	SEGD_UCHAR	expanded_file_number_[3];
	SEGD_UCHAR	source_line_number_integer_[3];
	SEGD_UCHAR	source_line_number_fraction_[2];
	SEGD_UCHAR	source_point_number_integer_[3];
	SEGD_UCHAR	source_point_number_fraction_[2];
	SEGD_UCHAR	source_point_index_;
	SEGD_UCHAR	phase_control_;
	SEGD_UCHAR	type_vibrator_;
	SEGD_USHORT	phase_angle_;
	SEGD_UCHAR	general_header_block_number_;
}SEGD_GENERAL_HEADER_N_PRIVATE, *SEGD_GENERAL_HEADER_N_PRIVATE_PTR;

typedef struct __tagSEGD_GENERAL_HEADER_N
{
	SEGD_UINT	expanded_file_number_;
	float		source_line_number_;
	float		source_point_number_;
	SEGD_UCHAR	source_point_index_;
	SEGD_UCHAR	phase_control_;
	SEGD_UCHAR	type_vibrator_;
	SEGD_USHORT	phase_angle_;
	SEGD_UCHAR	general_header_block_number_;
	SEGD_UCHAR	source_set_number_;
	SEGD_UCHAR	undefined_4_[12];
}SEGD_GENERAL_HEADER_N, *SEGD_GENERAL_HEADER_N_PTR;

#if defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)
	typedef struct __tagSEGD_GENERAL_HEADER_3_PRIVATE
	{
		SEGD_UCHAR	expanded_file_number_[3];
		SEGD_UCHAR	source_line_number_[5];
		SEGD_UCHAR	source_point_number_[5];
		SEGD_UCHAR	source_point_index_;
		SEGD_UCHAR	phase_control_;
		SEGD_UCHAR	type_vibrator_;
		SEGD_USHORT	phase_angle_;
		SEGD_UCHAR	general_header_block_number_;
		SEGD_UCHAR	source_set_number_;
		SEGD_UCHAR	undefined_[12];
	}SEGD_GENERAL_HEADER_3_PRIVATE, *SEGD_GENERAL_HEADER_3_PRIVATE_PTR;

	typedef struct __tagSEGD_GENERAL_HEADER_3
	{
		SEGD_UINT	expanded_file_number_;
		SEGD_FLOAT	source_line_number_;
		SEGD_FLOAT	source_point_number_;
		SEGD_UCHAR	source_point_index_;
		SEGD_UCHAR	phase_control_;
		SEGD_UCHAR	type_vibrator_;
		SEGD_USHORT	phase_angle_;
		SEGD_UCHAR	general_header_block_number_;
		SEGD_UCHAR	source_set_number_;
		SEGD_UCHAR	undefined_[12];
	}SEGD_GENERAL_HEADER_3, *SEGD_GENERAL_HEADER_3_PTR;
#endif //defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)

typedef struct __tagSEGD_SCAN_TYPE_HEADER_PRIVATE
{
	SEGD_UCHAR	scan_type_number_;
	SEGD_UCHAR	channel_set_number_;
	SEGD_USHORT channel_set_start_time_;
	SEGD_USHORT	channel_set_end_time_;
	SEGD_USHORT	descale_multiplier_;
	SEGD_USHORT	channels_number_;
	SEGD_UCHAR	channel_type_identification_; // high 4 bit
	SEGD_UCHAR	sample_channel_channel_gain_; //high 4 bit: sample-channel_gain , low 4bit is channel_gain
	SEGD_USHORT	alias_filter_frequency_;
	SEGD_USHORT	alias_filter_slope_; 
	SEGD_USHORT	low_cut_filter_;
	SEGD_USHORT	low_cut_filter_slope_;
	SEGD_USHORT	first_notch_filter_;
	SEGD_USHORT second_notch_filter_;
	SEGD_USHORT third_notch_filter_;
	SEGD_USHORT	extended_channel_set_number_;
	SEGD_UCHAR	extended_header_flag_trace_header_extension_;
	SEGD_UCHAR	vertical_stack_;
	SEGD_UCHAR	streamer_number_;
	SEGD_UCHAR	array_forming_;
} SEGD_SCAN_TYPE_HEADER_PRIVATE, *SEGD_SCAN_TYPE_HEADER_PRIVATE_PTR;

typedef struct __tagSEGD_SCAN_TYPE_HEADER
{
	SEGD_UCHAR	scan_type_number_;
	SEGD_UCHAR	channel_set_number_;
	SEGD_USHORT channel_set_start_time_;
	SEGD_USHORT	channel_set_end_time_;
#if defined(__SEG_D_FILE_FORMAT_REV_2_1_MODE__)
	SEGD_UCHAR	optional_byte_;
#endif
	SEGD_USHORT	descale_multiplier_;
	SEGD_USHORT	channels_number_;
	SEGD_UCHAR	channel_type_identification_;
	SEGD_UCHAR	sample_channel_subscans_number_;
	SEGD_UCHAR	channel_gain_control_;
	SEGD_USHORT	alias_filter_frequency_;
	SEGD_USHORT	alias_filter_slope_; 
	SEGD_USHORT	low_cut_filter_;
	SEGD_USHORT	low_cut_filter_slope_;
	SEGD_USHORT	first_notch_filter_;
	SEGD_USHORT second_notch_filter_;
	SEGD_USHORT third_notch_filter_;
	SEGD_USHORT	extended_channel_set_number_;
	SEGD_UCHAR	extended_header_flag_;
	SEGD_UCHAR	trace_header_extension_;
	SEGD_UCHAR	vertical_stack_;
	SEGD_UCHAR	streamer_number_;
	SEGD_UCHAR	array_forming_;
} SEGD_SCAN_TYPE_HEADER, *SEGD_SCAN_TYPE_HEADER_PTR;

#if defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)
	typedef struct __tagSEGD_EXTENDED_HEADER_PRIVATE
	{
		SEGD_UINT	sample_length_;			//采集长度1000 到 128000 毫秒
		SEGD_UINT	sample_frequency_;		//采样率250、500、1000、2000、4000 微秒
		SEGD_UINT	trace_total_number_;	//记录道总数1 到 100000
		SEGD_UINT	auxiliary_trace_number_;//13-16 XXXX bin 辅助道数目1 到 100000
		SEGD_UINT	trace_number_;			//17-20 XXXX bin 地震记录道数目1 到 100000
		SEGD_UINT	stopped_trace_number_;	//21-24 XXXX bin 停滞地震记录道数目1 到 100000
		SEGD_UINT	active_trace_number_;	//25-28 XXXX bin 活动地震记录道数目1 到 100000
		SEGD_UINT	source_type_;			//29-32 XXXX bin 震源类型0 = 无震源 1 = 脉冲 2 = 可控震源
		SEGD_UINT	sample_number_in_trace_;//33-36 XXXX bin 记录道中样本数目1 到 128000
		SEGD_UINT	shot_number_;			//37-40 XXXX bin 炮号1 到 9999
		SEGD_FLOAT	tb_window_;				//41-44 XXXX flt TB 窗口0 到 64 秒
		SEGD_UINT	test_type_;				//45-48 XXXX bin 测试记录类型
											//	0 正常记录。 1 野外噪声。2 野外脉冲。
											//	3 野外串音。4 仪器噪声。	5 仪器畸变。	6 仪器增益/ 相位。7 仪器串音。	8 仪器共模。	9 合成。
											//	10 一致性（仅限 CM408）。11 仪器脉冲。12 野外畸变。13 仪器重力。14 野外漏电。	15 野外电阻。
		
		SEGD_UINT	line_first_test_;		//49-52 XXXX bin 线束第一条测线	1 到 99999
		SEGD_UINT	line_first_number_;		//53-56 XXXX bin 线束第一个号码1 到 99999
		SEGD_UINT	array_number_;			//57-60 XXXX bin 排列编号1 到 32
		SEGD_UINT	array_type_;			//61-64 	XXXX 	bin	排列类型	1 = 普通	 2 = 绝对 	 
		SEGD_UINT	time_break_signal_;		//65-68 	XXXX 	bin	时断信号	0 到 9999 微秒 	 
		SEGD_UINT	well_time_;				//69-72 	XXXX 	bin	井口时间	微秒	 由导航放炮或导航系统更新 	 
		SEGD_UINT	blaster_serial_number_;	//73-76 	XXXX 	bin	爆炸机标识号	- MACHA 爆炸机标识号	（0 到 15）- 或 OPSEIS 爆炸机 815 	 
											//						SAR 地址（1 到 65535）

		SEGD_UINT	blaster_status_;		//77-80 	XXXX 	bin	爆炸机状态	MACHA 爆炸机状态：
											//20电池电量太低。 	 
											//21高电压准备就绪。 	 
											//22已点火。 	 
											//23点火故障。	 
											//OPSEIS 815 爆炸机： 	 
											//20爆炸机做好放炮准备。 	 
											//21爆炸机盖打开。 	 
											//22爆炸机井口故障。	 
											//SHOTPRO 爆炸机状态： 	 
											//0未点火（接收到电台状态，但设备未点火）
											//1接收到已点火放炮和状态。一切正常
											//2未接收到状态（电台故障）
											//3接收到状态，但没有井口模拟数据(电台故障）
											//4解码器电池电量太低警告。
											//5井口检波器电阻未测量或超出允许范围。 	 
											//6雷管阻抗未测量或超出允许范围。
											//7自动井口时间拾取不成功。

		SEGD_UINT	refraction_delay_;		//81-84 	XXXX 	bin	折射延迟	毫秒
		SEGD_UINT	tb_to_t0_time_;			//85-88 	XXXX 	± bin	Tb 至 T0 时间	微秒
		SEGD_UINT	internal_time_break_signal_;	//89-92 	XXXX 	bin	内部时断信号	0 = 是 1 = 否 	 
		SEGD_UINT	field_unit_prestack_;			//93-96 	XXXX 	bin	野外设备中预叠加	0 = 是 1 = 否
		SEGD_UINT	Noise_elimination_type_;		//97-100 	XXXX 	bin	噪声抑制类型	1 无	 2 花样叠加	3 历史	 4 关闭 	 

		SEGD_UINT	low_trace_percent_;				//101-104 	XXXX 	bin	低记录道百分比	0 到 100% 	 
		SEGD_UINT	low_trace_value_;				//105-108 	XXXX 	bin	低记录道值	0 到 132 dB 	 
		SEGD_UINT	window_number_;					//109-112 	XXXX 	bin	窗口数目	1 到 64 	 
		SEGD_UINT	historical_edit_type_;				//113-116 	XXXX 	bin	历史编辑类型	1 = 归零	 2 = 剪裁 	 
		SEGD_UINT	noisy_trace_percent_;				//117-120 	XXXX 	bin	噪声记录道百分比	0 到 100% 	 
		SEGD_UINT	historical_range_;					//121-124 	XXXX 	bin	历史范围	0 到 36 dB 	 
		SEGD_UINT	historical_taper_length_;			//125-128 	XXXX 	bin	历史锥形长度 2 的指数	0 到 8
		SEGD_UINT	threshold_changed_;					//129-132 	XXXX 	bin	门槛值不变 /变化	1 = 不变2 = 变化 
		SEGD_UINT	historical_threshold_initial_value_;//133-136 	XXXX 	bin	历史门槛值初始值	0 到 132 dB
		SEGD_UINT	historical_zeroing_length_;			//137-140 	XXXX 	bin	历史归零长度	1 到 500毫秒 	 
		SEGD_UINT	process_type_;						//141-144 	XXXX 	bin	处理类型
														//1 2	无操作（原始数据）叠加
														//3 	叠加后相关
														//4 	叠加前相关
														//56 7	原始数据，叠加 原始数据，叠加后相关 原始数据，叠加前相关

		SEGD_UINT	sample_type_table_[32];				//145-272 	XXXX 	bin	采集类型表	32 个数值（128 字节） 	 
		SEGD_UINT	threshold_value_table_[32];			//273-400 	XXXX 	bin	门槛值类型表	32 个数值（128 字节） 	 
		SEGD_UINT	overlap_number_;					//401-404 	XXXX 	bin	叠加次数	1 到 32 	 
		SEGD_UCHAR	not_used1_[80];						//405-484 	XXXX 	asc	未使用 		 
		SEGD_UINT	record_length_;						//485-488 	XXXX 	bin	记录长度	100 到 128000 毫秒 	 
		SEGD_UINT	autocorrelative_peak_time_;			//489-492 	XXXX 	bin	自相关峰值时间	1 到 128000 毫秒	 
		SEGD_UINT	not_used2_;							//493-496 	XXXX 	bin	未使用 		 
		SEGD_UINT	related_reference_signal_number_;	//497-500 	XXXX 	bin	相关参考信号编号	1 到 100000 	 
		SEGD_UINT	reference_signal_length_;			//501-504 	XXXX 	bin	参考信号长度	1000 到 128000 毫秒 	 
		SEGD_UINT	scan_length_;						//505-508 	XXXX 	bin	扫描长度	1000 到 128000 毫秒 	 
		SEGD_UINT	sample_number_;						//509-512 	XXXX 	bin	采集号	1 到 32 	 
		SEGD_FLOAT	max_auxiliary_channel_max_value_;	//513-516 	XXXX 	flt	最多辅助道的最大值	IEEE 格式，单精度 	 
		SEGD_FLOAT	max_channel_max_value_;		//517-520 	XXXX 	flt	最多地震道最大值	IEEE 格式，单精度
		SEGD_UINT	dump_overlap_number_;		//521-524 	XXXX 	bin	转储叠加次数 	1..32 	 
		SEGD_UCHAR	tape_lable_[16];			//525-540 	XXXX 	asc	磁带标签	ASCII 文本，16 个字符 	 
		SEGD_UINT	tape_number_;				//541-544 	XXXX 	bin	磁带号	1 到 9999 	 
		SEGD_UCHAR	version_[16];				//545-560 	XXXX 	asc	软件版本	ASCII 文本，16 个字符 	 
		SEGD_UCHAR	date_[12];					//561-572 	XXXX 	asc	日期	ASCII 文本，12 个字符 (dd mmm yyyy) 	 
		SEGD_UCHAR	source_easting_[8];			//573-580 	XXXX 	dbl	震源偏东距	默认为 GUI 设置，或由放炮或导航系统更新 	 
		SEGD_UCHAR	source_northing_[8];			//581-588 	XXXX 	dbl	震源北向纬度差	默认为 GUI 设置，或由放炮或导航系统更新 	 
		SEGD_FLOAT	Source_elevation_;			//589-592 	XXXX 	flt	震源高程	默认为 GUI 设置，或由放炮或导航系统更新 	 
		SEGD_UINT	slip_sweep_mode_;			//593-596 	XXXX 	bin	所用的滑动扫描模式	0 = 否1 = 是 	 
		SEGD_UINT	file_number_per_tape_;		//597-600 	XXXX 	bin	每个磁带的文件数	1 到 9999 	 
		SEGD_UINT	file_counter_;				//601-604 	XXXX 	bin	文件计数	1 到 9999 	 
		SEGD_UCHAR	sample_error_explaination_[160];	//605-764 	XXXX 	asc	采集错误说明	ASCII 文本，160 个字符 	 
		SEGD_UINT	filter_type_;				//765-768 	XXXX 	bin	滤波器类型 	1 2 	= = 	0.8 最小 0.8 线性 	 
		SEGD_UINT	stack_dumped_;				//769-772 	XXXX 	bin	叠加已转储	0 = 否1 = 是 	 
		SEGD_UINT	stack_symbol_;				//773-776 	XXXX 	bin	叠加符号（当前）	0 = 否 1 = 正 2 = 负
		SEGD_UINT	prm_tilt_correction_;		//777-780 	XXXX 	bin	所用的 PRM 倾斜度校正	0 = 否 1 = 是 	 
		SEGD_UCHAR	line_name_[64];				//781-844 	XXXX 	asc	线束名 		 
		SEGD_UINT	operation_mode_;			//845-848 	XXXX 	bin	工作模式	bit0 = 1 正常
												//bit1 = 1 微地震
												//bit2 = 1 滑动扫描 	 
												//bit3 = 1 HFVS	 
												//bit4 = 1 导航 	

		SEGD_UINT	reserved2_;					//849-852 	XXXX 	bin	保留
		SEGD_UINT	non_log_;					//853-856 	XXXX 	bin	无记录	0 = 否	1 = 是 	 
		SEGD_UINT	listen_time_;				//857-860 	XXXX 	bin	监听时间	100 到 128000 毫秒 	 
		SEGD_UINT	dump_type_;					//861-864 	XXXX 	bin	转储类型	0 = 正常转储 1 = 原始转储	 2 = 附加转储 	 
		SEGD_UINT	reserved3_;					//865-868 	XXXX 	bin	保留
		SEGD_UINT	line_id_;					//869-872 	XXXX 	bin	线束 Id 		 
		SEGD_UCHAR	undefined_[152];			//873-1024 	0		未使用

	}SEGD_EXTENDED_HEADER_PRIVATE, *SEGD_EXTENDED_HEADER_PRIVATE_PTR;

	typedef struct __tagSEGD_EXTENDED_HEADER
	{
		SEGD_UINT	sample_length_;			//采集长度1000 到 128000 毫秒
		SEGD_UINT	sample_frequency_;		//采样率250、500、1000、2000、4000 微秒
		SEGD_UINT	trace_total_number_;	//记录道总数1 到 100000
		SEGD_UINT	auxiliary_trace_number_;//13-16 XXXX bin 辅助道数目1 到 100000
		SEGD_UINT	trace_number_;			//17-20 XXXX bin 地震记录道数目1 到 100000
		SEGD_UINT	stopped_trace_number_;	//21-24 XXXX bin 停滞地震记录道数目1 到 100000
		SEGD_UINT	active_trace_number_;	//25-28 XXXX bin 活动地震记录道数目1 到 100000
		SEGD_UINT	source_type_;			//29-32 XXXX bin 震源类型0 = 无震源 1 = 脉冲 2 = 可控震源
		SEGD_UINT	sample_number_in_trace_;//33-36 XXXX bin 记录道中样本数目1 到 128000
		SEGD_UINT	shot_number_;			//37-40 XXXX bin 炮号1 到 9999
		SEGD_FLOAT	tb_window_;				//41-44 XXXX flt TB 窗口0 到 64 秒
		SEGD_UINT	test_type_;				//45-48 XXXX bin 测试记录类型
											//	0 正常记录。 1 野外噪声。2 野外脉冲。
											//	3 野外串音。4 仪器噪声。	5 仪器畸变。	6 仪器增益/ 相位。7 仪器串音。	8 仪器共模。	9 合成。
											//	10 一致性（仅限 CM408）。11 仪器脉冲。12 野外畸变。13 仪器重力。14 野外漏电。	15 野外电阻。

		SEGD_UINT	line_first_test_;		//49-52 XXXX bin 线束第一条测线	1 到 99999
		SEGD_UINT	line_first_number_;		//53-56 XXXX bin 线束第一个号码1 到 99999
		SEGD_UINT	array_number_;			//57-60 XXXX bin 排列编号1 到 32
		SEGD_UINT	array_type_;			//61-64 	XXXX 	bin	排列类型	1 = 普通	 2 = 绝对 	 
		SEGD_UINT	time_break_signal_;		//65-68 	XXXX 	bin	时断信号	0 到 9999 微秒 	 
		SEGD_UINT	well_time_;				//69-72 	XXXX 	bin	井口时间	微秒	 由导航放炮或导航系统更新 	 
		SEGD_UINT	blaster_serial_number_;	//73-76 	XXXX 	bin	爆炸机标识号	- MACHA 爆炸机标识号	（0 到 15）- 或 OPSEIS 爆炸机 815 	 
											//						SAR 地址（1 到 65535）

		SEGD_UINT	blaster_status_;		//77-80 	XXXX 	bin	爆炸机状态	MACHA 爆炸机状态：
											//20电池电量太低。 	 
											//21高电压准备就绪。 	 
											//22已点火。 	 
											//23点火故障。	 
											//OPSEIS 815 爆炸机： 	 
											//20爆炸机做好放炮准备。 	 
											//21爆炸机盖打开。 	 
											//22爆炸机井口故障。	 
											//SHOTPRO 爆炸机状态： 	 
											//0未点火（接收到电台状态，但设备未点火）
											//1接收到已点火放炮和状态。一切正常
											//2未接收到状态（电台故障）
											//3接收到状态，但没有井口模拟数据(电台故障）
											//4解码器电池电量太低警告。
											//5井口检波器电阻未测量或超出允许范围。 	 
											//6雷管阻抗未测量或超出允许范围。
											//7自动井口时间拾取不成功。

		SEGD_UINT	refraction_delay_;		//81-84 	XXXX 	bin	折射延迟	毫秒
		SEGD_UINT	tb_to_t0_time_;			//85-88 	XXXX 	± bin	Tb 至 T0 时间	微秒
		SEGD_UINT	internal_time_break_signal_;	//89-92 	XXXX 	bin	内部时断信号	0 = 是 1 = 否 	 
		SEGD_UINT	field_unit_prestack_;			//93-96 	XXXX 	bin	野外设备中预叠加	0 = 是 1 = 否
		SEGD_UINT	Noise_elimination_type_;		//97-100 	XXXX 	bin	噪声抑制类型	1 无	 2 花样叠加	3 历史	 4 关闭 	 

		SEGD_UINT	low_trace_percent_;				//101-104 	XXXX 	bin	低记录道百分比	0 到 100% 	 
		SEGD_UINT	low_trace_value_;				//105-108 	XXXX 	bin	低记录道值	0 到 132 dB 	 
		SEGD_UINT	window_number_;					//109-112 	XXXX 	bin	窗口数目	1 到 64 	 
		SEGD_UINT	historical_edit_type_;				//113-116 	XXXX 	bin	历史编辑类型	1 = 归零	 2 = 剪裁 	 
		SEGD_UINT	noisy_trace_percent_;				//117-120 	XXXX 	bin	噪声记录道百分比	0 到 100% 	 
		SEGD_UINT	historical_range_;					//121-124 	XXXX 	bin	历史范围	0 到 36 dB 	 
		SEGD_UINT	historical_taper_length_;			//125-128 	XXXX 	bin	历史锥形长度 2 的指数	0 到 8
		SEGD_UINT	threshold_changed_;					//129-132 	XXXX 	bin	门槛值不变 /变化	1 = 不变2 = 变化 
		SEGD_UINT	historical_threshold_initial_value_;//133-136 	XXXX 	bin	历史门槛值初始值	0 到 132 dB
		SEGD_UINT	historical_zeroing_length_;			//137-140 	XXXX 	bin	历史归零长度	1 到 500毫秒 	 
		SEGD_UINT	process_type_;						//141-144 	XXXX 	bin	处理类型
														//1 2	无操作（原始数据）叠加
														//3 	叠加后相关
														//4 	叠加前相关
														//56 7	原始数据，叠加 原始数据，叠加后相关 原始数据，叠加前相关

		SEGD_UINT	sample_type_table_[32];				//145-272 	XXXX 	bin	采集类型表	32 个数值（128 字节） 	 
		SEGD_UINT	threshold_value_table_[32];			//273-400 	XXXX 	bin	门槛值类型表	32 个数值（128 字节） 	 
		SEGD_UINT	overlap_number_;					//401-404 	XXXX 	bin	叠加次数	1 到 32 	 
		SEGD_UCHAR	not_used1_[80];						//405-484 	XXXX 	asc	未使用 		 
		SEGD_UINT	record_length_;						//485-488 	XXXX 	bin	记录长度	100 到 128000 毫秒 	 
		SEGD_UINT	autocorrelative_peak_time_;			//489-492 	XXXX 	bin	自相关峰值时间	1 到 128000 毫秒	 
		SEGD_UINT	not_used2_;							//493-496 	XXXX 	bin	未使用 		 
		SEGD_UINT	related_reference_signal_number_;	//497-500 	XXXX 	bin	相关参考信号编号	1 到 100000 	 
		SEGD_UINT	reference_signal_length_;			//501-504 	XXXX 	bin	参考信号长度	1000 到 128000 毫秒 	 
		SEGD_UINT	scan_length_;						//505-508 	XXXX 	bin	扫描长度	1000 到 128000 毫秒 	 
		SEGD_UINT	sample_number_;						//509-512 	XXXX 	bin	采集号	1 到 32 	 
		SEGD_FLOAT	max_auxiliary_channel_max_value_;	//513-516 	XXXX 	flt	最多辅助道的最大值	IEEE 格式，单精度 	 
		SEGD_FLOAT	max_channel_max_value_;		//517-520 	XXXX 	flt	最多地震道最大值	IEEE 格式，单精度
		SEGD_UINT	dump_overlap_number_;		//521-524 	XXXX 	bin	转储叠加次数 	1..32 	 
		SEGD_UCHAR	tape_lable_[16];			//525-540 	XXXX 	asc	磁带标签	ASCII 文本，16 个字符 	 
		SEGD_UINT	tape_number_;				//541-544 	XXXX 	bin	磁带号	1 到 9999 	 
		SEGD_UCHAR	version_[16];				//545-560 	XXXX 	asc	软件版本	ASCII 文本，16 个字符 	 
		SEGD_UCHAR	date_[12];					//561-572 	XXXX 	asc	日期	ASCII 文本，12 个字符 (dd mmm yyyy) 	 
		SEGD_DOUBLE	source_easting_;			//573-580 	XXXX 	dbl	震源偏东距	默认为 GUI 设置，或由放炮或导航系统更新 	 
		SEGD_DOUBLE	source_northing_;			//581-588 	XXXX 	dbl	震源北向纬度差	默认为 GUI 设置，或由放炮或导航系统更新 	 
		SEGD_FLOAT	Source_elevation_;			//589-592 	XXXX 	flt	震源高程	默认为 GUI 设置，或由放炮或导航系统更新 	 
		SEGD_UINT	slip_sweep_mode_;			//593-596 	XXXX 	bin	所用的滑动扫描模式	0 = 否1 = 是 	 
		SEGD_UINT	file_number_per_tape_;		//597-600 	XXXX 	bin	每个磁带的文件数	1 到 9999 	 
		SEGD_UINT	file_counter_;				//601-604 	XXXX 	bin	文件计数	1 到 9999 	 
		SEGD_UCHAR	sample_error_explaination_[160];	//605-764 	XXXX 	asc	采集错误说明	ASCII 文本，160 个字符 	 
		SEGD_UINT	filter_type_;				//765-768 	XXXX 	bin	滤波器类型 	1 2 	= = 	0.8 最小 0.8 线性 	 
		SEGD_UINT	stack_dumped_;				//769-772 	XXXX 	bin	叠加已转储	0 = 否1 = 是 	 
		SEGD_UINT	stack_symbol_;				//773-776 	XXXX 	bin	叠加符号（当前）	0 = 否 1 = 正 2 = 负
		SEGD_UINT	prm_tilt_correction_;		//777-780 	XXXX 	bin	所用的 PRM 倾斜度校正	0 = 否 1 = 是 	 
		SEGD_UCHAR	line_name_[64];				//781-844 	XXXX 	asc	线束名 		 
		SEGD_UINT	operation_mode_;			//845-848 	XXXX 	bin	工作模式	bit0 = 1 正常
												//bit1 = 1 微地震
												//bit2 = 1 滑动扫描 	 
												//bit3 = 1 HFVS	 
												//bit4 = 1 导航 	

		SEGD_UINT	reserved2_;					//849-852 	XXXX 	bin	保留
		SEGD_UINT	non_log_;					//853-856 	XXXX 	bin	无记录	0 = 否	1 = 是 	 
		SEGD_UINT	listen_time_;				//857-860 	XXXX 	bin	监听时间	100 到 128000 毫秒 	 
		SEGD_UINT	dump_type_;					//861-864 	XXXX 	bin	转储类型	0 = 正常转储 1 = 原始转储	 2 = 附加转储 	 
		SEGD_UINT	reserved3_;					//865-868 	XXXX 	bin	保留
		SEGD_UINT	line_id_;					//869-872 	XXXX 	bin	线束 Id 		 
		SEGD_UCHAR	undefined_[152];			//873-1024 	0		未使用

	}SEGD_EXTENDED_HEADER, *SEGD_EXTENDED_HEADER_PTR;

	typedef struct __tagSEGD_EXTERNAL_HEADER
	{
		SEGD_UCHAR*	series_;
	}SEGD_EXTERNAL_HEADER, *SEGD_EXTERNAL_HEADER_PTR, 
	SEGD_EXTERNAL_HEADER_PRIVATE, *SEGD_EXTERNAL_HEADER_PRIVATE_PTR;

#endif //defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)

typedef struct __tagSEGD_DEMUX_TRACE_HEADER_PRIVATE
{
	SEGD_USHORT	file_number_;
	SEGD_UCHAR	scan_type_number_;
	SEGD_UCHAR	channel_set_number_;
	SEGD_USHORT	trace_number_;
	SEGD_UCHAR	first_timing_word_[3];
	SEGD_UCHAR	trace_header_extension_;
	SEGD_UCHAR	sample_skew_;
	SEGD_UCHAR	trace_edit_;
	SEGD_UCHAR	time_break_window_[3];
	SEGD_UCHAR	extended_channel_set_number_[2];
	SEGD_UCHAR	extended_file_number_[3];	
}SEGD_DEMUX_TRACE_HEADER_PRIVATE, *SEGD_DEMUX_TRACE_HEADER_PRIVATE_PTR;

typedef struct __tagSEGD_DEMUX_TRACE_HEADER
{
	SEGD_USHORT	file_number_;
	SEGD_UCHAR	scan_type_number_;
	SEGD_UCHAR	channel_set_number_;
	SEGD_USHORT	trace_number_;
	SEGD_UINT	first_timing_word_;
	SEGD_UCHAR	trace_header_extension_;
	SEGD_UCHAR	sample_skew_;
	SEGD_UCHAR	trace_edit_;
	SEGD_FLOAT	time_break_window_;
	SEGD_USHORT	extended_channel_set_number_;
	SEGD_UINT	extended_file_number_;	
}SEGD_DEMUX_TRACE_HEADER, *SEGD_DEMUX_TRACE_HEADER_PTR;

#if defined(__SEG_D_FILE_FORMAT_REV_2_1_MODE__)
	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_PRIVATE
	{
		SEGD_UCHAR	receiver_line_number_[3];
		SEGD_UCHAR	receiver_point_number_[3];
		SEGD_UCHAR	receiver_point_index_;
		SEGD_UCHAR	number_of_samples_per_trace_[3];
		SEGD_UCHAR	extended_receiver_line_number_[3];
		SEGD_UCHAR	extended_receiver_point_number_[3];
		SEGD_UCHAR	sensor_type_;
		SEGD_UCHAR	undefined_[11];
	}SEGD_TRACE_HEADER_EXTENSION_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_PRIVATE_PTR;

	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION
	{
		SEGD_UCHAR	receiver_line_number_[3];
		SEGD_UCHAR	receiver_point_number_[3];
		SEGD_UCHAR	receiver_point_index_;
		SEGD_UCHAR	number_of_samples_per_trace_[3];
		SEGD_UCHAR	extended_receiver_line_number_[3];
		SEGD_UCHAR	extended_receiver_point_number_[3];
		SEGD_UCHAR	sensor_type_;
		SEGD_UCHAR	undefined_[11];
	}SEGD_TRACE_HEADER_EXTENSION, *SEGD_TRACE_HEADER_EXTENSION_PTR;

#elif defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)

	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_1_PRIVATE
	{
		SEGD_UCHAR	receiver_line_number_[3];
		SEGD_UCHAR	receiver_point_number_[3];
		SEGD_UCHAR	receiver_point_index_;
		SEGD_UCHAR	number_of_samples_per_trace_[3];
		SEGD_UCHAR	extended_receiver_line_number_[5];
		SEGD_UCHAR	extended_receiver_point_number_[5];
		SEGD_UCHAR	sensor_type_;
		SEGD_UCHAR	undefined_[11];
	}SEGD_TRACE_HEADER_EXTENSION_1_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_1_PRIVATE_PTR;

	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_1
	{
		SEGD_UINT	receiver_line_number_;
		SEGD_UINT	receiver_point_number_;
		SEGD_UCHAR	receiver_point_index_;
		SEGD_UINT	number_of_samples_per_trace_;
		SEGD_ULONGLONG	extended_receiver_line_number_;
		SEGD_ULONGLONG	extended_receiver_point_number_;
		SEGD_UCHAR	sensor_type_;
		SEGD_UCHAR	undefined_[11];
	}SEGD_TRACE_HEADER_EXTENSION_1, *SEGD_TRACE_HEADER_EXTENSION_1_PTR;

	//sercel 428XL compatible
	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_2_PRIVATE
	{
		SEGD_UCHAR	receiver_point_east_[8];
		SEGD_UCHAR	receiver_point_north_[8];
		SEGD_FLOAT	receiver_point_high_;
		SEGD_UCHAR	detector_type_number_;
		SEGD_UCHAR	undefined_[7];
		SEGD_UINT	extended_trace_number_;
	}SEGD_TRACE_HEADER_EXTENSION_2_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_2_PRIVATE_PTR;

	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_2
	{
		SEGD_DOUBLE	receiver_point_east_;
		SEGD_DOUBLE	receiver_point_north_;
		SEGD_FLOAT	receiver_point_high_;
		SEGD_UCHAR	detector_type_number_;
		SEGD_UCHAR	undefined_[7];
		SEGD_UINT	extended_trace_number_;
	}SEGD_TRACE_HEADER_EXTENSION_2, *SEGD_TRACE_HEADER_EXTENSION_2_PTR;


	//sercel 428XL compatible
	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_3
	{
		SEGD_FLOAT	resistor_lower_limit_;
		SEGD_FLOAT	resistor_higher_limit_;
		SEGD_FLOAT	resistor_value_;
		SEGD_FLOAT	tilt_limit_;
		SEGD_FLOAT	tilt_value_;
		SEGD_UCHAR	resistor_deviation_;
		SEGD_UCHAR	tilt_deviation_;
		SEGD_UCHAR	undefined_[10];
	}SEGD_TRACE_HEADER_EXTENSION_3, *SEGD_TRACE_HEADER_EXTENSION_3_PTR,
		SEGD_TRACE_HEADER_EXTENSION_3_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_3_PRIVATE_PTR;

	//sercel 428XL compatible
	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_4
	{
		SEGD_FLOAT	capacitance_lower_limit_;
		SEGD_FLOAT	capacitance_higher_limit_;
		SEGD_FLOAT	capacitance_value_;
		SEGD_FLOAT	cutoff_lower_limit_;
		SEGD_FLOAT	cutoff_higher_limit_;
		SEGD_FLOAT	cutoff_value_;
		SEGD_UCHAR	capacitance_deviation_;
		SEGD_UCHAR	cutoff_deviation_;
		SEGD_UCHAR	undefined_[6];
	}SEGD_TRACE_HEADER_EXTENSION_4, *SEGD_TRACE_HEADER_EXTENSION_4_PTR,
		SEGD_TRACE_HEADER_EXTENSION_4_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_4_PRIVATE_PTR;;

	//sercel 428XL compatible
	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_5
	{
		SEGD_FLOAT	leakage_limit_;
		SEGD_FLOAT	leakage_value_;
		SEGD_UCHAR	undefined_1_[16];
		SEGD_UCHAR	leakage_deviation_;
		SEGD_UCHAR	undefined_2_[7];
	}SEGD_TRACE_HEADER_EXTENSION_5, *SEGD_TRACE_HEADER_EXTENSION_5_PTR,
		SEGD_TRACE_HEADER_EXTENSION_5_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_5_PRIVATE_PTR;

	//sercel 428XL compatible
	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_6_PRIVATE
	{
		SEGD_UCHAR	device_type_;
		SEGD_UCHAR	device_serial_number_[3];
		SEGD_UCHAR	seism_trace_number_;
		SEGD_UCHAR	reserved_1_[3];
		SEGD_UCHAR	component_type_;
		SEGD_UCHAR	FDU_DSU_component_serial_number_[3];
		SEGD_UCHAR	FDU_DSU_component_position_;
		SEGD_UCHAR	reserved_2_[3];
		SEGD_UCHAR	FDU_DSU_device_type_;
		SEGD_UCHAR	seism_trace_type_;
		SEGD_USHORT	reserved_3_;
		SEGD_FLOAT	detector_Sensitivity_;
		SEGD_UCHAR	undefined_[8];
	}SEGD_TRACE_HEADER_EXTENSION_6_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_6_PRIVATE_PTR;

	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_6
	{
		SEGD_UCHAR	device_type_;
		SEGD_UINT	device_serial_number_;
		SEGD_UCHAR	seism_trace_number_;
		SEGD_UCHAR	reserved_1_[3];
		SEGD_UCHAR	component_type_;
		SEGD_UINT	FDU_DSU_component_serial_number_;
		SEGD_UCHAR	FDU_DSU_component_position_;
		SEGD_UCHAR	reserved_2_[3];
		SEGD_UCHAR	FDU_DSU_device_type_;
		SEGD_UCHAR	seism_trace_type_;
		SEGD_USHORT	reserved_3_;
		SEGD_FLOAT	detector_Sensitivity_;
		SEGD_UCHAR	undefined_[8];
	}SEGD_TRACE_HEADER_EXTENSION_6, *SEGD_TRACE_HEADER_EXTENSION_6_PTR;

	//sercel 428XL compatible
	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_7_PRIVATE
	{
		SEGD_UCHAR	control_unit_type_;
		SEGD_UCHAR	control_unit_serial_number_[3];
		SEGD_UCHAR	channel_gain_scale_;
		SEGD_UCHAR	channel_filter_;
		SEGD_UCHAR	channel_data_deviation_;
		SEGD_UCHAR	channel_edit_status_;
		SEGD_FLOAT	channel_sample_conversion_factor_;
		SEGD_UCHAR	number_of_noisy_stack_;
		SEGD_UCHAR	number_of_low_stacks_;
		SEGD_UCHAR	channel_type_serial_number_;
		SEGD_UCHAR	channel_process_type_;
		SEGD_FLOAT	trace_max_value_;
		SEGD_UINT	trace_max_time_;
		SEGD_UINT	interval_polations_number_;
		SEGD_UCHAR	undefined_[4];
	}SEGD_TRACE_HEADER_EXTENSION_7_PRIVATE, *SEGD_TRACE_HEADER_EXTENSION_7_PRIVATE_PTR;

	typedef struct __tagSEGD_TRACE_HEADER_EXTENSION_7
	{
		SEGD_UCHAR	control_unit_type_;
		SEGD_UINT	control_unit_serial_number_;
		SEGD_UCHAR	channel_gain_scale_;
		SEGD_UCHAR	channel_filter_;
		SEGD_UCHAR	channel_data_deviation_;
		SEGD_UCHAR	channel_edit_status_;
		SEGD_FLOAT	channel_sample_conversion_factor_;
		SEGD_UCHAR	number_of_noisy_stack_;
		SEGD_UCHAR	number_of_low_stacks_;
		SEGD_UCHAR	channel_type_serial_number_;
		SEGD_UCHAR	channel_process_type_;
		SEGD_FLOAT	trace_max_value_;
		SEGD_UINT	trace_max_time_;
		SEGD_UINT	interval_polations_number_;
		SEGD_UCHAR	undefined_[4];
	}SEGD_TRACE_HEADER_EXTENSION_7, *SEGD_TRACE_HEADER_EXTENSION_7_PTR;

#endif //__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__

typedef struct __tagSEGD_GENERAL_TRAILER
{
	SEGD_USHORT	general_trailer_number_;
	SEGD_UCHAR	undefined_1_[8];
	SEGD_UCHAR	channel_type_;
	SEGD_UCHAR	undefined_2_[21];
}SEGD_GENERAL_TRAILER, *SEGD_GENERAL_TRAILER_PTR,
SEGD_GENERAL_TRAILER_PRIVATE, *SEGD_GENERAL_TRAILER_PRIVATE_PTR;

typedef struct __tagSEGD_FILE_TRACE_HEADER
{
	SEGD_DEMUX_TRACE_HEADER			demux_trace_header_;
	SEGD_TRACE_HEADER_EXTENSION_1	trace_header_extended_1_;
	SEGD_TRACE_HEADER_EXTENSION_2	trace_header_extended_2_;
	SEGD_TRACE_HEADER_EXTENSION_3	trace_header_extended_3_;
	SEGD_TRACE_HEADER_EXTENSION_4	trace_header_extended_4_;
	SEGD_TRACE_HEADER_EXTENSION_5	trace_header_extended_5_;
	SEGD_TRACE_HEADER_EXTENSION_6	trace_header_extended_6_;
	SEGD_TRACE_HEADER_EXTENSION_7	trace_header_extended_7_;
	SEGD_UCHAR*						trace_data_;
} SEGD_FILE_TRACE_HEADER, *SEGD_FILE_TRACE_HEADER_PTR;

typedef struct __tagSEGD_FILE
{
#if defined(__SEG_D_FILE_FORMAT_REV_2_1_MODE__)

#elif defined(__SEG_D_FILE_FORMAT_428XL_COMPATIBLE_MODE__)
	TCHAR							path_name_[MAX_PATH];
	HANDLE							file_handle_;
	OVERLAPPED						overlapped_;
	LARGE_INTEGER					file_pointer_;
	
	unsigned long					access_mode_;
	unsigned long					share_mode_;
	unsigned long					creation_disposition_;
	unsigned long					file_flag_;

	unsigned long					file_create_type_;
	unsigned long					file_io_type_;

	SEGD_GENERAL_HEADER_1			header_1_;
	SEGD_GENERAL_HEADER_2			header_2_;
	SEGD_GENERAL_HEADER_3			header_3_;
	SEGD_SCAN_TYPE_HEADER_PTR		scan_type_header_; //use malloc or new, destroy it to free or delete
	SEGD_EXTENDED_HEADER			extended_header_;
	SEGD_EXTERNAL_HEADER			external_header_;

	SEGD_FILE_TRACE_HEADER_PTR		trace_headers_;
	SEGD_FILE_TRACE_HEADER_PTR		aux_trace_headers_;
#endif
}SEGD_FILE, *SEGD_FILE_PTR;

/**
function: bool segd_file_create_private(__in_out__ SEGD_FILE_PTR segd_file_ptr)
args:	
	segd_file_ptr [in/out]	SEGD文件数据结构指针
return:
	success	return true 
	fail	return false
comment:
	use it internal
	do not support thread-safe
*/
bool	segd_file_create_private(__in_out__ SEGD_FILE_PTR segd_file_ptr);

/**
function: segd_file_seek_private(__in__ HANDLE file_handle, __in__ LONGLONG distance_to_move, __in__ enum __enuSEGD_FILE_MOVE_METHOD move_method, __in_out__ LONGLONG* file_pointer)
args:	
	file_handle			HANDLE		[in]
	distance_to_move	LONGLONG	[in]	
	move_method			__enuSEGD_FILE_MOVE_METHOD	[in]
	file_pointer		LONGLONG*	[out]	 default is NULL
return:
	true	success
	false	fail
comment:
	use it internal
	do not support thread-safe
*/
bool segd_file_seek_private(__in__ HANDLE file_handle, __in__ LONGLONG distance_to_move, __in__ enum __enuSEGD_FILE_MOVE_METHOD move_method, __in_out__ LONGLONG* file_pointer = NULL);
bool segd_file_seek_begin_private(__in__ HANDLE file_handle, __in_out__ LONGLONG* file_pointer = NULL);
bool segd_file_seek_end_private(__in__ HANDLE file_handle, __in_out__ LONGLONG* file_pointer = NULL);

/**
function: segd_file_read_private(__in__ SEGD_FILE_PTR file_handle, __in_out__ SEGD_UCHAR* buff, __in__ SEGD_UINT buff_size); 
args:	
	file_handle	[in]		文件句柄
	buf			[in]		读取存放缓冲
	buff_size	[in]		读取缓冲预设大小
return:
	>	0	读取缓冲大小
	==	0	没有数据可读
	<	0	读取错误，查看GetLastError
comment:
	use it internal
	do not support thread-safe
*/
int segd_file_read_private(__in__ SEGD_FILE_PTR file_handle, __in_out__ SEGD_UCHAR* buff, __in__ SEGD_UINT buff_size);

/**
function: segd_file_write_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_UCHAR* buff, __in__ SEGD_UINT buff_size) 
args:
	file_handle	[in]		文件句柄
	buf			[in]		写入缓冲
	buff_size	[in]		写入缓冲大小
return:
	>	0	写入缓冲大小
	==	0	没有数据写入
	<	0	写入错误，查看GetLastError
comment:
	use it internal
	do not support thread-safe
*/
int segd_file_write_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_UCHAR* buff, __in__ SEGD_UINT buff_size);

/**
function: segd_file_create(__in__ LPTSTR path_name, __in__ unsigned int create_type)
args:	
	path_name	[in]	路径
	create_type	[in]	创建标示
return:
	success	return seg-d file structure pointer
	fail	return NULL and call GetLastError function to get error info
comment:
	do not support thread-safe
*/
SEGD_FILE_PTR	segd_file_create(__in__ LPTSTR path_name, __in__ unsigned int create_type);

/**
function: segd_file_read_genernal_header_1(__in_out__ SEGD_GENERAL_HEADER_1_PTR header) 
args:	
		file_handle		[in]	SEG-D文件
		header			[out]	SEG-D普通头段数据块
return:
		true	success
		false	fail
comment:
	do not support thread-safe
*/
bool segd_file_read_genernal_header_1(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_GENERAL_HEADER_1_PTR header);
bool segd_file_read_genernal_header_2(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_GENERAL_HEADER_2_PTR header);
bool segd_file_read_genernal_header_3(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_GENERAL_HEADER_3_PTR header);
bool segd_file_read_scan_type_header(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_SCAN_TYPE_HEADER_PTR header);
bool segd_file_read_extended_header(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_EXTENDED_HEADER_PTR header);
bool segd_file_read_external_header(__in__ SEGD_FILE_PTR file_handle, __in__ int external_header_size, __out__ SEGD_EXTERNAL_HEADER_PTR header);
bool segd_file_read_trace_header(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_FILE_TRACE_HEADER_PTR header);

bool segd_file_read_demux_trace_header_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_DEMUX_TRACE_HEADER_PTR header);
bool segd_file_read_trace_header_EXTENDED_1_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_TRACE_HEADER_EXTENSION_1_PTR header);
bool segd_file_read_trace_header_EXTENDED_2_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_TRACE_HEADER_EXTENSION_2_PTR header);
bool segd_file_read_trace_header_EXTENDED_3_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_TRACE_HEADER_EXTENSION_3_PTR header);
bool segd_file_read_trace_header_EXTENDED_4_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_TRACE_HEADER_EXTENSION_4_PTR header);
bool segd_file_read_trace_header_EXTENDED_5_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_TRACE_HEADER_EXTENSION_5_PTR header);
bool segd_file_read_trace_header_EXTENDED_6_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_TRACE_HEADER_EXTENSION_6_PTR header);
bool segd_file_read_trace_header_EXTENDED_7_private(__in__ SEGD_FILE_PTR file_handle, __out__ SEGD_TRACE_HEADER_EXTENSION_7_PTR header);
bool segd_file_read_trace_data(__in__ SEGD_FILE_PTR file_handle, __in_out__ SEGD_UCHAR* data, __in__ SEGD_ULONG data_size);

/**
function: segd_file_write_genernal_header_1(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_GENERAL_HEADER_1_PTR header) 
args:	
		file_handle		[in]	SEG-D文件
		header			[in]	SEG-D普通头段数据块
return:
		true	success
		false	fail
comment:
	do not support thread-safe
*/
bool segd_file_write_genernal_header_1(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_GENERAL_HEADER_1_PTR header);
bool segd_file_write_genernal_header_2(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_GENERAL_HEADER_2_PTR header);
bool segd_file_write_genernal_header_3(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_GENERAL_HEADER_3_PTR header);
bool segd_file_write_scan_type_header(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_SCAN_TYPE_HEADER_PTR header);
bool segd_file_write_extended_header(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_EXTENDED_HEADER_PTR header);
bool segd_file_write_external_header(__in__ SEGD_FILE_PTR file_handle, __in__ int external_header_size, __in__ SEGD_EXTERNAL_HEADER_PTR header);
bool segd_file_write_trace_header(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_FILE_TRACE_HEADER_PTR header);
bool segd_file_write_trace_data(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_UCHAR* data, __in__ SEGD_ULONG data_size);

bool segd_file_write_demux_trace_header_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_DEMUX_TRACE_HEADER_PTR header);
bool segd_file_write_trace_header_EXTENDED_1_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_TRACE_HEADER_EXTENSION_1_PTR header);
bool segd_file_write_trace_header_EXTENDED_2_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_TRACE_HEADER_EXTENSION_2_PTR header);
bool segd_file_write_trace_header_EXTENDED_3_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_TRACE_HEADER_EXTENSION_3_PTR header);
bool segd_file_write_trace_header_EXTENDED_4_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_TRACE_HEADER_EXTENSION_4_PTR header);
bool segd_file_write_trace_header_EXTENDED_5_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_TRACE_HEADER_EXTENSION_5_PTR header);
bool segd_file_write_trace_header_EXTENDED_6_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_TRACE_HEADER_EXTENSION_6_PTR header);
bool segd_file_write_trace_header_EXTENDED_7_private(__in__ SEGD_FILE_PTR file_handle, __in__ SEGD_TRACE_HEADER_EXTENSION_7_PTR header);

void segd_file_close(SEGD_FILE_PTR segd_file);
void segd_file_close_private(__in_out__ SEGD_FILE_PTR segd_file_ptr);

inline int segd_file_get_trace_data_size_base(__in__ UCHAR manufacturer_code, __in__ UINT sample_number_in_trace, 
											  __in__ UINT sample_length, __in__ UINT sample_freq)
{
	int size = 0x0;

	switch(manufacturer_code){
		case 0:
			size = sample_number_in_trace * CURRENT_SEGD_MINI_DATA_LENGTH;
			break;
		case 13:
			size = (((sample_length * 1000) / sample_freq) + 1);
			size *= CURRENT_SEGD_MINI_DATA_LENGTH;
			break;
		default:
			size = sample_number_in_trace * CURRENT_SEGD_MINI_DATA_LENGTH;
			break;
	}

	return size;
}

inline int segd_file_get_trace_data_size(__in__ SEGD_FILE_PTR file_handle)
{return segd_file_get_trace_data_size_base(file_handle->header_1_.manufacturer_code_,
											file_handle->extended_header_.sample_number_in_trace_,
											file_handle->extended_header_.sample_length_,
											file_handle->extended_header_.sample_frequency_);
};

#endif //__SEGD_FILE_FORMAT_H__
