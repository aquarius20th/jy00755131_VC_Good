#include "stdafx.h"
#include "pcid.client.h"
#include <WinSock2.h>
#include <Mswsock.h>

#include "resource.h"

#pragma comment(lib, "ws2_32")

HINSTANCE instance_;// current instance
HWND main_window_;

enum __enuMATRIX_SYSTEM_CONFIGURE_INFO
{
	INPUT_PACKAGE_SIZE = 128,
	OUTPUT_PACKAGE_SIZE = 128,
	MAX_RECV_THREAD_SIZE = 256,
	MAX_SEND_THREAD_SIZE = 256
};

//sock
char listen_ip_[MAX_STRING_SIZE] = "192.168.0.25"; 
char remote_ip_[MAX_STRING_SIZE] = "192.168.0.19";
unsigned short listen_port_ = 0x9002;
unsigned short remote_port_ = 0x9999;

unsigned recv_buff_size_ = 1024 * 1024 * 10;
unsigned send_buff_size_ = 1024 * 1024 * 10;

unsigned recv_poll_time_ = 10;
unsigned snd_waiting_time_ = 1000;

unsigned snd_pack_num_per_loop_ = 1;

//UDP & PCI counter
LONG volatile upd_send_num_ = 0;
LONG volatile upd_recv_num_ = 0;

SOCKET __socket = NULL;

unsigned char input_data_[INPUT_PACKAGE_SIZE] = {0};
unsigned char output_data_[OUTPUT_PACKAGE_SIZE] = {0};

HANDLE snd_thread_handle_[MAX_SEND_THREAD_SIZE] = {NULL};
HANDLE recv_thread_handle_[MAX_RECV_THREAD_SIZE] = {NULL};

HANDLE stop_sending_event_ = NULL;
HANDLE stop_receiving_event_ = NULL;

bool be_socket_initialize_ = false;
bool be_starting_recv_ = false;
bool be_starting_snd_ = false;

unsigned int recv_thread_num_ = 0x1;//线程数量不能超过MAX_RECV_THREAD_SIZE
unsigned int snd_thread_num_ = 0x1; //线程数量不能超过MAX_SEND_THREAD_SIZE

BOOL set_window_control(bool);

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;

	input_data_[0] = 0x11;
	input_data_[1] = 0x22;
	input_data_[2] = 0x33;
	input_data_[3] = 0x44;
	input_data_[4] = 0x00;
	input_data_[5] = 0x00;
	input_data_[6] = 0x00;
	input_data_[7] = 0x00;
	input_data_[8] = 0x00;
	input_data_[9] = 0x00;
	input_data_[10] = 0x00;
	input_data_[11] = 0x00;
	input_data_[12] = 0x00;
	input_data_[13] = 0x00;
	input_data_[14] = 0x00;
	input_data_[15] = 0x00;

	BYTE IPSource = 0x00000000;
	BYTE IPAim = 0x00000000;
	BYTE PortAim = 0;
	BYTE Command = 1;

	memcpy(&input_data_[16], &IPSource, 4);
	memcpy(&input_data_[20], &IPAim, 4);
	memcpy(&input_data_[24], &PortAim, 2);
	memcpy(&input_data_[26], &Command, 2);

	// Initialize global strings
	instance_ = hInstance;
	
	main_window_ = CreateDialogParam(instance_, MAKEINTRESOURCE(IDD_MAIN_DIALOG),
		NULL, main_Wnd_proc, NULL);

	if(!main_window_){

		return FALSE;
	}

	SetWindowText(main_window_, "pcid.client.1.0.10.40.2012.02.23");
	ShowWindow(main_window_, nCmdShow);

	UpdateWindow(main_window_);


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)){

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

BOOL WINAPI main_Wnd_proc(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_DIALOG_MESSAGE(window_handle, WM_INITDIALOG, init_dialog);
		HANDLE_DIALOG_MESSAGE(window_handle, WM_CLOSE, close_dialog);
		HANDLE_DIALOG_MESSAGE(window_handle, WM_COMMAND, command_message);
		HANDLE_DIALOG_MESSAGE(window_handle, WM_DESTROY, destroy_dialog);
	}

	return 0;
}

BOOL init_dialog(HWND window_handle, HWND default_window_handle, LPARAM param)
{
	bool is_ok = TRUE;
	WSADATA wsaData;

	WORD wVersionRequested = MAKEWORD(2, 2);
	int nResult = WSAStartup(wVersionRequested, &wsaData);
	if (nResult != 0)
		return false;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){

		WSACleanup();
		WSASetLastError (WSAVERNOTSUPPORTED);
		return false;
	}

	main_window_ = window_handle;

	set_window_control(false);

	return TRUE;
}

BOOL destroy_dialog(HWND window_handle)
{
	PostQuitMessage(0);
	return 0;
}

BOOL close_dialog(HWND window_handle)
{
	return DestroyWindow(window_handle);
}

BOOL command_message(HWND window_handle, int resource_id, HWND window_control_handle, UINT notification_code)
{
	BOOL is_ok = TRUE;
	is_ok = do_control(window_handle, resource_id, window_control_handle, notification_code);
	return is_ok;
}

BOOL do_control(HWND window_handle, int resource_id, HWND window_control_handle, UINT notification_code)
{
	BOOL is_ok = TRUE;

	switch(resource_id){
		case ID_START_SOCKET:
			start_socket_button_down();
			break;
		case ID_SND_BUTTON:
			snd_button_down();
			break;
		case ID_RECV_BUTTON:
			recv_button_down();
			break;
	}

	return is_ok;
}

unsigned __stdcall udp_recv_thread_proc(void* args)
{
	int result = 0x0;

	char recv_buf[MAX_PACK_NET_SIZE] = {0};
	char buf[INPUT_PACKAGE_SIZE] = {0};
	unsigned long ioctl_argp = 0L;
	int recv_size = 0;
	int count = 0;

	//sockaddr_in addr;
	//int len = sizeof(addr);

	//addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = inet_addr("192.168.0.19");
	//addr.sin_port = htons(listen_port_);

	while(true){

		while(true){

			if(ioctlsocket(__socket, FIONREAD, &ioctl_argp) == 0){

				if(ioctl_argp > 0){

					recv_size = recv(__socket, recv_buf, sizeof(recv_buf), 0);
					//recv_size = recvfrom(__socket, buf, sizeof(buf), 0 , (SOCKADDR*)&addr, &len);

					if(recv_size == SOCKET_ERROR)			break;
					else if(recv_size == 0x0)				break;
					else if(recv_size > MAX_PACK_NET_SIZE)	break;
					else{
						InterlockedIncrement(&upd_recv_num_);
	//					SendMessage(GetDlgItem(hwnd, IDC_STATIC_UDP_RECV_NUM), WM_SETTEXT, NULL, 
						//			(LPARAM)itoa(upd_recv_num_, buf, 10));
					}
				}
				else
					break;
			}
			else{
				break;
			}
		}

		if(WaitForSingleObject(stop_receiving_event_, recv_poll_time_) == WAIT_OBJECT_0) 
			break;
	}

	_endthreadex(0x0);
	return result;
}

unsigned __stdcall udp_snd_thread_proc(void* args)
{
	int i  = 0;
	int result = 0x0;
	char buf[MAX_STRING_SIZE] = {0};
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(remote_port_);
	addr.sin_addr.s_addr = inet_addr(remote_ip_);

	while(true){

		for(i = 0; i < snd_pack_num_per_loop_; ++i){

			sendto(__socket, (char*)input_data_, sizeof(input_data_), 0, (sockaddr*)&addr, sizeof(addr));
			InterlockedIncrement(&upd_send_num_);
		}
		
		if(WaitForSingleObject(stop_sending_event_, snd_waiting_time_) == WAIT_OBJECT_0) 
			break;
	}

//	upd_send_num_ += snd_pack_num_per_loop_;
//	sprintf_s(buf, sizeof(buf), "%d", upd_send_num_);
//	SendMessage(GetDlgItem(hwnd, IDC_STATIC_UDP_SEND_NUM), WM_SETTEXT, NULL, (LPARAM)buf);
	_endthreadex(0x0);
	return result;
}

void start_socket_button_down()
{
	u_long arg = 1;
	sockaddr_in addr;
	DWORD bytes_returned = 0;
	BOOL new_behavior = FALSE;

	if(!be_socket_initialize_){

		//reset the counter
		InterlockedExchange(&upd_recv_num_, 0);
		InterlockedExchange(&upd_send_num_, 0);

		//save the option-info
		set_window_control(true);

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(listen_ip_);
		addr.sin_port = htons(listen_port_);

		__socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		bind(__socket, (SOCKADDR*)&addr, sizeof(addr));

		setsockopt(__socket, SOL_SOCKET, SO_RCVBUF, (const char*)&recv_buff_size_, sizeof(recv_buff_size_));
		setsockopt(__socket, SOL_SOCKET, SO_SNDBUF, (const char*)&send_buff_size_, sizeof(send_buff_size_));

		ioctlsocket(__socket, FIONBIO ,&arg);

		// disable  new behavior using IOCTL: SIO_UDP_CONNRESET
		WSAIoctl(__socket, SIO_UDP_CONNRESET,	&new_behavior, sizeof(new_behavior), 
			NULL, 0, &bytes_returned,	NULL, NULL);

		be_socket_initialize_ = true;
		SetWindowText(GetDlgItem(main_window_, ID_START_SOCKET), "释放SOCKET");
		SetTimer(main_window_, IDT_GET_COUNTER, 1000, timer_proc);
	}
	else{

		closesocket(__socket);
		KillTimer(main_window_, IDT_GET_COUNTER);

		be_socket_initialize_ = false;
		SetWindowText(GetDlgItem(main_window_, ID_START_SOCKET), "初始化SOCKET");
	}
}

VOID CALLBACK timer_proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	char buf[MAX_STRING_SIZE] = {0};

	sprintf_s(buf, sizeof(buf), "%d", InterlockedCompareExchange(&upd_send_num_, 0, 0));
	SendMessage(GetDlgItem(hwnd, IDC_STATIC_UDP_SEND_NUM), WM_SETTEXT, NULL, (LPARAM)buf);

	sprintf_s(buf, sizeof(buf), "%d", InterlockedCompareExchange(&upd_recv_num_, 0, 0));
	SendMessage(GetDlgItem(hwnd, IDC_STATIC_UDP_RECV_NUM), WM_SETTEXT, NULL, (LPARAM)buf);
}

void recv_button_down()
{
	int i= 0;

	if(!be_starting_recv_){

		InterlockedExchange(&upd_recv_num_, 0);

		//save the option-info
		set_window_control(true);

		stop_receiving_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);

		for(i = 0; i < recv_thread_num_; ++i)
			recv_thread_handle_[i] = (HANDLE)_beginthreadex(NULL, 0, udp_recv_thread_proc, NULL, 0x0, NULL);

		be_starting_recv_ = true;

		//set timer
		//SetTimer(main_window_, IDT_RECV_TIMER, recv_poll_time_, recv_timer_proc);
		SetWindowText(GetDlgItem(main_window_, ID_RECV_BUTTON), "停止接受数据");
	}
	else{

		SetEvent(stop_receiving_event_);
		WaitForMultipleObjects(recv_thread_num_, recv_thread_handle_, TRUE, 100);

		CloseHandle(stop_receiving_event_);
		stop_receiving_event_ = NULL;

		for(i = 0; i < recv_thread_num_; ++i){

			CloseHandle(recv_thread_handle_[i]);
			recv_thread_handle_[i] = NULL;
		}

		be_starting_recv_ = false;
		//KillTimer(main_window_, IDT_RECV_TIMER);
		SetWindowText(GetDlgItem(main_window_, ID_RECV_BUTTON), "开始接受数据");
	}
}

void snd_button_down()
{
	int i = 0x0;

	if(!be_starting_snd_){

		//save the option-info
		set_window_control(true);
		
		InterlockedExchange(&upd_send_num_, 0);

		stop_sending_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
		for(i = 0; i < snd_thread_num_; ++i)
			snd_thread_handle_[i] = (HANDLE)_beginthreadex(NULL, 0, udp_snd_thread_proc, NULL, 0x0, NULL);

		be_starting_snd_ = true;

		//set timer
		//SetTimer(main_window_, IDT_SND_TIMER, snd_waiting_time_, snd_timer_proc);
		SetWindowText(GetDlgItem(main_window_, ID_SND_BUTTON), "结束发送数据");
	}
	else{

		SetEvent(stop_sending_event_);
		WaitForMultipleObjects(snd_thread_num_, snd_thread_handle_, TRUE, 100);

		CloseHandle(stop_sending_event_);
		stop_sending_event_ = NULL;

		for(i = 0; i < snd_thread_num_; ++i){

			CloseHandle(snd_thread_handle_[i]);
			snd_thread_handle_[i] = NULL;
		}

		be_starting_snd_ = false;
		//	KillTimer(main_window_, IDT_SND_TIMER);

		SetWindowText(GetDlgItem(main_window_, ID_SND_BUTTON), "开始发送数据");
	}
}

VOID CALLBACK recv_timer_proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	char recv_buf[MAX_PACK_NET_SIZE] = {0};
	char buf[128] = {0};
	unsigned long ioctl_argp = 0L;
	int recv_size = 0;
	int count = 0;
	
	while(true){

		if(ioctlsocket(__socket, FIONREAD, &ioctl_argp) == 0){

			if(ioctl_argp > 0){

				recv_size = recv(__socket, recv_buf, sizeof(recv_buf), 0);
				//recv_size = recvfrom(__socket, buf, sizeof(buf), 0 , (SOCKADDR*)&addr, &len);
				
				if(recv_size == SOCKET_ERROR)			break;
				else if(recv_size == 0x0)				break;
				else if(recv_size > MAX_PACK_NET_SIZE)	break;
				else{
					++upd_recv_num_;					
					SendMessage(GetDlgItem(hwnd, IDC_STATIC_UDP_RECV_NUM), WM_SETTEXT, 
						NULL, (LPARAM)itoa(upd_recv_num_, buf, 10));
				}
			}
			else
				break;
		}
		else{
			break;
		}
	}

}

VOID CALLBACK snd_timer_proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	char buf[MAX_STRING_SIZE] = {0};
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(remote_port_);
	addr.sin_addr.s_addr = inet_addr(remote_ip_);

	for(int i = 0; i < snd_pack_num_per_loop_; ++i){
		sendto(__socket, (char*)input_data_, sizeof(input_data_), 0, (sockaddr*)&addr, sizeof(addr));
		InterlockedIncrement(&upd_send_num_);
	}

	sprintf_s(buf, sizeof(buf), "%d", InterlockedCompareExchange(&upd_send_num_, 0, 0));
	SendMessage(GetDlgItem(hwnd, IDC_STATIC_UDP_SEND_NUM), WM_SETTEXT, NULL, (LPARAM)buf);
}

BOOL set_window_control(bool is_save)
{
	char buf[MAX_STRING_SIZE] = {0};

	if(is_save){

		SendDlgItemMessage(main_window_, IDC_EDIT_LISTEN_PORT, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		listen_port_ = atoi(buf);

		SendDlgItemMessage(main_window_, IDC_EDIT_REMOTE_IP, WM_GETTEXT, 
			sizeof(remote_ip_), (LPARAM)remote_ip_);

		SendDlgItemMessage(main_window_, IDC_EDIT_LISTEN_IP, WM_GETTEXT, 
			sizeof(listen_ip_), (LPARAM)listen_ip_);

		SendDlgItemMessage(main_window_, IDC_EDIT_REMOTE_PORT, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		remote_port_ = atoi(buf);

		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_RECV_NUM, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		recv_buff_size_ = atoi(buf);

		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_SEND_NUM, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		send_buff_size_ = atoi(buf);

		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_POLL_TIME, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		recv_poll_time_ = atoi(buf);

		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_SND_NUM_PER_HALF_HUNDRAND, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		snd_pack_num_per_loop_ = atoi(buf);

		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_SND_POLL_TIME, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		snd_waiting_time_ = atoi(buf);

	}
	else{

		sprintf_s(buf, sizeof(buf), "%d", listen_port_);
		SendDlgItemMessage(main_window_, IDC_EDIT_LISTEN_PORT, WM_SETTEXT, NULL, (LPARAM)buf);

		SendDlgItemMessage(main_window_, IDC_EDIT_REMOTE_IP, WM_SETTEXT, NULL, (LPARAM)remote_ip_);

		SendDlgItemMessage(main_window_, IDC_EDIT_LISTEN_IP, WM_SETTEXT, NULL, (LPARAM)listen_ip_);

		sprintf_s(buf, sizeof(buf), "%d", remote_port_);
		SendDlgItemMessage(main_window_, IDC_EDIT_REMOTE_PORT, WM_SETTEXT, NULL, (LPARAM)buf);

		sprintf_s(buf, sizeof(buf), "%d", recv_buff_size_);
		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_RECV_NUM, WM_SETTEXT, NULL, (LPARAM)buf);

		sprintf_s(buf, sizeof(buf), "%d", send_buff_size_);
		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_SEND_NUM, WM_SETTEXT, NULL, (LPARAM)buf);

		sprintf_s(buf, sizeof(buf), "%d", recv_poll_time_);
		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_POLL_TIME, WM_SETTEXT, NULL, (LPARAM)buf);

		sprintf_s(buf, sizeof(buf), "%d", snd_pack_num_per_loop_);
		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_SND_NUM_PER_HALF_HUNDRAND, WM_SETTEXT, NULL, (LPARAM)buf);

		sprintf_s(buf, sizeof(buf), "%d", snd_waiting_time_);
		SendDlgItemMessage(main_window_, IDC_EDIT_SOCK_SND_POLL_TIME, WM_SETTEXT, NULL, (LPARAM)buf);
	}

	return TRUE;
}
