
// MFCApplication3Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication3.h"
#include "MFCApplication3Dlg.h"
#include "afxdialogex.h"

#include <gsw_func.h>

#include <windows.h>
#include <tchar.h>
#include <afxstr.h>  // CString

#include <stack>
#include <cctype> // isdigit(), isspace() 등
#include <map>
#include <vector>
#include <sstream> // 문자열 Stream

#include <chrono> // 시간 측정
#include <iostream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
const CString PIE = _T("3.1415926535897932384626433832795");
const CString E = _T("2.7182818284590452353602874713527");

enum class ButtonID : int {
	PLUS = 15,
	MINUS = 16,
	MULTIPLY = 17,
	DIVIDE = 18,
	POSTFIX = 19,
	EQUAL = 20,
	CLEAR = 21,
	PIE = 23,
	E = 24,
	POW = 25,
	ABS = 27,
	EXP = 28,
	MOD = 29,
	SQRT = 30,
	LPAREN = 31,
	RPAREN = 32,
	FACT = 33,
	POW10 = 34,
	LOG = 35,
	LN = 36,
	BENCHADD = 37
};

// 연산자 우선순위 정의
map<char, int> precedence = {
	{'+', 1},
	{'-', 1},
	{'*', 2},
	{'/', 2},
	{'^', 3}
};

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication3Dlg 대화 상자



CMFCApplication3Dlg::CMFCApplication3Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION3_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication3Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication3Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication3Dlg::OnBnClickedButton3)
	ON_COMMAND_RANGE(1000,1100, &CMFCApplication3Dlg::OnDigitClicked)
END_MESSAGE_MAP()

bool CMFCApplication3Dlg::isFunction(const string& token) {
	return token == "abs" || token == "exp" || token == "sqrt" || token == "fact" || token == "log" || token == "ln" || token == "mod" || token == "sin" || token == "cos" || token == "tan";
}

//중위표기 string -> 후위표기로 변환
vector<string> CMFCApplication3Dlg::shuntingYard(const string& expr) {
	vector<string> output; //output은 출력. opStack은 스택.
	stack<char> opStack;
	string number;
	string func = "";

	for (size_t i = 0; i < expr.size(); ++i) {
		char ch = expr[i];

		// 공백은 무시
		if (isspace(ch)) continue;

		if (isalpha(ch)) {
			while (i < expr.size() && isalpha(expr[i])) {
				func += expr[i++];
			}
			--i;
			for (size_t j = 0; j < func.size();++j) {
				opStack.push(func[j]);
			}
			continue;
		}

		// 숫자 or 소수점 or 음수
		if (isdigit(ch) || ch == '.' ||
			(ch == '-' && (i == 0 || expr[i - 1] == '('))) {
			number += ch;
		}
		else {
			if (!number.empty()) {
				output.push_back(number);
				number.clear();
			}

			if (ch == '(') {
				opStack.push(ch);
			}

			else if (ch == ')') {
				while (!opStack.empty() && opStack.top() != '(') {
					output.push_back(string(1, opStack.top())); // string(1, ch) = ch를 1번 반복해서 만든 문자열
					opStack.pop();
				}
				if (!opStack.empty()) opStack.pop(); // '(' 제거

				if (isFunction(func)) { //abs 같은 func 있으면 바로 출력
					output.push_back(func);
					for(size_t i = 0; i < func.size(); ++i)
						opStack.pop();
					func = "";
				}
			}

			else if (precedence.count(ch)) {
				while (!opStack.empty() && opStack.top() != '(' && precedence[opStack.top()] >= precedence[ch]) {
					output.push_back(string(1, opStack.top()));
					opStack.pop();
				}
				opStack.push(ch);
			}

			else {
				AfxMessageBox(_T("잘못된 문자"));
			}
		}
	}

	// 마지막 숫자 처리
	if (!number.empty()) output.push_back(number);

	// 남은 연산자 pop
	while (!opStack.empty()) {
		output.push_back(string(1, opStack.top()));
		opStack.pop();
	}

	return output;
}

//Operator가 연속으로 들어오는지 체크
bool CMFCApplication3Dlg::isConsecutiveOperator(TCHAR lastChar, int newChar) {
	return (
				(lastChar == _T('+') || lastChar == _T('-') || lastChar == _T('*') || lastChar == _T('/')) &&
				(static_cast<int>(ButtonID::PLUS) <= newChar && newChar <= static_cast<int>(ButtonID::DIVIDE))
			)
				? true : false;
}


// 이벤트
void CMFCApplication3Dlg::OnDigitClicked(UINT nID)
{
	int digit = nID - 1000;
	CString text;
	
	m_btnStatics[0].CWnd::GetWindowText(text);
	if(text == "0")
		text = "";
	

	//1~9 버튼 클릭
	if (-1 < digit && digit < 10) {
		CString digitStr;
		digitStr.Format(_T("%d"), digit);
		text += digitStr;
		m_btnStatics[0].SetWindowText(text);
	}

	// Operator가 연속으로 오면 리턴
	int textLength = text.GetLength();
	if (0 < textLength && isConsecutiveOperator(text.GetAt(textLength - 1), digit)) return;

	//variable for equals
	CT2A ansi(text);                   // CString → const char*. CT2A는 CString → ANSI 문자열 변환용 ATL 클래스
	string expr(ansi);            // const std::string& expr 에 넣을 수 있음
	CString msg;
	string result = "";

	ButtonID btnID = static_cast<ButtonID>(digit);

	switch (btnID) {
		case ButtonID::POSTFIX:
			printPostfix(shuntingYard(expr));
			break;

		case ButtonID::PLUS:
			m_btnStatics[0].SetWindowText(text + _T("+"));
			break;

		case ButtonID::MINUS:
			m_btnStatics[0].SetWindowText(text + _T("-"));
			break;

		case ButtonID::MULTIPLY:
			m_btnStatics[0].SetWindowText(text + _T("*"));
			break;

		case ButtonID::DIVIDE:
			m_btnStatics[0].SetWindowText(text + _T("/"));
			break;

		case ButtonID::EQUAL: {
			result = evaluatePostfix(shuntingYard(expr));
			CString msg;
			msg.Format(_T("%S"), result.c_str()); //std::string → CString 변환
			m_btnStatics[1].SetWindowText(text + _T(" = ") + msg);
			m_btnStatics[0].SetWindowText(_T("0"));
			break;
		}

		case ButtonID::CLEAR:
			m_btnStatics[0].SetWindowText(_T("0"));
			m_btnStatics[1].SetWindowText(_T("0"));
			break;

		case ButtonID::PIE:
			m_btnStatics[1].SetWindowText(_T("π ="));
			m_btnStatics[0].SetWindowText(PIE);
			break;

		case ButtonID::E:
			m_btnStatics[1].SetWindowText(_T("e ="));
			m_btnStatics[0].SetWindowText(E);
			break;

		case ButtonID::POW:
			m_btnStatics[0].SetWindowText(text + _T("^"));  
			break;

		case ButtonID::ABS:
			m_btnStatics[0].SetWindowText(text + _T("abs("));  
			break;

		case ButtonID::EXP:
			m_btnStatics[0].SetWindowText(text + _T("exp("));  // 지수 함수 e^x
			break;

		case ButtonID::MOD:
			m_btnStatics[0].SetWindowText(text + _T("%"));  
			break;

		case ButtonID::SQRT:
			m_btnStatics[0].SetWindowText(text + _T("sqrt("));  // 루트
			break;

		case ButtonID::LPAREN:
			m_btnStatics[0].SetWindowText(text + _T("("));
			break;

		case ButtonID::RPAREN:
			m_btnStatics[0].SetWindowText(text + _T(")"));
			break;

		case ButtonID::FACT:
			m_btnStatics[0].SetWindowText(text + _T("fact("));
			break;

		case ButtonID::POW10:
			m_btnStatics[0].SetWindowText(text + _T("10^"));
			break;

		case ButtonID::LOG:
			m_btnStatics[0].SetWindowText(text + _T("log(")); 
			break;

		case ButtonID::LN:
			m_btnStatics[0].SetWindowText(text + _T("ln("));   // 자연로그
			break;

		case ButtonID::BENCHADD:
			m_btnStatics[1].SetWindowText(text + _T("BENCHADD"));   // 자연로그
			benchAdd();
			break;

		default:
			break;
	}
}
//keyboard Event start
BOOL CMFCApplication3Dlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR)  // 문자 입력 (Shift 조합 포함)
	{
		switch (pMsg->wParam)
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '+': case '-': case '*': case '/':
		case '(': case ')':
		case '=':
			OnKeyboardInput((char)pMsg->wParam);
			break;

		case '\r':  // Enter
			OnKeyboardInput('=');
			break;

		case '\b':  // Backspace
			OnKeyboardInput('C');
			break;

		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMFCApplication3Dlg::OnKeyboardInput(char ch)
{
	CString text;
	m_btnStatics[0].GetWindowText(text);

	switch (ch)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		if (text == _T("0"))
			text = ch;
		else
			text += ch;
		m_btnStatics[0].SetWindowText(text);
		break;

	case '+': case '-': case '*': case '/':
		text += ch;
		m_btnStatics[0].SetWindowText(text);
		break;

	case '=':
	{
		CT2A ansi(text);
		std::string expr(ansi);
		std::string result = evaluatePostfix(shuntingYard(expr));
		CString msg;
		msg.Format(_T("%S"), result.c_str());
		m_btnStatics[1].SetWindowText(text + _T(" = ") + msg);
		m_btnStatics[0].SetWindowText(_T("0"));
		break;
	}

	case 'C':  // Clear
		m_btnStatics[0].SetWindowText(_T("0"));
		m_btnStatics[1].SetWindowText(_T("0"));
		break;

	case '(': case ')':
		text += ch;
		m_btnStatics[0].SetWindowText(text);
		break;

	default:
		break;
	}
}

//keyboard Event end

BOOL CMFCApplication3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//button create
	for (int i = 0; i < 100;i++) {
		CString label;
		int x = 200 + (i % 5) * 60; //초기위치 + 버튼 몇개마다 개행 + 버튼 간격
		int y = 150 + (i / 5) * 31;
		int btnW = 40;
		int btnH = 30;
		bool isSkip = false;

		switch (i) {
			case 0: label.Format(_T("%d"), i); break;
			case 1: label.Format(_T("%d"), i); break;
			case 2: label.Format(_T("%d"), i); break;
			case 3: label.Format(_T("%d"), i); break;
			case 4: label.Format(_T("%d"), i); break;
			case 5: label.Format(_T("%d"), i); break;
			case 6: label.Format(_T("%d"), i); break;
			case 7: label.Format(_T("%d"), i); break;
			case 8: label.Format(_T("%d"), i); break;
			case 9: label.Format(_T("%d"), i); break;
			case 10: label.Format(_T("File Write")); btnW *= 2; btnW += 20; break; // btn사이즈 2배; btn 2칸 건너뜀;
			case 11: continue;
			case 12: label.Format(_T("File Load")); btnW *= 2; btnW += 20; break;
			case 13: continue;
			case 14: label.Format(_T("getKnownPath")); btnW *= 3; btnW += 40; break;
			case 15: label.Format(_T("+")); break;
			case 16: label.Format(_T("-")); break;
			case 17: label.Format(_T("*")); break;
			case 18: label.Format(_T("/")); break;
			case 19: label.Format(_T("Postfix Print")); btnW *= 3; btnW += 40; break;
			case 20: label.Format(_T("=")); break;
			case 21: label.Format(_T("Clear")); btnW *= 2; btnW += 20; break;
			case 22: continue;
			case 23: label.Format(_T("π")); break;
			case 24: label.Format(_T("e")); break; //자연상수
			case 25: label.Format(_T("x^2")); break;
			case 26: label.Format(_T("1/x")); break;
			case 27: label.Format(_T("|x|")); break;
			case 28: label.Format(_T("exp")); break;//자연상수 제곱
			case 29: label.Format(_T("mod")); break;
			case 30: label.Format(_T("sqrt")); break;
			case 31: label.Format(_T("(")); break;
			case 32: label.Format(_T(")")); break;
			case 33: label.Format(_T("n!")); break;
			case 34: label.Format(_T("10^x")); break;
			case 35: label.Format(_T("log")); break;
			case 36: label.Format(_T("ln")); break; //자연로그
			case 37: label.Format(_T("BenchAdd")); btnW *= 3; btnW += 40; break;
			case 38: continue;
			case 39: continue;
		}
		
		m_btnDigits[i].Create(
			label,
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CRect(x, y, x + btnW, y + btnH), //btn size
			this,
			1000 + i
		);
	}

	//텍스트 박스
	for (int i = 0; i < 2;i++) {
		CString label;
		int x = 280 + (i % 1) * 41; //btn margin
		int y = 100 + (i / 1) * 31;

		switch (i) {
			case 0: label.Format(_T("%d"), 0); break;			
			case 1: label.Format(_T("%d"), 0); y /= 2; break;
		}

		m_btnStatics[i].Create(
			label,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			CRect(x, y, x + 300, y + 30), //btn size
			this,
			1100 + i
		);
	}

	//button create end

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 후위식 출력 
void CMFCApplication3Dlg::printPostfix(const vector<string>& postfix) {

	CString msg;

	for (const auto& tok : postfix) {
		msg += CString(tok.c_str());
		msg += _T(" ");
	}

	AfxMessageBox(msg);

}

void CMFCApplication3Dlg::normalizeLength(string& a, string& b) {
	while (a.length() < b.length()) a = "0" + a;
	while (b.length() < a.length()) b = "0" + b;
}

string CMFCApplication3Dlg::add(string a, string b) {
	normalizeLength(a, b);
	string result = "";
	int carry = 0;

	for (int i = a.length() - 1; i >= 0; --i) {
		int sum = (a[i] - '0') + (b[i] - '0') + carry;
		carry = sum / 10;
		result = char(sum % 10 + '0') + result;
	}

	if (carry > 0) result = "1" + result;
	return result;
}

string CMFCApplication3Dlg::minus(string a, string b) {
	normalizeLength(a, b);
	string result = "";
	int carry = 0;

	for (int i = a.length() - 1; i >= 0; --i) {
		int sum = (a[i] - '0') - (b[i] - '0') - carry;
		if (sum < 0) {
			carry = 1;
			sum *= -1;
		}
		else
			carry = 0;
		result = char(sum + '0') + result;
	}

	if (carry > 0) result = "-" + result;
	return result;
}

string CMFCApplication3Dlg::multiply(string a, string b) {
	int n = a.size();
	int m = b.size();
	vector<int> result(n + m, 0);

	// 뒤에서부터 곱셈
	for (int i = n - 1; i >= 0; --i) {
		for (int j = m - 1; j >= 0; --j) {
			int mul = (a[i] - '0') * (b[j] - '0');
			int p1 = i + j;
			int p2 = i + j + 1;
			int sum = mul + result[p2];

			result[p2] = sum % 10;
			result[p1] += sum / 10;
		}
	}

	// 결과 문자열 만들기
	string res = "";
	for (int num : result)
		if (!(res.empty() && num == 0)) res += to_string(num);

	return res.empty() ? "0" : res;
}

string CMFCApplication3Dlg::divide(string dividend, string divisor) {
	if (divisor == "0") return "DivideByZeroError";

	string result = "";
	string current = "";

	for (char digit : dividend) {
		current += digit;
		// 0 제거
		while (current.length() > 1 && current[0] == '0')
			current.erase(0, 1);

		int count = 0;
		while (current.length() > 0 && multiply(divisor, to_string(count + 1)) <= current) {
			count++;
		}
		result += (count + '0');
		current = minus(current, multiply(divisor, to_string(count)));
	}

	// 0 제거
	int pos = result.find_first_not_of('0');
	if (pos == string::npos) return "0";
	return result.substr(pos);
}

string CMFCApplication3Dlg::power_string(string base, string exponent) {
	string result = "1";

	//지수
	int exp = stoi(exponent);

	while (exp > 0) {
		result = multiply(result, base);
		--exp;
	}

	return result;
}

string CMFCApplication3Dlg::abs(string base) {
	return base[0] == '-' ? base.substr(1) : base;
}

string CMFCApplication3Dlg::mod(string dividend, string divisor) {
	if (divisor == "0") return "DivideByZeroError";

	string current = "";

	for (char digit : dividend) {
		current += digit;

		// 앞자리 0 제거
		while (current.length() > 1 && current[0] == '0')
			current.erase(0, 1);

		int count = 0;
		while (current.length() > 0 && multiply(divisor, to_string(count + 1)) <= current) {
			count++;
		}

		current = minus(current, multiply(divisor, to_string(count)));
	}
	return current.empty() ? "0" : current;
}

// exp(x) = e^x. precision = 소수점 몇 자리 까지
string CMFCApplication3Dlg::exp(string x, int precision) {
	int X = stoi(x);

	string result = "1" + string(precision, '0'); // string() 은 문자'0'을 5번 반복해서 만든 문자열 리턴함
	string term = result;

	//테일러 급수로 근사하여 계산
	for (int i = 1; i <= 20; ++i) {
		term = divide(multiply(term, x), to_string(i)); // term = term * x / i
		result = add(result, term);
	}

	// 소수점 위치 넣기
	if (result.length() <= precision)
		result = string(precision + 1 - result.length(), '0') + result;

	result.insert(result.end() - precision, '.');
	return result;
}

string CMFCApplication3Dlg::fact(string curr) {
	if (curr == "0" || curr == "1") return "1";
	string prev = fact(minus(curr, "1"));
	return multiply(curr, prev);
}

string CMFCApplication3Dlg::evaluatePostfix(const vector<string>& postfix) {
	stack<string> st;

	for (const string& token : postfix) {
		if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-')) { //숫자인 경우 푸쉬
			st.push(token);
		}
		else {
			string result = "";
			if (st.size() < 2 && (token == "+" || token == "-" || token == "*" || token == "/" || token == "%" || token == "^")) {//숫자 1개 이하인데 사칙 연산나온 경우 리턴.
				AfxMessageBox(_T("Invalid expression"));
				return "";
			}

			//함수 부분
			else if (token == "abs") {
				string b = st.top(); st.pop();
				result = abs(b);
				st.push(result);
				continue;
			}
			else if (token == "exp") {
				string b = st.top(); st.pop();
				result = exp(b,5);
				st.push(result);
				continue;
			}
			else if (token == "sqrt") {
				string b = st.top(); st.pop();
				result = abs(b);
				st.push(result);
				continue;
			}
			else if (token == "fact") {
				string b = st.top(); st.pop();
				result = fact(b);
				st.push(result);
				continue;
			}
			else if (token == "log") {
				string b = st.top(); st.pop();
				result = abs(b);
				st.push(result);
				continue;
			}
			else if (token == "ln") {
				string b = st.top(); st.pop();
				result = abs(b);
				st.push(result);
				continue;
			}

			string b = st.top(); st.pop();
			string a = st.top(); st.pop();

			if (token == "+") result = add(a,b);
			else if (token == "-") result = minus(a,b);
			else if (token == "*") result = multiply(a, b);
			else if (token == "/") result = divide(a, b);
			else if (token == "^") result = power_string(a, b);
			else if (token == "%") result = mod(a, b);
			else { AfxMessageBox(_T("not supported operator")); return "";}

			st.push(result);
		}
	}
	
	return st.top();
}



void CMFCApplication3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);

		mstring strMasd;
		tstring strTasd;

		strMasd = gswf::uni2utf(strTasd);
		strTasd = gswf::utf2uni(strMasd);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

int CMFCApplication3Dlg::bitwise_add(int x, int y)
{
	while (y != 0)
	{
		int carry = x & y;        // 올림이 생기는 자리
		x = x ^ y;                // 올림 없는 덧셈 (자리수 합)
		y = carry << 1;           // 올림을 왼쪽으로 한 자리 이동해서 다시 더함
	}
	return x;
}

void CMFCApplication3Dlg::benchAdd()
{
	using namespace std::chrono;

	// 1. string
	string a = "123456789";
	string b = "987654321";
	string result;
	auto start1 = high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i)
		result = add(a, b);
	auto end1 = high_resolution_clock::now();
	auto duration1 = duration_cast<microseconds>(end1 - start1).count();

	// 2. ALU (Arithmetic Logic Unit)
	int x = 123456789;
	int y = 987654321;
	int z = 0;
	auto start2 = high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i)
		z = x + y;
	auto end2 = high_resolution_clock::now();
	auto duration2 = duration_cast<microseconds>(end2 - start2).count();
	
	// 3. bitwise
	x = 123456789;
	y = 987654321;
	z = 0;
	auto start3 = high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i)
		z = bitwise_add(x, y);
	auto end3 = high_resolution_clock::now();
	auto duration3 = duration_cast<microseconds>(end3 - start3).count();
	
	CString msg;
	msg.Format(_T("String add: %lldus초\nint add: %lldus초\nbit add: %lldus초"), duration1, duration2, duration3);
	AfxMessageBox(msg);
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void WriteUtf8FileWinAPI(const CString& path)
{
	// 저장할 내용 (유니코드 CString -> UTF-8 바이트 변환)
	CString content = _T("텍스트 파일 내용123456789.\r\n다른 줄도 포함됩니다.121363463");
	CT2W unicodeStr(content);  // CString → const wchar_t*

	// 유니코드 → UTF-8 변환
	//UTF - 8로 변환할 때 필요한 버퍼 크기 계산
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, unicodeStr, -1, NULL, 0, NULL, NULL);
	//변환된 문자열을 담을 char[] 버퍼를 동적 할당하고, UTF-8로 변환 수행
	char* utf8Buffer = new char[utf8Len];
	WideCharToMultiByte(CP_UTF8, 0, unicodeStr, -1, utf8Buffer, utf8Len, NULL, NULL);

	// UTF-8 BOM (EF BB BF) //Byte Order Mark
	//BYTE bom[] = { 0xEF, 0xBB, 0xBF };

	try
	{
		CFile file;
		if (file.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			//file.Write(bom, 3);                  // BOM 먼저 씀
			file.Write(utf8Buffer, utf8Len - 1); // 마지막 null 문자 제외하고 저장
			file.Close();

			AfxMessageBox(_T("파일이 UTF-8로 저장되었습니다"));
		}
		else
		{
			AfxMessageBox(_T("파일을 열 수 없습니다"));
		}
	}
	catch (CFileException* e)
	{
		TCHAR err[1024];
		e->GetErrorMessage(err, 1024);
		AfxMessageBox(err);
		e->Delete();
	}

	delete[] utf8Buffer;

}

void ReadUtf8FileWinAPI(const CString& path)
{
	//path경로 WinAPI로 파일 열기, 읽기 모드, 공유 허용, 기존 파일 열기,
	HANDLE hFile = CreateFile(path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("파일 열기 실패"));
		return;
	}

	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE || fileSize == 0)
	{
		CloseHandle(hFile);
		AfxMessageBox(_T("파일 크기 오류 또는 비어있음"));
		return;
	}

	BYTE* buffer = new BYTE[fileSize + 1];
	DWORD bytesRead;
	BOOL result = ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
	CloseHandle(hFile);

	if (!result || bytesRead != fileSize)
	{
		delete[] buffer;
		AfxMessageBox(_T("파일 읽기 실패"));
		return;
	}

	buffer[fileSize] = '\0';  // null-terminate

	// BOM 제거 (UTF-8 BOM: EF BB BF)
	int offset = 0;
	if (fileSize >= 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
		offset = 3;

	// UTF-8 → UTF-16 (CString) 변환
	int wideLen = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)(buffer + offset), -1, NULL, 0);
	if (wideLen == 0)
	{
		delete[] buffer;
		AfxMessageBox(_T("인코딩 변환 실패"));
		return;
	}

	wchar_t* wstr = new wchar_t[wideLen];
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)(buffer + offset), -1, wstr, wideLen);

	CString content(wstr);
	AfxMessageBox(content);  // 결과 확인

	delete[] buffer;
	delete[] wstr;
}

void CMFCApplication3Dlg::OnBnClickedButton1() 
{
	// 파일 저장 다이얼로그 생성
	CFileDialog dlg(FALSE,                 // FALSE = 저장, TRUE = 열기
		_T("txt"),             // 기본 확장자
		_T("output.txt"),      // 기본 파일 이름
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		//_T("텍스트 파일(*.txt)|*.txt|All Files (*.*)|*.*||"),
		_T("All Files (*.*)|*.*||"),
		this);

	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName();
		WriteUtf8FileWinAPI(path);
	}
}

void CMFCApplication3Dlg::OnBnClickedButton2()
{
	// 1. 파일 열기 다이얼로그
	CFileDialog dlg(TRUE,                  // TRUE = 파일 열기
		_T("txt"),             // 기본 확장자
		NULL,                  // 초기 파일 이름 없음
		OFN_FILEMUSTEXIST,     // 존재하는 파일만 선택 가능
		_T("All Files (*.*)|*.*||"),
		this);

	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName();  // 선택된 파일 경로
		ReadUtf8FileWinAPI(path);
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMFCApplication3Dlg::OnBnClickedButton3()
{
	// 예: 문서 폴더 GUID 사용
	tstring docPath = gswf::get_KnownPathEx(FOLDERID_Documents);

#ifdef _UNICODE
	AfxMessageBox(docPath.c_str());
#else
	AfxMessageBox(CA2T(docPath.c_str())); // ANSI 문자열을 TCHAR로
#endif
}