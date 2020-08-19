#pragma once

#include <wx/wx.h>
#include  <wx/xml/xml.h>
#include  <wx/tokenzr.h>
#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>


constexpr auto PI = 3.14149;

static int  nn, ne, nb, nmat, nsec, ntoken, nbound, ndf, nne, ndfel, ms, N;

struct Node
{
	std::string nodeid;
	double x = 0.0, y = 0.0, z = 0.0;

	double distance(struct Node node) {
		double length = sqrt(pow((this->x - node.x), 2) + pow((this->y - node.y), 2) + pow((this->z - node.z), 2));
		return length;
	}

};

struct Section
{
	std::string sectionid;
	std::string sectiontype;
	double od = 0.0, wth = 0.0;
	int nsec = 14;
	double W = 0.0, A = 0.0, Ix = 0.0, Zx = 0.0, Sx = 0.0, rx = 0.0, Iy = 0.0, Zy = 0.0, Sy = 0.0, ry = 0.0, J = 0.0;

	void secTube()
	{
		double id = od - 2 * wth;
		double area = 0.25 * PI * (od * od - id * id);
		double iner = PI * (od * od * od * od - id * id * id * id) / 64;
		double s = 2 * iner / od;
		double r = sqrt(iner / area);
		A = area;
		A = area;
		Ix = iner;
		Iy = iner;
		J = 2 * iner;
		Sx = s;
		Sy = s;
		rx = r;
		ry = r;
	}
	void Rectangular()
	{
		double b = wth;
		double d = od;
		double area = d * b;
		double inerx = b * pow(d, 3) / 12;
		double inery = d * pow(b, 3) / 12;
		double sx = 2 * inerx / d;
		double sy = 2 * inery / b;
		double rx = sqrt(inerx / area);
		double ry = sqrt(inery / area);
		double beta = (1 / 3) - 0.21 * (b / d) * (1 - pow(b / d, 4) / 12);
		double J = beta * pow(b, 3) * d;
		A = area;
		A = area;
		Ix = inerx;
		Iy = inery;//  iner;
		this->J = J;//2 * iner;
	}
	void AISC(const char* wfname) {

		try
		{
			//insert parameter
			//const char* wfname = "W40X503";
			//const char* wfname = "W40X503";
			// Open a database file
			SQLite::Database    db("data/aisc_shapes_v15_US.db3");

			// Compile a SQL query, containing one parameter (index 1)
			SQLite::Statement   query(db, "SELECT * FROM shapesv15_US WHERE EDI=?");

			// Bind the string value wfname to the first parameter of the SQL query
			query.bind(1, wfname);

			// Loop to execute the query step by step, to get rows of result
			while (query.executeStep())
			{
				int ncol = query.getColumnCount();

				W = atof(query.getColumn(query.getColumnIndex("W")));
				A = atof(query.getColumn(query.getColumnIndex("A")));
				Ix = atof(query.getColumn(query.getColumnIndex("Ix")));
				Zx = atof(query.getColumn(query.getColumnIndex("Zx")));
				Sx = atof(query.getColumn(query.getColumnIndex("Sx")));
				rx = atof(query.getColumn(query.getColumnIndex("rx")));
				Iy = atof(query.getColumn(query.getColumnIndex("Iy")));
				Zy = atof(query.getColumn(query.getColumnIndex("Zy")));
				Sy = atof(query.getColumn(query.getColumnIndex("Sy")));
				ry = atof(query.getColumn(query.getColumnIndex("ry")));
				J = atof(query.getColumn(query.getColumnIndex("J")));

				std::cout << "W: " << W << std::endl;
				std::cout << "A: " << A << std::endl;
				std::cout << "Ix: " << Ix << std::endl;
			}

		}
		catch (std::exception& e)
		{
			std::cout << "exception: " << e.what() << std::endl;
		}
	}
};

struct Member {
	std::string memberid;
	std::string inc_j1; std::string inc_j2; std::string section; std::string material;
	double d = 0.0, cx = 0.0, cy = 0.0, cz = 0.0;
};

struct nMaterial
{

	//nMaterial(std::string tabname, int matid, std::string matname, double Ematerial, double poisson, double denMaterial);
	//private:
	int matid=0;
	std::string matname="";
	std::string mtablename="";
	double Ematerial=0.0;
	double poisson=0.0;
	double denMaterial=0.0;
};

struct nBoundary
{
		int ib[4] = {-1,0,0,0};
		wxString nodebid = "N/A";
		wxString nbtype = "ENCASTRE";

	
		void setBound(wxString nbid, wxString ntype, wxArrayString nlist) 
		{
			int i = 0;
			this->nodebid = nbid;
			this->nbtype = ntype;

			ib[0] = nlist.Index(nodebid);
			if (nbtype == "ENCASTRE")	i = 1;
			if (nbtype == "HINGE")		i = 2;
			if (nbtype == "HSLIDEX")	i = 3;
			if (nbtype == "HSLIDEY")	i = 4;
			enum StringValues { ENCASTRE = 1, HINGE = 2, HSLIDEX = 3, HSLIDEY = 4 };
			switch (i)
			{
			case ENCASTRE:
				ib[1] = 0; ib[2] = 0; ib[3] = 0;
				break;
			case HINGE:
				ib[1] = 0; ib[2] = 0; ib[3] = 1;
			case HSLIDEX:
				ib[1] = 1; ib[2] = 0; ib[3] = 0;
				break;
			case HSLIDEY:
				ib[1] = 0; ib[2] = 1; ib[3] = 0;
				break;
			default:
				ib[1] = 0; ib[2] = 0; ib[3] = 0;
				break;
			}
	}

};

class Input
{
public:
	Input(wxString filein);
	void InputXml(wxString filein);
	int wxStructure(wxString tagname);
	void wxNodes(wxString content, wxString attrv1);
	void wxElements(wxString content);
	void wxSections(wxString content, wxString attrv1);
	void wxMaterial(wxString content, wxString attrv1, wxString attrv2);
	void wxBoundary(wxString content);
	//static int  nn, ne, nb, nmat, nsec, ntoken, nbound, ndf , nne, ndfel, ms, N;
	
private:

	/*---------------- Array of objects-----------------------------------*/
	wxArrayString nodeInput;
	wxArrayString sectionInput;
	wxArrayString matInput;
	wxArrayString boundInput;
	wxArrayString elemInput;
	/*----------------------------------------------------------------------*/
	wxString s1;
	wxString nname;
	/*--------------------------list of each object components names*/
	std::vector<Node> wxcoor;
	std::vector<nMaterial> wxmaterial;
	std::vector<Section> wxsection;
	std::vector<nBoundary> wxboundary;
	std::vector<Member> wxelement;
	wxArrayString nlist;
	wxArrayString boundlist;
	wxArrayString matlist;
	wxArrayString seclist;
	wxArrayString elemlist;
	/*----------------------list of names*/
	wxArrayString lineinput;
	wxXmlDocument* doc=nullptr;			/*Constructor for new wxXmlDocument */
	wxString filein;
	wxString attrvalue1;
	wxString attrvalue2;

	
};

