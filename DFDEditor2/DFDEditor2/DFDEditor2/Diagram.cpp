#include "stdafx.h"
#include "Diagram.h"
#include"Element.h"
#include"DFDStream.h"

Diagram::Diagram()
{
}


Diagram::~Diagram()
{
}

void Diagram::add(Element* e){
	elems.push_back(e);
}
bool Diagram::Find(CPoint pos, Element *&e){
	vector<Element*>::iterator it;
	for (it = elems.begin(); it != elems.end(); it++){
		if ((*it)->Contains(pos)){
			e = (*it);
			return true;
		}
	}
	return false;
}
bool Diagram::FindStream(Element *e, list<Element*>&ElementQueue){
	return true;
}
HWND Diagram::SearchWnd(Element *e){
	HWND hWnd = NULL;
	map<Element*, HWND>::iterator it;
	for (it = process2Diagram.begin(); it != process2Diagram.end(); it++){
		if (it->first == e){
			hWnd = it->second;
			break;
		}
	}
	return hWnd;
}
void Diagram::SetElementforStreambyElement(Element *e, CPoint pos){
	vector<Element*>::iterator it;
	for (it = elems.begin(); it != elems.end(); it++){
		if ((*it)->isStream()){
			Stream *tempse = (Stream*)(*it);
			if (tempse->startisInfieldof(e, pos)){
				tempse->setStartElement(e);
			}
			if (tempse->endisInfieldof(e, pos)){
				(tempse->setEndElement(e));
			}
		}
	}
}
void Diagram::SetStartElementforStream(Stream *se, CPoint pos){
	vector<Element*>::iterator it;
	for (it = elems.begin(); it != elems.end(); it++){
		if ((*it)->Contains(pos)){
			//Stream *tempse = (Stream*)e;
			//tempse->setStartElement(*it);
			se->setStartElement(*it);
			break;
		}
	}
}
void Diagram::SetEndElementforStream(Stream *se, CPoint pos){
	vector<Element*>::iterator it;
	for (it = elems.begin(); it != elems.end(); it++){
		if ((*it)->Contains(pos)){
			//Stream *tempse = (Stream*)e;
			//tempse->setEndElement(*it);
			se->setEndElement(*it);
			break;
		}
	}
}
void Diagram::Remove(Element *currente){

}
/*void Diagram::DrawDiagram(map<CPoint*, int>&drawdiagram){
	vector<Element*>::iterator it;
	for (it = elems.begin(); it != elems.end(); it++){
	drawdiagram.insert(pair<CPoint*, int>(&(*it)->midPoint, (*it)->type));
	}
	}*/

void Diagram::DrawDiagram(vector<CPoint*>&poss, vector<int>&types, vector<CString>&strs, CPoint startmidend[][3]){
	vector<Element*>::iterator it;
	int i = 0;
	Stream *tempse;
	for (it = elems.begin(); it != elems.end(); it++){
		poss.push_back(&(*it)->midPoint);
		types.push_back((*it)->type);
		strs.push_back((*it)->text);
		if ((*it)->isStream()){
			tempse = (Stream*)(*it);
			startmidend[i][0] = tempse->getStart();
			startmidend[i][1] = tempse->getmidPoint();
			startmidend[i][2] = tempse->getEnd();
			i++;
		}
	}
}
void Diagram::InsertMap(Element *e, HWND hWnd){
	process2Diagram.insert(pair<Element*, HWND>(e, hWnd));
}

void Diagram::FindStreams(vector<Element*>& elemq) //传入EndElement传出所有Stream
{
	vector<Element*>::iterator it;
	Stream* tmp;
	vector<Element*> midElems = elemq;
	elemq.clear();
	int endElemNum = midElems.size();  //获取终点节点数目
	while (!midElems.empty()){  //中间节点不为空
		for (it = elems.begin(); it != elems.end(); it++){
			if ((*it)->isStream())
			{
				tmp = (Stream*)(*it);
				if (tmp->getEndElement()->midPoint == midElems.front()->midPoint)  // Stream终点图元为传入图元时
				{
					elemq.push_back(tmp);  //传出流
					midElems.push_back(tmp->getStartElement());
				}
			}
		}
		midElems.erase(midElems.begin());  //第一个元素清除
		endElemNum--;
	}
}
