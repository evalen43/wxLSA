#include "Input.h"
#include "MyFrame.h"
//#include "Matrix_d.h"



Input::Input(wxString filein) 
{
	InputXml(filein);
}

#pragma region ----wxStructure

/*------------------------------  wxStructure  -------------------------------------------------------------*/
int Input::wxStructure(wxString structure)
{
	int ndf = 0;
	int i = -1;
	if (structure == "Frame2D")  i = 1;
	if (structure == "Frame3D")   i = 2;
	if (structure == "Truss3D") i = 3;
	if (structure == "Truss2D")  i = 4;
	if (structure == "Grid") i = 5;
	if (structure == "Frame2D_8DOF")i = 6;
	enum StringValues { Frame2D = 1, Frame3D = 2, Truss3D = 3, Truss2D = 4, Grid = 5, Frame2D_8DOF = 6 };
	switch (i)
	{
	case Frame2D:
		structure = "Frame2D";
		ndf = 3;
		break;
	case Frame3D:
		structure = "Frame3D";
		ndf = 6;
		break;
	case Truss3D:
		structure = "Truss3D";
		ndf = 3;
		break;
	case Truss2D:
		structure = "Truss2D";
		ndf = 2;
		break;
	case Grid:
		structure = "Grid";
		ndf = 3;
		break;
	case Frame2D_8DOF:
		structure = "Frame2D_8DOF";
		ndf = 4;
		break;
	}
	return ndf;
	//ndfel = nne * ndf;
}
#pragma endregion wxStructure

#pragma region ----InputXml

/*---------------------------  ParseXml  ----------------------------------------------------*/
void Input::InputXml(wxString filein)
{
	doc = new wxXmlDocument();

	if (!doc->Load(filein))return;

	wxString s1 = doc->GetRoot()->GetName();
	ndf=wxStructure(s1);
	nne = 2;
	ndfel = ndf * nne;
	//wxLogMessage(s1);

	wxXmlNode* child = doc->GetRoot()->GetChildren();
	while (child)
	{

		wxString tagname = child->GetName();

		//wxLogMessage(tagname);
		wxString content = child->GetNodeContent();

		if (tagname == "nodes")
		{
			attrvalue1 = child->GetAttribute("unitL", "default-value");
			wxNodes(content, attrvalue1);
		}
		if (tagname == "elements")
		{
			wxElements(content);
		}
		if (tagname == "section")
		{
			attrvalue1 = child->GetAttribute("unitL", "default-value");
			wxSections(content, attrvalue1);
		}
		if (tagname == "material")
		{
			attrvalue1 = child->GetAttribute("unitS", "default-value");
			attrvalue2 = child->GetAttribute("den", "default-value");
			wxMaterial(content, attrvalue1, attrvalue2);
		}
		if (tagname == "boundary") wxBoundary(content);

		wxXmlNode* load = child->GetChildren();
		if (tagname == "loading")
		{

			//while (load){
			wxString tagname = load->GetName();
			wxLogMessage(tagname);
			wxString content = load->GetNodeContent();
			if (tagname == "loadednodes")
			{
				//wxString content = load->GetNodeContent();
				wxLogMessage(content);
			}
			if (tagname == "loadedmembers")
			{
				//wxString content = load->GetNodeContent();
				wxLogMessage(content);
			}
			//}
			load = load->GetNext();

		}

		child = child->GetNext();
	}
	return;
}

#pragma endregion InputXml



#pragma region ----wxNodes

/*------------------------------  wxNodes   -------------------------------------------------------------------------------*/
void Input::wxNodes(wxString content, wxString attrv1)
{
	Node node; //Constructor for struc Node
	std::string ucv = attrv1.ToStdString();

	double scaleL=Matrix_d::ToMeters(ucv);
	if (attrv1 == "default-value") scaleL = 1.0; else scaleL = Matrix_d::ToMeters(ucv);
	double x = 0.0; double y = 0.0; double z = 0.0;
	wxStringTokenizer tokenizer(content, "\n");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		nodeInput.Add(token);

	}
	nn = nodeInput.GetCount();
	N = nn * ndf;
	txt_out->WriteText("Number of Nodes: " + wxString::Format(wxT("%i"), nn) + "\n");
	for (int i = 0; i < nn; i++)
	{
		wxString s1 = nodeInput[i];
		wxStringTokenizer tokenizer(s1, " ");
		while (tokenizer.HasMoreTokens())
		{
			wxString token = tokenizer.GetNextToken();
			token.erase(std::remove(token.begin(), token.end(), '\t'), token.end());
			lineinput.Add(token);
		}
		ntoken = lineinput.GetCount();
		nlist.push_back(lineinput[0]);
		if (ntoken > 1) x = wxAtof(lineinput[1]) * scaleL;
		if (ntoken > 2) y = wxAtof(lineinput[2]) * scaleL;
		if (ntoken > 3) z = wxAtof(lineinput[3]) * scaleL;
		std::string s5 = std::string(lineinput[0].mb_str());
		node = { s5,  x, y, z };
		wxcoor.push_back(node);
		m_txt_out->WriteText(wxString::Format(wxT("%s %i\t %.2f\t %.2f\t %.2f\n"), s5, i, x, y, z));
		x = 0.0; y = 0.0; z = 0.0;
		lineinput.Clear();

	}
}
#pragma endregion wxNodes

#pragma region ----wxElements

/*------------------------  wxElements  ------------------------------------------------------------------------------*/

void Input::wxElements(wxString content)
{

	wxStringTokenizer tokenizer(content, "\n");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		elemInput.Add(token);

	}
	ne = elemInput.GetCount();

	m_txt_out->WriteText("Number of Elements: " + wxString::Format(wxT("%i"), ne) + "\n");
	int L = 0;
	ms = 0;
	for (int i = 0; i < ne; i++)
	{
		wxString s1 = elemInput[i];
		wxStringTokenizer tokenizer(s1, " ");
		while (tokenizer.HasMoreTokens())
		{
			wxString token = tokenizer.GetNextToken();
			lineinput.Add(token);
		}
		ntoken = lineinput.GetCount();
		std::string e_id = lineinput[0].ToStdString();
		elemlist.push_back(e_id);
		std::string inc1 = lineinput[1].ToStdString();
		inc1.erase(std::remove(inc1.begin(), inc1.end(), '\t'), inc1.end());
		std::string  inc2 = lineinput[2].ToStdString();
		inc2.erase(std::remove(inc2.begin(), inc2.end(), '\t'), inc2.end());
		std::string esec = lineinput[3].ToStdString();
		esec.erase(std::remove(esec.begin(), esec.end(), '\t'), esec.end());
		std::string emat = lineinput[4].ToStdString();
		emat.erase(std::remove(emat.begin(), emat.end(), '\t'), emat.end());
		int iinc1, iinc2, iesec, iemat;

		iinc1 = nlist.Index(inc1);
		iinc2 = nlist.Index(inc2);
		L = abs(iinc1 - iinc2);
		if (ms < L)ms = L;
		iesec = seclist.Index(esec);
		iemat = matlist.Index(emat);
		double dircx, dircy, dircz, length;
		length=wxcoor[iinc1].distance(wxcoor[iinc2]);
		//auto data = wxcoor[iinc1].distance(wxcoor[iinc2]);
		//length = std::get<3>(data);
		dircx=(wxcoor[iinc1].x-wxcoor[iinc2].x)/length;
		dircy = (wxcoor[iinc1].y - wxcoor[iinc2].y) / length;
		dircz = (wxcoor[iinc1].z - wxcoor[iinc2].z) / length;
		//dircx = std::get<0>(data) / length;
		//dircy = std::get<1>(data) / length;
		//dircz = std::get<2>(data) / length;

		m_txt_out->WriteText(wxString::Format(wxT("%i\t %s\t  %s\t %s\t %s\t %i\t %i\t %i\t %i\t %f\t %f\t %f\t %f \n"), i, inc1, inc2, esec, emat, iinc1, iinc2, iesec, iemat, dircx, dircy, dircz, length));

		wxelement.push_back(Member{ e_id,inc1,inc2,esec,emat,dircx,dircy,dircz,length });
		lineinput.Clear();
	}//wxMessageBox(content);
	ms = ndf * (ms + 1);
	m_txt_out->WriteText(wxString::Format(wxT("%s\t %i \n"), "Bandwith= ", ms));
}
#pragma endregion wxElements

#pragma region ----wxSections

/*--------------------  wxSections  -------------------------------------------------------------------------*/
void Input::wxSections(wxString content, wxString attrv1)
{
	Section nsection;
	double x, y;
	std::string ucv = attrv1.ToStdString();
	//uconv = new unitConv();
	double scaleL;
	if (attrv1 == "default-value") scaleL = 1.0; else scaleL = Matrix_d::ToMeters(ucv);

	wxStringTokenizer tokenizer(content, "\n");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		sectionInput.Add(token);

	}
	nsec = sectionInput.GetCount();

	m_txt_out->WriteText("Number of Sections: " + wxString::Format(wxT("%i"), nsec) + "\n");
	for (int i = 0; i < nsec; i++)
	{
		wxString s1 = sectionInput[i];
		wxStringTokenizer tokenizer(s1, " ");
		while (tokenizer.HasMoreTokens())
		{
			wxString token = tokenizer.GetNextToken();
			lineinput.Add(token);
		}
		ntoken = lineinput.GetCount();
		seclist.push_back(lineinput[0]);

		std::string s5 = lineinput[0].ToStdString();
		std::string s6 = lineinput[1].ToStdString();
		x = wxAtof(lineinput[2]) * scaleL;
		y = atof(lineinput[3]) * scaleL;
		nsection = { s5, s6, x, y };

		wxsection.push_back(nsection);
		m_txt_out->WriteText(wxString::Format(wxT(" %i\t %s\t %s\t %.2f\t %.2f %.4f\t %.4f\n"), i, s5, s6, x, y, nsection.od, nsection.wth));
		x = 0.0; y = 0.0;
		lineinput.Clear();

	}
	//wxMessageBox(content);
}
#pragma endregion wxSections


#pragma region ----wxMaterial

/*--------------------------------  wxMaterial  ------------------------------------------------------------------*/
void Input::wxMaterial(wxString content, wxString attrv1, wxString attrv2)
{
	nMaterial nmaterial;
	std::string ucv = attrv1.ToStdString();
	std::string ucv2 = attrv2.ToStdString();
	//uconv = new unitConv();
	double scaleS, scaleden;
	if (attrv1 == "default-value") scaleS = 1.0; else scaleS = Matrix_d::ToNperM2(ucv);
	if (attrv2 == "default-value") scaleden = 1.0; else scaleden = Matrix_d::ToNperM3(ucv2);
	wxStringTokenizer tokenizer(content, "\n");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		matInput.Add(token);

	}
	nmat = matInput.GetCount();

	m_txt_out->WriteText("Number of Materials: " + wxString::Format(wxT("%i"), nmat) + "\n");
	for (int i = 0; i < nmat; i++)
	{
		wxString s1 = matInput[i];
		wxStringTokenizer tokenizer(s1, " ");
		while (tokenizer.HasMoreTokens())
		{
			wxString token = tokenizer.GetNextToken();
			lineinput.Add(token);
		}
		ntoken = lineinput.GetCount();
		matlist.push_back(lineinput[0]);

		std::string tabname = std::string(lineinput[0].mb_str());
		std::string mtname = std::string(lineinput[1].mb_str());
		if (mtname == "Steel")
		{
			//nmaterial = new nMaterial();
			wxmaterial.push_back(nMaterial());
			m_txt_out->WriteText(wxString::Format(wxT("%s\t %i\t %s \n"), tabname, i, mtname));

		}
		else
		{
			double Emat = wxAtof(lineinput[2]) * scaleS;
			double poisson = wxAtof(lineinput[3]);
			double denMat = wxAtof(lineinput[4]) * scaleden;
			nmaterial = { i,tabname,  mtname, Emat, denMat, poisson };
			wxmaterial.push_back(nmaterial);
			m_txt_out->WriteText(wxString::Format(wxT("%s %i\t %s %.2f\t %.2f\t %.2f\n"), tabname, i, mtname, Emat, denMat, poisson));
		}

		lineinput.Clear();
	}
	//wxMessageBox(content);
}
#pragma endregion wxMaterial

#pragma region ----wxBoundary

/*----------------------------------  wxBoundary  --------------------------------------------------------------------*/
void Input::wxBoundary(wxString content)
{
	lineinput.Clear();
	nBoundary nboundary ;
	wxStringTokenizer tokenizer(content, "\n");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		boundInput.Add(token);
	}
	nbound = boundInput.GetCount();

	m_txt_out->WriteText("Number of Boundary Nodes: " + wxString::Format(wxT("%i"), nbound) + "\n");

	for (int i = 0; i < nbound; i++)
	{
		int j;
		wxString s1 = boundInput[i];
		wxStringTokenizer tokenizer(s1, "   ");
		while (tokenizer.HasMoreTokens())
		{
			wxString token = tokenizer.GetNextToken();
			lineinput.Add(token);
		}
		ntoken = lineinput.GetCount();

		boundlist.push_back(lineinput[0]);
		std::string boundid = lineinput[0].ToStdString();
		boundid.erase(std::remove(boundid.begin(), boundid.end(), '\t'), boundid.end());
		std::string boundtype = lineinput[1].ToStdString();

		nboundary.setBound(boundid, boundtype, nlist);
		j = nboundary.ib[0];
		int j1 = nboundary.ib[1];
		int j2 = nboundary.ib[2];
		int j3 = nboundary.ib[3];
		wxboundary.push_back(nBoundary());
		m_txt_out->WriteText(wxString::Format(wxT("%s\t %i\t %s\t %i\t %i\t %i \n"), boundid, j, boundtype, j1, j2, j3));

		lineinput.Clear();
	}

	//wxMessageBox(content);
}

#pragma endregion wxBoundary
