// DlgImpulsive.cpp : 实现文件
//
// vivi 2010.11.9
#include "stdafx.h"
#include "Operation.h"
#include "OperaStruct.h"
#include "DlgImpulsive.h"
#include "MainFrm.h"


// CDlgImpulsive 对话框

IMPLEMENT_DYNAMIC(CDlgImpulsive, CBCGPDialog)

CDlgImpulsive::CDlgImpulsive(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgImpulsive::IDD, pParent)
	, m_comboPulg(PROCESS_PLUG1)
	, m_comboBoxType(PROCESS_LCI428)
	, m_wndEditGrid(NULL)
	, m_wndListGrid(NULL)
{
	EnableVisualManagerStyle();
}

CDlgImpulsive::~CDlgImpulsive()
{
}

void CDlgImpulsive::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_PULG, m_comboPulg);
	DDX_CBIndex(pDX, IDC_COMBO_BOXTYPE, m_comboBoxType);
	DDX_Control(pDX, IDC_STATICGRIDEDIT, m_ctrlEditGridLocation);
	DDX_Control(pDX, IDC_STATICGRID, m_ctrlGridLocation);
}


BEGIN_MESSAGE_MAP(CDlgImpulsive, CBCGPDialog)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SETUPGRID_CLICK,OnGridItemClick)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_DBLCLICK,OnGridItemDblClk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgImpulsive::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CDlgImpulsive::OnBnClickedButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgImpulsive::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CDlgImpulsive 消息处理程序

BOOL CDlgImpulsive::OnInitDialog(void)
{
	CBCGPDialog::OnInitDialog();
  
	// 设置对话框尺寸为IDC_STATICPICTURE的尺寸
	CWnd::SetWindowPos(NULL,0,0,dlgRect.Width(),dlgRect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	
	m_wndEditGrid = new CGridCtrlEdit;
	m_wndListGrid = new CGridCtrlList;
	CRect rectGrid;
	m_ctrlGridLocation.GetWindowRect (&rectGrid);
	ScreenToClient (&rectGrid);
	m_wndListGrid->CreateGrid(rectGrid,this);
	m_wndListGrid->InsertColumn (0, _T("Nb"), 30);
	m_wndListGrid->InsertColumn (1, _T("Processing"), 90);

	m_ctrlEditGridLocation.GetWindowRect (&rectGrid);
	ScreenToClient (&rectGrid);
	m_wndEditGrid->CreateGrid(rectGrid,this);
	m_wndEditGrid->InsertColumn (0, _T("Nb"), 30);
	m_wndEditGrid->InsertColumn (1, _T("Processing"), 90);
	m_wndEditGrid->AddRow();
	m_wndEditGrid->AdjustLayout();

	Load();

	return 0;
}

void CDlgImpulsive::OnBnClickedButtonAdd()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CGridCtrlOperation::OnBnClickedButtonAddA(m_wndEditGrid,m_wndListGrid);

}

void CDlgImpulsive::OnBnClickedButtonChange()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CGridCtrlOperation::OnBnClickedButtonChangeA(m_wndEditGrid,m_wndListGrid);
}

void CDlgImpulsive::OnBnClickedButtonDelete()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CGridCtrlOperation::OnBnClickedButtonDeleteA(m_wndEditGrid,m_wndListGrid);
}

// 载入脉冲类型设置
void CDlgImpulsive::Load(void)
{
	CMainFrame*	pFrm = (CMainFrame*)AfxGetMainWnd();
	if(!pFrm)
		return;
	int				i, j, nCount, mCount;
	CProcessType*	pProcess=NULL;
	nCount = pFrm->m_AllProcesses.GetCount();

	m_wndListGrid->RemoveAll();

	for (i=0;i<nCount;i++)
	{
 		pProcess = pFrm->m_AllProcesses.GetProcessType(i);
 		if(pProcess && pProcess->m_byProcessType==PROCESS_IMPULSIVE)
 		{
			// 载入设置的数据类型参数
			_variant_t	oVariant;
			CString		str;
			oVariant = pProcess->m_dwFDUSN;
			// 必须要改变类型为字符串，也为在两个网格的对应条目拷贝时，类型应该一致。否则会报错
			oVariant.ChangeType(VT_BSTR);
			str = oVariant;
			GetDlgItem(IDC_EDIT_SERIALNB)->SetWindowText(str);

			oVariant = pProcess->m_dwRecordLen;
			oVariant.ChangeType(VT_BSTR);
			str = oVariant;
			GetDlgItem(IDC_EDIT_RECORDLENGTH)->SetWindowText(str);

			oVariant = ((CProcessImpulsive*)pProcess)->m_dwRefractionDelay;
			oVariant.ChangeType(VT_BSTR);
			str = oVariant;
			GetDlgItem(IDC_EDIT_REFRACTIONDELAY)->SetWindowText(str);

			oVariant = ((CProcessImpulsive*)pProcess)->m_dwTBWindow;
			oVariant.ChangeType(VT_BSTR);
			str = oVariant;
			GetDlgItem(IDC_EDIT_TBWINDOW)->SetWindowText(str);
			mCount = pProcess->m_arrAuxiChannel.GetCount();
			if (mCount > 0)
			{
				AUXICHANNEL m_auxichannel;
				for (j=0; j<mCount; j++)
				{
 					m_auxichannel = pProcess->m_arrAuxiChannel[j];
 					AppendRecord(m_auxichannel);
				}
			}
 			
 		}
	}
}
// 增加一条辅助记录道设置数据
void CDlgImpulsive::AppendRecord(AUXICHANNEL m_arrAuxiChannel)
{
	_variant_t  oVariant;
	CBCGPGridRow* pRow = m_wndListGrid->CreateRow(m_wndListGrid->GetColumnCount());
	// Label
	oVariant = m_arrAuxiChannel.m_dwNb;
	oVariant.ChangeType(VT_BSTR);
	pRow->GetItem (0)->SetValue(oVariant);

	oVariant = m_arrAuxiChannel.m_szProcessing;
	oVariant.ChangeType(VT_BSTR);
	pRow->GetItem (1)->SetValue(oVariant);

	// 设置行的属性数据为辅助记录道编号
	pRow->SetData(m_arrAuxiChannel.m_dwNb);
	// 增加一行记录
	m_wndListGrid->AddRow(pRow);	
	m_wndListGrid->AdjustLayout();

}

void CDlgImpulsive::OnDestroy()
{
	CBCGPDialog::OnDestroy();
	if(m_wndEditGrid)
	{			
		m_wndEditGrid->RemoveAll();		
		delete m_wndEditGrid;
	}
	m_wndEditGrid = NULL;
	if(m_wndListGrid)
	{			
		m_wndListGrid->RemoveAll();		
		delete m_wndListGrid;
	}
	m_wndListGrid = NULL;
}
// 单击辅助道列表框
LRESULT CDlgImpulsive::OnGridItemClick(WPARAM wParam, LPARAM lParam)
{
	CGridCtrlOperation::OnGridItemClick(m_wndEditGrid, m_wndListGrid);	
	return 0;
}  
// 双击辅助道列表框
LRESULT  CDlgImpulsive::OnGridItemDblClk(WPARAM wParam, LPARAM lParam)
{	
	CBCGPGridItem*	pItem = (CBCGPGridItem*)lParam;
	if(!pItem)
		return 0;
	CBCGPGridRow*   pRow = pItem->GetParentRow();
	if(!pRow)
		return 0;
	if(pRow->GetOwnerList() != m_wndListGrid)
		return 0;
	CGridCtrlOperation::OnGridItemDblClk(m_wndEditGrid, m_wndListGrid);
	return 0;
}