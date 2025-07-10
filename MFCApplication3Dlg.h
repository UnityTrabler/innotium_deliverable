
// MFCApplication3Dlg.h: 헤더 파일
//

#pragma once
#include <atlconv.h>
#include <string>
#include <vector>

using namespace std;

// CMFCApplication3Dlg 대화 상자
class CMFCApplication3Dlg : public CDialogEx
{
	// 생성입니다.
public:
	CMFCApplication3Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION3_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	bool isFunction(const string& token);

	

	vector<string> shuntingYard(const string& expr);

	afx_msg
		bool isConsecutiveOperator(TCHAR lastChar, int newChar);
	
	void OnDigitClicked(UINT nID);
	BOOL PreTranslateMessage(MSG* pMsg);
	CButton m_btnDigits[100];
	CStatic m_btnStatics[10];


	// 구현입니다.
protected:
	HICON m_hIcon;

	void OnKeyboardInput(char ch);

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();

	std::string evaluatePostfix(const std::vector<std::string>& postfix);

	void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg void OnPaint();
	int bitwise_add(int x, int y);
	void benchAdd();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void printPostfix(const std::vector<std::string>& postfix);
	void normalizeLength(std::string& a, std::string& b);
	std::string add(std::string a, std::string b);
	std::string minus(std::string a, std::string b);
	std::string multiply(std::string a, std::string b);
	std::string divide(std::string a, std::string b);
	string power_string(std::string base, std::string exponent);
	string abs(string base);
	string mod(string dividend, string divisor);
	string exp(string x, int precision);
	string fact(string a);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};

