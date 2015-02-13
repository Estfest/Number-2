//============================================================================
// Name        : Number.cpp
// Author      : Chris Estrerer
// Version     :
// Copyright   : Your copyright notice
// Description : Cell to Cell alignment
//============================================================================
//*******************************************INCLUDES****************************************************
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <cmath>
#include <math.h>
#include <iterator>
#include <algorithm>
#include <vector>
using namespace std;
//******************************************************************************************************

//****************************************CLASSES/STRUCTS***********************************************
struct point
{
	vector<double> points;
    int x;
    int y;
    double z;
    bool IsPlate = 0; // Flag to be set if combinePlates function sees a plate gap..
};

class feature
{
public:
	double thickness;
	char Type;
	double Hight;
	double HightDiff;
	int FNumb = 0;
	std::list<std::list<point> >::const_iterator Group;
	void PushHD(double HD) { HightDiff = HD;};
};
//******************************************************************************************************

//***************************************FUNCTION DECLARATION*******************************************
point get_values (string s);
void printPoints(std::list<point, std::allocator<point> > p);
void printHeights(std::list<feature> f);
void printGroups(std::list<std::list<point, std::allocator<point> > > g);
bool Compare( double a, double b);
std::list<std::list<point> > putintoGroups (std::list<point, std::allocator<point> > s );
std::list<feature> find_thickness (std::list<std::list<point, std::allocator<point> > > g); // Finds Thickness and Heights
std::list<std::list<point, std::allocator<point> > > combinePlates(std::list<std::list<point, std::allocator<point> > > g);
double find_max(std::list<std::list<point> >::const_iterator i);
//******************************************************************************************************
point get_values (string s){
    int tempX;
	int tempY;
	double tempZ;

	int pos = 0;
	string TempS;

	pos = s.find(",");	TempS = s.substr(0, pos);	pos++; s.erase(0,pos);	// removes the first coord from the string passed in.
	tempX = atol(TempS.c_str());//  converts string to int. Casts string to char* first.

	pos = s.find(",");	TempS = s.substr(0, pos);	pos++;	s.erase(0,pos);
	tempY = atol(TempS.c_str()); //

	TempS = s.substr(0);
	tempZ = atof(TempS.c_str());

	point p;	p.x = tempX;	p.y = tempY;	p.z = tempZ;

	return p;
};
// **********************************************MAIN***************************************************
int main() {
bool run = true;
int choice = 0, datachoice = 0;
string DataFileName;

	cout << "Start" << endl; // prints

	std::list<point> Points; // list of Points
	std::list<std::list<point> > Groups; //list of Groups (List of list of points)
	std::list<feature> Features; //list of Features

	cout << "Data File | 1. 10cm A4 | 2. Full B1 | 3. Half Stack | 4. Other" << endl; // Data Menu Prompt
	cin >> datachoice; // Get users choice
	switch(datachoice) //Data Menu Choice
	{
	case 1:
		DataFileName = "data_old.txt"; // 10 cm of A4.1
		break;
	case 2:
		DataFileName = "data_b1r.txt";	//Full profile of A4.1
		break;
	case 3:
		DataFileName = "data_cut.txt"; //Cut 'show' stack
		break;
	case 4:
		cout << "File Name? " << endl;
		cin >> DataFileName; 			//Get file name of other
		break;
	 }
	ifstream myfile (DataFileName.c_str()); // Read Data File
	if (myfile.is_open()) // if File is open
		  {
			string L; // TempLine
		    while (getline(myfile,L) ) // Parse a line from the file
		    {
		    	Points.push_back(get_values(L)); // Send the line to get_values function, push the returned point to the list
		    }
		  }
do // After the Data is parsed...
{
	cout << "Menu:"<<endl<<"1. Analyse Data"<<endl << "2. Print Analysis "<< endl <<"3. Print Heights "<< endl << "4. Print Groups  "<<endl<<"5. Print Thickness"<<endl<<"6. Quit" << endl; // Menu prompt
	cin >> choice;
	switch(choice)
	{
	case 1: // Analyse
		Groups = putintoGroups (Points); // Puts the points into Groups
		Groups = combinePlates(Groups);  // Removes the gap between the 2 halves of plates.
		Features = find_thickness (Groups); // finds thickness and maximum height of each group. Saves this data in a list of Features
		break;
	case 2: // Print
		printHeights(Features);
		printGroups(Groups);
		cout << Groups.size() << endl;
		break;
	case 3:
		printHeights(Features);
		break;
	case 4:
		printGroups(Groups);
		break;
	case 5:
		Features = find_thickness (Groups);
		break;
	case 6:
		run = false; //Breaks loop. Exits
		break;
	}
	cout << "********************************************" << endl << endl; // Visual cue to show the program is working
}while (run);
	return 0;
};
// *****************************************************************************************************
//*********************************************FUNCTIONS************************************************
void printPoints(	std::list<point, std::allocator<point> > s ){ // Prints List of Points Passed in
	list<point>::const_iterator i;

	for( i = s.begin(); i != s.end(); ++i){ // For every point in the list (For the iterator not equaling the end of list, increment)
		cout << i->x << " " << i->y << " " << std::fixed << i->z << endl; // Print to console the X , Y ,Z points to full digits.
	}
	cout << endl;
};
void printHeights(	std::list<feature> f ){ // Prints the Heights of the Features to a file
	list<feature>::const_iterator i;

	  ofstream HeightFile ("Heights.txt");
	  if (HeightFile.is_open())
	  {
			for( i = f.begin(); i != f.end(); ++i){
				HeightFile << i->Type << i->FNumb << " | " << std::fixed << i->Hight <<" | " << i->HightDiff << " | " << i->thickness << endl;
				//cout << i->Type << i->FNumb << "| " << std::fixed << i->Hight <<" | " << i->HightDiff << " | " << i->thickness << endl;
			}
			//cout << endl;
		  HeightFile.close();
	  }
	  else cout << "*************************Unable to open file*****************************";

};
void printGroups(std::list<std::list<point, std::allocator<point> > > g){ // Prints the Points in the list of Groups
	list<list<point> >::const_iterator i;
	list<point>::const_iterator p;
	  ofstream GroupFile ("Groups.txt");
	  if (GroupFile.is_open())
	  {
	  for( i = g.begin(); i != g.end(); ++i)
	  	  {
		  for( p = i->begin(); p != i->end(); ++p){
		  				GroupFile << std::fixed << p->x << "," << p->y << "," << p->z << "  " << p->IsPlate << endl;
		  				//cout << i->Type << i->FNumb << "| " << std::fixed << i->Hight <<" | " << i->HightDiff << " | " << i->thickness << endl;
		  			}
		GroupFile << "Next Group" << endl;
	  	  }
	  GroupFile.close();
	  }
	  else cout << "******************************Unable to open file****************************";
}
std::list<feature> find_thickness (std::list<std::list<point, std::allocator<point> > > g){ // Finds the Thickness of Groups
	std::list<feature> Features;
	list<list<point> >::const_iterator i;
	for( i = g.begin(); i != g.end(); ++i) //for every Node in Groups
{
				feature TempFeature;
				TempFeature.thickness = (i->front().y - i->back().y );
				if (TempFeature.thickness > 400 && i->front().z != 0){		// Greater than 300
					TempFeature.Type = 'O'; // Other
					TempFeature.Hight = find_max(i);
				}else if (TempFeature.thickness >= 200 && i->front().z != 0 /*&& i->front().IsPlate*/){         // Greater than 100, Z != 0
					TempFeature.Type = 'P'; // Plate
					TempFeature.Hight = find_max(i);
				}else if (TempFeature.thickness < 200 && i->front().z != 0) { // Less than or Equal to 100, Z != 0
					TempFeature.Type = 'F';// Frame
					TempFeature.Hight = find_max(i);
				}else{															//Neither
					TempFeature.Type = 'S'; // Space
					TempFeature.Hight = find_max(i);
				}
				TempFeature.Group = i;
				if (TempFeature.Type != 'S')
				Features.push_back(TempFeature);
}
	double TempFH = 0, TempPH = 0, hightDiff = 0;
	list<feature>::iterator f;
	int Pcount = 0, Fcount = 0;
for (f = Features.begin(); f != Features.end(); ++f)
{
		if (f->Type == 'P'){
			++Pcount;
			 hightDiff = (TempPH - f->Hight);
			f->HightDiff = (fabs(hightDiff));
			TempPH = f->Hight;
			f->FNumb = Pcount;
		}
		else if(f->Type == 'F'){
			++Fcount;
			 hightDiff = (TempFH - f->Hight);
			f->PushHD( fabs(hightDiff) );
			 TempFH = f->Hight;
			f->FNumb = Fcount;
		} else{

		}
		//if( hightDiff > 200){
			//cout << f->Type << f->FNumb<< "  " << f->Hight << "  Fail by  " << f->HightDiff << endl;
		//}
}

return Features;
}
double find_max(std::list<std::list<point> >::const_iterator s){ // Finds the Maximum Z Coord in the group
		list<point>::const_iterator i;
		double largest = s->begin()->z; // assign the first value
		for( i = s->begin(); i != s->end(); ++i){
	    if (i->z > largest) {
	      largest= i->z;
	    }
		}
		return largest;
}
bool Compare(double a, double b){ // Compares two points to each other to figure out if they form an edge
	if( (a == 0 && b != 0) || (b==0 && a != 0) ){ // Do the Points for from High to Low or Low to High?
	return false;
	}else{
		return true;
	}
};
std::list<std::list<point> > putintoGroups (	std::list<point, std::allocator<point> > s ) { // Puts Points into Groups
std::list<std::list<point> > Groups;
list <point>::const_iterator i;
for ( i = s.begin(); i != s.end();) // For every Point in the list
	{
	std::list<point> TempSubList;
	TempSubList.push_back(*i); // Pn
	i++;						//P(n+1)
	while(Compare(i->z , TempSubList.back().z)) // Add points to the TempList until an edge is found
		{
		TempSubList.push_back(*i);				// Push Pn+1
		i++;
		}
	Groups.push_back(TempSubList); // Once an edge is found Add the TempList of points to the list of Groups.
	TempSubList.clear();
	}

return Groups; // Return the List of Groups.
};
std::list<std::list<point, std::allocator<point> > > combinePlates(std::list<std::list<point, std::allocator<point> > > g) { // Takes in the list of groups and removes Plate gaps
	list<list<point> >::iterator i;
	for( i = g.begin(); i != g.end(); ++i){ //for every Node in Groups
		if ((i->size() <= 4 && i->front().z == 0 && i->back().z == 0 && i->back().y != 0 ) ){ // if the group has 3 or less points and the first point z = 0
			list<list<point> >::iterator  n,p,m;
			p._M_node = i._M_node->_M_prev;
			n._M_node = i._M_node->_M_next;
			p->splice(p->end(), *n);
			p->begin()->IsPlate = 1;
			g.erase(i++);
			}
			}
	return g;
}
//******************************************************************************************************
