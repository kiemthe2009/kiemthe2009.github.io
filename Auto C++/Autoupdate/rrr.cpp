void CBmpDlg::OnMouseMove(UINT nFlags, CPoint point)

{
	
	CWnd * pWnd; 
	
	HCURSOR MyCursor; 
	
	CRgn m_regionPower; //Region of POWER Button
	
	CRgn m_regionPlay; //Region of PLAY Button
	
	//����λͼ��ť�ľ�������
	
	m_regionPower.CreateEllipticRgnIndirect(CRect(27,56,72,92)); 
	
	m_regionPlay.CreateEllipticRgnIndirect(CRect(78,56,120,92));
	
	CString ShowString0 = "���Ĺ����λͼ��ť��!!!";
	
	�� �� �� ��
		
		m_bPlay = FALSE;
	
	m_bPower= FALSE;
	
	if (m_regionPower.PtInRegion(point)) //�������λͼ��ť֮�� 
		
	{
		
		m_bPower = TRUE; 
		
		//��λͼ��ť���óɻ������״̬
		
		pWnd= GetDlgItem(IDC_BUTTON_POWER);
		
		pWnd->SetFocus(); 
		
		SetCapture();
		
		InputEdit().SetWindowText(ShowString0);
		
		InputEdit().ShowWindow(TRUE); 
		
		//���������С����״
		
		MyCursor = AfxGetApp()->LoadCursor(IDC_MYCURSOR); 
		
		::SetCursor(MyCursor);
		
		VERIFY(m_Play.LoadBitmaps("PLAYU","PLAYD","PLAYF","PLAYX"));
		
		m_bPressedPlay = FALSE;
		
		return;
		
	}
	
	if (m_regionPlay.PtInRegion(point)) //�������λͼ��ť֮�� 
		
	{ 
		
		if (m_bPowerOn) { //�����Դ�ѱ�����
			
			m_bPlay = TRUE; 
			
			pWnd= GetDlgItem(IDC_BUTTON_PLAY);
			
			pWnd->SetFocus(); 
			
			SetCapture();
			
			InputEdit().SetWindowText(ShowString0);
			
			InputEdit().ShowWindow(TRUE);
			
			MyCursor = AfxGetApp()->LoadCursor(IDC_MYCURSOR);
			
			::SetCursor(MyCursor); 
			
			VERIFY(m_Power.LoadBitmaps("POWERONU","POWEROND","POWERONF"));
			
		}
		
		else { //�����Դ�ѱ��ر�
			
			ReleaseCapture();
			
			InputEdit().SetWindowText(ShowString0+ShowString2);
			
			InputEdit().ShowWindow(TRUE); 
			
			VERIFY(m_Power.LoadBitmaps("POWEROFU","POWEROFD","POWEROFF"));
			
		} 
		
		m_bPressedPower= FALSE;
		
		return;
		
	}
	
	//����������е�λͼ��ť֮�� 
	
	ReleaseCapture();
	
	InputEdit().SetWindowText(ShowString1);
	
	InputEdit().ShowWindow(TRUE); 
	
	pWnd= GetDlgItem(IDOK);
	
	pWnd->SetFocus(); 
	
	VERIFY(m_Play.LoadBitmaps("PLAYU","PLAYD","PLAYF","PLAYX"));
	
	if (m_bPowerOn)
		
		VERIFY(m_Power.LoadBitmaps("POWERONU","POWEROND","POWERONF"));
	
	else
		
		VERIFY(m_Power.LoadBitmaps("POWEROFU","POWEROFD","POWEROFF"));
	
	m_bPressedPlay = FALSE;
	
	m_bPressedPower= FALSE;
	
	CDialog::OnMouseMove(nFlags, point);
	
}