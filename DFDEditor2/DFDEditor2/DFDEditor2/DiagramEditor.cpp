#include "stdafx.h"
#include "DiagramEditor.h"
#include"Tool.h"
#include"RectangleTool.h"
#include"EllipseTool.h"
#include"LineTool.h"
#include"StreamTool.h"
#include"EditTool.h"
#include"LookupTool.h"
#include"DFDStream.h"
#include"Element.h"
#include"Diagram.h"
#include"MainFrm.h"
#include"ChildFrm.h"
#include"DFDEditor2View.h"
#include"DFDEditor2Doc.h"

DiagramEditor::DiagramEditor()
{
	currente = NULL;
	currentd = NULL;
	currenttool = NULL;
	cpMainFrame = NULL;
	cpChildFrame = NULL;

	rt = new RectangleTool(this);
	et = new EllipseTool(this);
	lt = new LineTool(this);
	st = new StreamTool(this);
	edt = new EditTool(this);
	lkt = new LookupTool(this);

	currenttool = edt;
}


DiagramEditor::~DiagramEditor()
{

}
bool DiagramEditor::hasCurrentE(){
	if (currente) return true;
	else return false;
}
void DiagramEditor::InsertMap(HWND hWnd, Diagram *d){
	WindowtoDiagram.insert(pair<HWND, Diagram*>(hWnd, d));
}

void DiagramEditor::Menu(int item){
	this->ClearCurrentE();
	switch (item){
	case 1:
		SetCurrentTool(rt);
		break;
	case 2: SetCurrentTool(et);
		break;
	case 3: SetCurrentTool(lt);
		break;
	case 4: SetCurrentTool(st);
		break;
	case 5: SetCurrentTool(lkt);
		break;
	default:;
	}
}
void DiagramEditor::SetCurrentE(Element *e){
	currente = e;
	rt->SetCurrentE(e);
	et->SetCurrentE(e);
	lt->SetCurrentE(e);
	st->SetCurrentE(e);
	edt->SetCurrentE(e);
	lkt->SetCurrentE(e);
}
void DiagramEditor::SetCurrentD(Diagram *d){
	currentd = d;
	rt->SetCurrentD(d);
	et->SetCurrentD(d);
	lt->SetCurrentD(d);
	st->SetCurrentD(d);
	edt->SetCurrentD(d);
	lkt->SetCurrentD(d);
}
void DiagramEditor::SetCurrentTool(Tool *t){
	currenttool = t;
}
void DiagramEditor::ClearCurrentE(){
	currente = NULL;
	rt->ClearCurrentE();
	et->ClearCurrentE();
	lt->ClearCurrentE();
	st->ClearCurrentE();
	edt->ClearCurrentE();
	lkt->ClearCurrentE();
}
void DiagramEditor::ClearCurrentTool(){
	currenttool = edt;
}
void DiagramEditor::AddDiagram(){
	cpMainFrame = (CMainFrame*)AfxGetMainWnd();
	cpChildFrame = (CChildFrame*)cpMainFrame->GetActiveFrame();
	CDFDEditor2Doc *cpDoc = (CDFDEditor2Doc*)cpChildFrame->GetActiveDocument();
	Diagram *d = new Diagram();
	HWND hWnd = cpChildFrame->m_hWnd;

	ds.push_back(d);  // 存入图形数组中
	this->InsertMap(hWnd, d);
	SetCurrentD(d);
	currenttool->SetCurrentD(d);
	cpDoc->SetDiagram(d);
}
void DiagramEditor::SearchDiagram(HWND hWnd, Diagram *&d){
	map<HWND, Diagram*>::iterator it;
	for (it = WindowtoDiagram.begin(); it != WindowtoDiagram.end(); it++){
		if (it->first == hWnd){
			d = it->second;
			//AfxMessageBox(_T("Window Found."));
			return;
		}
	}
	AfxMessageBox(_T("Window Not Found."));
}
void DiagramEditor::Press(CPoint pos){
	//Diagram *d = NULL;
	cpMainFrame = (CMainFrame*)AfxGetMainWnd();
	cpChildFrame = (CChildFrame*)cpMainFrame->GetActiveFrame();
	HWND hWnd = cpChildFrame->m_hWnd;
	if (!hWnd) AfxMessageBox(_T("Top Window Not Found!"));

	//currenttool->Select(d, hWnd);
	//SetCurrentD(d);
	currenttool->Press(pos, hWnd);
}
void DiagramEditor::RightPress(CPoint pos){
	//SetCurrentTool(edt);
	cpMainFrame = (CMainFrame*)AfxGetMainWnd();
	cpChildFrame = (CChildFrame*)cpMainFrame->GetActiveFrame();
	HWND hWnd = cpChildFrame->m_hWnd;
	if (!hWnd) AfxMessageBox(_T("Top Window Not Found!"));

	//edt->RightPress(pos, hWnd);
	if (currenttool->GetType() == LOOKUPTOOL)  // 右击释放lookuptool并改为edittool
	{
		LookupTool *newlkt = (LookupTool*)currenttool;
		newlkt->RightPress();
		this->SetCurrentTool(edt);
	}
	EditTool *newet = (EditTool*)currenttool;
	newet->RightPress(pos, hWnd);
}
void DiagramEditor::DoubleClick(CPoint pos){
	cpMainFrame = (CMainFrame*)AfxGetMainWnd();
	cpChildFrame = (CChildFrame*)cpMainFrame->GetActiveFrame();
	HWND hWnd = cpChildFrame->m_hWnd;

	if (!hWnd) AfxMessageBox(_T("Top Window Not Found!"));

	if (currenttool->GetType() == EDITTOOL)
	{
		EditTool *newet = (EditTool*)currenttool;
		newet->DoubleClick(pos, hWnd);
	}
	else if (currenttool->GetType() == LOOKUPTOOL)
	{
		LookupTool *newlkt = (LookupTool*)currenttool;
		newlkt->DoubleClick(pos, hWnd);
	}

}
void DiagramEditor::Release(CPoint pos){
	if (currenttool->GetType() != LOOKUPTOOL)
	{
		currenttool->Release(pos);
		ClearCurrentTool();
	}
}
void DiagramEditor::RightRelease(CPoint pos){
	if (hasCurrentE()){
		EditTool *newet = (EditTool*)currenttool;
		newet->RightRelease(pos);
		ClearCurrentTool();
	}
}
void DiagramEditor::ExitEdit(){
	if (currenttool->GetType() == EDITTOOL){
		EditTool *newet = (EditTool*)currenttool;
		newet->GotoFatherWnd();
	}
}
void DiagramEditor::Remove(){
	if (currenttool->GetType() == EDITTOOL){
		EditTool *newet = (EditTool*)currenttool;
		newet->Remove();
	}
}

void DiagramEditor::Redraw(bool original){
	cpMainFrame = (CMainFrame*)AfxGetMainWnd();
	cpChildFrame = (CChildFrame*)cpMainFrame->GetActiveFrame();
	CDFDEditor2Doc *cpDoc = (CDFDEditor2Doc*)cpChildFrame->GetActiveDocument();
	CDFDEditor2View *cpView = (CDFDEditor2View*)cpChildFrame->GetActiveView();
	CDC *pDC = new CDC();
	cpDoc->SetOri(original);  // 设置是否正在创建图元的过程
	cpDoc->SetDiagram(currentd);  // 修改当前活动窗口文档类Doc中的对应图形
	cpDoc->SetElement(currente);  // 修改当前活动窗口文档类Doc中的对应活动图元
	cpView->Invalidate();  // 重绘当前活动窗口
}
void DiagramEditor::Draw(Element *doce, Diagram *docd, bool original, CPoint pos, CDC *pDC){
	if (!currente && original && (currenttool == rt || currenttool == et || currenttool == lt || currenttool == st)){
		this->DrawOriginal(pDC, pos);  //需要各种createtool的头一个操作是clearelement 
	}
	if (docd){   // 遍历当前窗口对应的图形，画出所有图元
		this->SetCurrentD(docd);
		CPen *pen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
		pDC->SelectObject(pen);
		vector<CPoint>poss;
		vector<int>types;
		vector<CString>strs;
		vector<CPoint>::iterator it1;
		vector<int>::iterator it2;
		vector<CString>::iterator it3;
		CPoint startmidend[20][3];
		docd->DrawDiagram(poss, types, strs, startmidend);
		int i = 0;
		for (it1 = poss.begin(), it2 = types.begin(), it3 = strs.begin(); it1 != poss.end(); it1++, it2++, it3++){
			switch ((*it2)){
			case 1:
				pDC->Rectangle((*it1).x - 60, (*it1).y - 40, (*it1).x + 60, (*it1).y + 40);
				break;
			case 2:
				pDC->Ellipse((*it1).x - 50, (*it1).y - 50, (*it1).x + 50, (*it1).y + 50);
				break;
			case 3:
				pDC->MoveTo((*it1).x - 60, (*it1).y + 20);
				pDC->LineTo((*it1).x + 60, (*it1).y + 20);
				pDC->MoveTo((*it1).x - 60, (*it1).y + 40);
				pDC->LineTo((*it1).x + 60, (*it1).y + 40);
				break;
			case 4:
				pDC->MoveTo(startmidend[i][0].x, startmidend[i][0].y);//起始点
				pDC->LineTo(startmidend[i][0].x, startmidend[i][1].y);
				pDC->LineTo(startmidend[i][2].x, startmidend[i][1].y);
				pDC->LineTo(startmidend[i][2].x, startmidend[i][2].y);//最终点

				if (startmidend[i][1].y < startmidend[i][2].y)   //箭头朝下
				{
					pDC->LineTo(startmidend[i][2].x + 10, startmidend[i][2].y - 20);
					pDC->MoveTo(startmidend[i][2].x, startmidend[i][2].y);
					pDC->LineTo(startmidend[i][2].x - 10, startmidend[i][2].y - 20);
				}
				else if (startmidend[i][1].y > startmidend[i][2].y) //箭头朝上
				{
					pDC->LineTo(startmidend[i][2].x - 10, startmidend[i][2].y + 20);
					pDC->MoveTo(startmidend[i][2].x, startmidend[i][2].y);
					pDC->LineTo(startmidend[i][2].x + 10, startmidend[i][2].y + 20);
				}
				else if (startmidend[i][1].y == startmidend[i][2].y)
				{
					if (startmidend[i][1].x < startmidend[i][2].x)  //箭头朝右
					{
						pDC->LineTo(startmidend[i][2].x - 20, startmidend[i][2].y - 10);
						pDC->MoveTo(startmidend[i][2].x, startmidend[i][2].y);
						pDC->LineTo(startmidend[i][2].x - 20, startmidend[i][2].y + 10);
					}
					else {  //箭头朝左
						pDC->LineTo(startmidend[i][2].x + 20, startmidend[i][2].y - 10);
						pDC->MoveTo(startmidend[i][2].x, startmidend[i][2].y);
						pDC->LineTo(startmidend[i][2].x + 20, startmidend[i][2].y + 10);

					}
				}
				i++;
				break;
			default:;
			}
			pDC->TextOutW((*it1).x - 20, (*it1).y - 10, (*it3));
		}
	}
	if (currenttool->GetType() == LOOKUPTOOL)
	{
		currentd->Highlight(pDC);
	}
	if (doce){
		this->SetCurrentE(doce);
		this->Highlight(pDC);
	}
}
void DiagramEditor::DrawOriginal(CDC *pDC, CPoint pos){
	if (currenttool == rt){
		pDC->Rectangle(pos.x - 60, pos.y - 40, pos.x + 60, pos.y + 40);
	}
	else if (currenttool == et){
		pDC->Ellipse(pos.x - 50, pos.y - 50, pos.x + 50, pos.y + 50);
	}
	else if (currenttool == lt){
		pDC->MoveTo(pos.x - 60, pos.y + 20);
		pDC->LineTo(pos.x + 60, pos.y + 20);
		pDC->MoveTo(pos.x - 60, pos.y + 40);
		pDC->LineTo(pos.x + 60, pos.y + 40);
	}
	else if (currenttool == st){

		//st->SetCurrentE(currente);
		//Stream *tempse = (Stream*)currente;
		//CPoint start = tempse->GetStart();
		//CPoint end = tempse->GetEnd();
		pDC->MoveTo(pos.x - 60, pos.y - 40);
		pDC->LineTo(pos.x - 60, pos.y);
		pDC->LineTo(pos.x + 60, pos.y);
		pDC->LineTo(pos.x + 60, pos.y + 40);


	}
	else{}
}
void DiagramEditor::Highlight(CDC *pDC){
	CPen *pen = new CPen(PS_SOLID, 5, RGB(255, 0, 0));
	pDC->SelectObject(pen);
	CPoint mp = currente->GetMidPoint();
	if (currente->isSource()){
		pDC->Rectangle(mp.x - 60, mp.y - 40, mp.x + 60, mp.y + 40);
	}
	else if (currente->isProcess()){
		pDC->Ellipse(mp.x - 50, mp.y - 50, mp.x + 50, mp.y + 50);
	}
	else if (currente->isDataStorage()){
		pDC->MoveTo(mp.x - 60, mp.y + 20);
		pDC->LineTo(mp.x + 60, mp.y + 20);
		pDC->MoveTo(mp.x - 60, mp.y + 40);
		pDC->LineTo(mp.x + 60, mp.y + 40);
	}
	else{
		Stream *tempse = (Stream*)currente;
		CPoint start = tempse->GetStart();
		CPoint end = tempse->GetEnd();
		CPoint mid = tempse->GetMidPoint();
		pDC->MoveTo(start.x, start.y);
		pDC->LineTo(start.x, mid.y);
		pDC->LineTo(end.x, mid.y);
		pDC->LineTo(end.x, end.y);

		if (mid.y < end.y)  //箭头朝下
		{
			pDC->MoveTo(end.x, end.y);
			pDC->LineTo(end.x - 10, end.y - 20);
			pDC->MoveTo(end.x, end.y);
			pDC->LineTo(end.x + 10, end.y - 20);
		}
		else if (mid.y > end.y) //箭头朝上
		{
			pDC->MoveTo(end.x, end.y);
			pDC->LineTo(end.x + 10, end.y + 20);
			pDC->MoveTo(end.x, end.y);
			pDC->LineTo(end.x - 10, end.y + 20);
		}
		else  {
			if (mid.x < end.x)  //箭头朝右
			{
				pDC->MoveTo(end.x, end.y);
				pDC->LineTo(end.x - 20, end.y - 10);
				pDC->MoveTo(end.x, end.y);
				pDC->LineTo(end.x - 20, end.y + 10);
			}
			else  //箭头朝左
			{

				pDC->MoveTo(end.x, end.y);
				pDC->LineTo(end.x + 20, end.y - 10);
				pDC->MoveTo(end.x, end.y);
				pDC->LineTo(end.x + 20, end.y + 10);
			}
		}
		this->Focus(pDC);
	}
	pDC->TextOutW(mp.x - 20, mp.y - 10, currente->GetText());
}
void DiagramEditor::Move(CPoint pos, CPoint oldpos){
	currenttool->Move(pos, oldpos);
}
void DiagramEditor::Focus(CDC *pDC){
	Stream *tempse = (Stream*)currente;
	CPen *pen = new CPen(PS_SOLID, 2, RGB(255, 0, 0));
	pDC->SelectObject(pen);
	pDC->Rectangle(tempse->GetStart().x - 5, tempse->GetStart().y - 5, tempse->GetStart().x + 5, tempse->GetStart().y + 5);
	pen = new CPen(PS_SOLID, 2, RGB(0, 0, 255));
	pDC->SelectObject(pen);

	pDC->MoveTo(tempse->GetEnd().x, tempse->GetEnd().y);
	pDC->LineTo(tempse->GetEnd().x - 5, tempse->GetEnd().y - 5);
	pDC->MoveTo(tempse->GetEnd().x - 5, tempse->GetEnd().y + 5);
	pDC->LineTo(tempse->GetEnd().x, tempse->GetEnd().y);
	pDC->Rectangle(tempse->GetEnd().x - 5, tempse->GetEnd().y - 5, tempse->GetEnd().x + 5, tempse->GetEnd().y + 5);
}
void DiagramEditor::EndCreate(Element *e){
	this->SetCurrentE(e);
	this->Redraw(false);
}

HWND DiagramEditor::SearchDiagramtoProcess(Diagram *&d, DFDProcess *p){
	HWND hWnd = currentd->SearchWnd(p);
	if (hWnd) this->SearchDiagram(hWnd, d);  // d是返回值
	return hWnd;
}