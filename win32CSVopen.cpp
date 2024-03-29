#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
//#include <memory>
#include "StudentRec.h"
#include "StudentWindowCollection.h"


//last update - 15.01.2024 - adding copy constructor to StudentRec object in order to be able to sort and swap
//update - 12.01.2024 - added SORT button graphics. now need to design the logic
//update - 11.01.2024 - working towards adding a SORT button - by grade.
//update - 26.02.2023 - added check for empty vector
//It's beginning to look better. Need to pack window handles into a vector and loop through the vector to move them all. Done.
//Now I need to implement horizontal scrolling across the output windows as well.




//lresult callback prototype
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void swap(std::vector<StudentRec>& v, int x, int y);

void quicksort(std::vector<StudentRec>& vec, double L, double R);


//window handles
HWND hMainWindow;
HWND hCanvasWindow;
HINSTANCE hMainInstance;

HWND hLblOutputId;
HWND hLblOutputFirstName;
HWND hLblOutputLastName;
HWND hLblOutputAge;
HWND hLblOutputPhone;
HWND hLblOutputGpa;

HWND hTxtInput; //1000
HWND hButton; //1001
HWND hButtonDel; //1002
HWND hButtonSort; //1003
HWND hButtonReset; //1004

CHAR s_text_1[]{ "Some text.." };
int posX = 0;
int posY = 0;

//vector to hold windows collection of student data
std::vector<StudentWindowCollection> windows;


#define IDC_TEXTBOX 1000
#define IDC_BUTTON 1001
#define IDC_BUTTON_DEL 1002
#define IDC_BUTTON_SORT_HTL 1003
#define IDC_BUTTON_RESET 1004





//call to winmain - equivalent of main for win32 environments
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{


	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = TEXT("NiceWindowsApp");
	if (!RegisterClass(&wc))
		return 1;

	//added ws_hscroll and ws_vscroll, but they are not working without additional control message flows
	hMainWindow = CreateWindow(wc.lpszClassName, TEXT("My Windows Application"), WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE,
		0, 0, 640, 480, 0, 0, hInstance, NULL);
	//hCanvasWindow = CreateWindowEx(0, "STATIC", "aaa", WS_CHILD | WS_VISIBLE, posX, 0, 8000, 4800, hMainWindow, NULL, hInstance, NULL);


	hMainInstance = wc.hInstance;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
//callback definition
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int offset = 0;
	std::wstring input;
	
	//std::vector<HWND> wndws;
	switch (message) {

	case WM_CREATE:
	{
		hMainWindow = hWnd; //aaa will be visible
		hCanvasWindow = CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_VISIBLE, posX, 0, 8000, 4800, hMainWindow, NULL, NULL, NULL);
		//hCanvasWindow = hWnd; //aaa will not be visible
		hTxtInput = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), s_text_1,
			WS_VISIBLE | WS_CHILD | ES_LEFT, 50, 50, 400, 25, hWnd,
			(HMENU)IDC_TEXTBOX, hMainInstance, NULL);
		
		//get info from csv into windows - button to trigger
		hButton = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Press Me!"), WS_VISIBLE | WS_CHILD | WM_COPY | ES_LEFT, 500, 50, 400, 25, hWnd,
			(HMENU)IDC_BUTTON, hMainInstance, NULL);

		//delete last series of windows - button to trigger
		hButtonDel = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Delete last!"), WS_VISIBLE | WS_CHILD | WM_COPY | ES_LEFT, 500, 100, 400, 25, hWnd,
			(HMENU)IDC_BUTTON_DEL, hMainInstance, NULL);

		//LOWEST BUTTON FEASIBLE - OTHER BUTTONS WOULD NEED TO BE TO THE RIGHT - moved to the right
		//sort from highest to lowest grades - button to trigger
		hButtonSort = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Sort list!"), WS_VISIBLE | WS_CHILD | WM_COPY | ES_LEFT, 950, 50, 400, 25, hWnd,
			(HMENU)IDC_BUTTON_DEL, hMainInstance, NULL);

	
		
		//wndws.push_back(hButton);

		//horizontal scrolling set up
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.nPos = 0;
		si.nMax = 800;
		si.nPage = 400;
		si.fMask = SIF_ALL;
		si.nMin = 0;
		SetScrollInfo(hCanvasWindow, SB_HORZ, &si, TRUE);

		//vertical scrolling set up
		SCROLLINFO sj;
		sj.cbSize = sizeof(sj);
		sj.nPos = 0;
		sj.nMax = 1200;
		sj.nPage = 400;
		sj.fMask = SIF_ALL;
		sj.nMin = 0;
		SetScrollInfo(hCanvasWindow, SB_VERT, &sj, TRUE);

		break;
	}
	
	case WM_HSCROLL: {
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		GetScrollInfo(hCanvasWindow, SB_HORZ, &si);
		switch (LOWORD(wParam)) {
		case SB_LINELEFT: {
			si.nPos -= 5;
		}break;
		case SB_LINERIGHT: {
			si.nPos += 5;
		}break;
		case SB_THUMBTRACK: {
			si.nPos = HIWORD(wParam);
		}break;
		};
		SetScrollInfo(hCanvasWindow, SB_HORZ, &si, TRUE);
		posX = -si.nPos;
		MoveWindow( 
			hCanvasWindow,
			posX,
			posY,
			8000,
			4800,
			TRUE
		);
		break;
	}
	case WM_VSCROLL: {
		SCROLLINFO sj;
		sj.cbSize = sizeof(sj);
		GetScrollInfo(hCanvasWindow, SB_VERT, &sj);
		switch (LOWORD(wParam)) {
		case SB_LINEDOWN: {
			sj.nPos += 5;
		}break;
		case SB_LINEUP: {
			sj.nPos -= 5;
		}break;
		case SB_THUMBTRACK: {
			sj.nPos = HIWORD(wParam);
		}break;
		};
		SetScrollInfo(hCanvasWindow, SB_VERT, &sj, TRUE);
		posY = -sj.nPos;
		MoveWindow(
			hCanvasWindow,
			posX,
			posY,
			8000,
			4800,
			TRUE
		);
		break;
	}



		//press button
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDC_BUTTON)
			{

				input.resize(GetWindowTextLengthW(hTxtInput));
				GetWindowTextW(hTxtInput, input.data(), input.size() + 1);



				/*hLblOutput = CreateWindowExW(WS_EX_STATICEDGE, TEXT(L"EDIT"), L"", WS_VISIBLE | WS_CHILD | ES_READONLY |
												ES_LEFT, 50, 200 + offset * 26, 800, 25, hCanvasWindow,
												(HMENU)IDC_TEXTBOX, hMainInstance, NULL);
				SetWindowTextW(hLblOutput, input.data());*/


				std::ifstream inputFile;
				inputFile.open(input.data());

				std::string line = "";
				std::vector<StudentRec> students;
				
				while (std::getline(inputFile, line))
				{

					std::stringstream inputString(line);

					std::string studentId;
					std::string lastName;
					std::string firstName;
					int age;
					std::string phone;
					double gpa;
					std::string tempString;



					getline(inputString, studentId, ',');
					getline(inputString, lastName, ',');
					getline(inputString, firstName, ',');
					//getting the age
					getline(inputString, tempString, ',');
					age = atoi(tempString.c_str());
					getline(inputString, phone, ',');
					//getting the GPA
					tempString = "";
					getline(inputString, tempString);
					gpa = atof(tempString.c_str());

					StudentRec student(studentId, firstName, lastName, age, phone, gpa);
					students.push_back(student);
					line = "";
				}

				//loop through students array and output info into windows

				for (const auto& student : students)
				{

					std::string StdId = student.Id;
					std::string StdFirstName = student.FirstName;
					std::string StdLastName = student.LastName;
					int StdAge = student.Age;
					std::string StdAgeString = std::to_string(StdAge);
					std::string StdPhone = student.PhoneNumber;
					double StdGpa = student.Gpa;
					std::string StdGpaString = std::to_string(StdGpa);
					

				


					hLblOutputId = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), StdId.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
							ES_LEFT, 50, 200 + offset * 26, 100, 25, hCanvasWindow,
							(HMENU)IDC_TEXTBOX, hMainInstance, NULL);

					hLblOutputFirstName = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), StdFirstName.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
						ES_LEFT, 250, 200 + offset * 26, 100, 25, hCanvasWindow,
						(HMENU)IDC_TEXTBOX, hMainInstance, NULL);

					hLblOutputLastName = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), StdLastName.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
						ES_LEFT, 360, 200 + offset * 26, 100, 25, hCanvasWindow,
						(HMENU)IDC_TEXTBOX, hMainInstance, NULL);

					hLblOutputAge = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), StdAgeString.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
						ES_LEFT, 470, 200 + offset * 26, 100, 25, hCanvasWindow,
						(HMENU)IDC_TEXTBOX, hMainInstance, NULL);

					hLblOutputPhone = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), StdPhone.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
						ES_LEFT, 580, 200 + offset * 26, 100, 25, hCanvasWindow,
						(HMENU)IDC_TEXTBOX, hMainInstance, NULL);

					hLblOutputGpa = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), StdGpaString.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
						ES_LEFT, 700, 200 + offset * 26, 100, 25, hCanvasWindow,
						(HMENU)IDC_TEXTBOX, hMainInstance, NULL);

					StudentWindowCollection Swc(hLblOutputId, hLblOutputFirstName, hLblOutputLastName, hLblOutputAge, hLblOutputPhone, hLblOutputGpa);
					windows.push_back(Swc);


					

					++offset;

				}



			}
			

			else if (LOWORD(wParam) == IDC_BUTTON_DEL)
			{
			if (windows.empty()) {}
			
			else
				{
			
					StudentWindowCollection Swc = windows.back();
			
					DestroyWindow(Swc.IdWindow);
					DestroyWindow(Swc.FirstNameWindow);
					DestroyWindow(Swc.LastNameWindow);
					DestroyWindow(Swc.AgeWindow);
					DestroyWindow(Swc.PhoneNumberWindow);
					DestroyWindow(Swc.GpaWindow);
					windows.pop_back();

			
				}
			}

			//else if - ALL THE OTHER BUTTONS - SORT AND RESET

			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//function definitions - swap and quicksort
void swap(std::vector<StudentRec>& v, int x, int y) {
	//need all parameters, not just Gpa - swap is going to affect the whole class
	double temp = v[x].Gpa;
	v[x].Gpa = v[y].Gpa;
	v[y].Gpa = temp;

	void quicksort(std::vector<StudentRec>&vec, int L, int R) {
		int i, j, mid;  //piv;
		double piv;
		i = L;
		j = R;
		mid = L + (R - L) / 2;
		piv = vec[mid].Gpa;

		while (i<R || j>L) {
			while (vec[i].Gpa < piv)
				i++;
			while (vec[j].Gpa > piv)
				j--;

			if (i <= j) {
				swap(vec, i, j); //error=swap function doesnt take 3 arguments
				i++;
				j--;
			}
			else {
				if (i < R)
					quicksort(vec, i, R);
				if (j > L)
					quicksort(vec, L, j);
				return;
			}
		}
	}


*******************************************************************************************************************************************************************************************
StudentRec.h 
********************************************************************************************************************************************************************************************

#ifndef STUDENTREC_H
#define STUDENTREC_H
#include <string>

class StudentRec
{
public:
	std::string Id;
	std::string FirstName;
	std::string LastName;
	std::string PhoneNumber;
	int Age;
	double Gpa;

	
		StudentRec(std::string id, std::string firstName, std::string lastName, int age, std::string phoneNumber, double gpa);
		StudentRec(StudentRec &t);
		
	

	


};
#endif


********************************************************************************************************************************************************************************************
StudentRec.cpp
********************************************************************************************************************************************************************************************

#include "StudentRec.h"

StudentRec::StudentRec(std::string id, std::string firstName, std::string lastName, int age, std::string phoneNumber, double gpa)
	{
		Id = id;
		FirstName = firstName;
		LastName = lastName;
		PhoneNumber = phoneNumber;
		Age = age;
		Gpa = gpa;

	}
StudentRec::StudentRec(StudentRec &t)
{
	Id = t.Id;
	FirstName = t.FirstName;
	LastName = t.LastName;
	Age = t.Age;
	PhoneNumber = t.PhoneNumber;
	Gpa = t.Gpa;
}
	
	
**********************************************************************************************************************************************************************************************
StudentWindowCollection.h
***********************************************************************************************************************************************************************************************

#ifndef STUDENTWINDOWCOLLECTION_H
#define STUDENTWINDOWCOLLECTION_H
#include <windows.h>


class StudentWindowCollection
{
public:

	StudentWindowCollection(HWND IdWindow,
		int IdWindowXCoord,
		int IdWindowYCoord,
		int IdWindowHeight,
		int IdWindowWidth,
		HWND FirstNameWindow,
		int FirstNameWindowXCoord,
		int FirstNameWindowYCoord,
		int FirstNameWindowHeight,
		int FirstNameWindowWidth,
		HWND LastNameWindow,
		int LastNameWindowXCoord,
		int LastNameWindowYCoord,
		int LastNameWindowHeight,
		int LastNameWindowWidth,
		HWND PhoneNumberWindow,
		int PhoneNumberWindowXCoord,
		int PhoneNumberWindowYCoord,
		int PhoneNumberWindowHeight,
		int PhoneNumberWindowWidth,
		HWND AgeWindow,
		int AgeWindowXCoord,
		int AgeWindowYCoord,
		int AgeWindowHeight,
		int AgeWindowWidth,
		HWND GpaWindow,
		int GpaWindowXCoord,
		int GpaWindowYCoord,
		int GpaWindowHeight,
		int GpaWindowWidth
		);

	StudentWindowCollection(
		int IdWindowXCoord,
		int IdWindowYCoord,
		int IdWindowHeight,
		int IdWindowWidth,
		int FirstNameWindowXCoord,
		int FirstNameWindowYCoord,
		int FirstNameWindowHeight,
		int FirstNameWindowWidth,
		int LastNameWindowXCoord,
		int LastNameWindowYCoord,
		int LastNameWindowHeight,
		int LastNameWindowWidth,
		int PhoneNumberWindowXCoord,
		int PhoneNumberWindowYCoord,
		int PhoneNumberWindowHeight,
		int PhoneNumberWindowWidth,
		int AgeWindowXCoord,
		int AgeWindowYCoord,
		int AgeWindowHeight,
		int AgeWindowWidth,
		int GpaWindowXCoord,
		int GpaWindowYCoord,
		int GpaWindowHeight,
		int GpaWindowWidth
		);






	StudentWindowCollection(HWND idWindow, int idWindowXCoord, HWND firstNameWindow, int firstNameWindowXCoord,
							HWND lastNameWindow, int lastNameWindowXCoord, HWND ageWindow, int ageWindowXCoord, HWND phoneNumberWindow, int phoneNumberWindowXCoord,
							HWND gpaWindow, int gpaWindowXCoord);

	StudentWindowCollection(HWND idWindow, HWND firstNameWindow, HWND lastNameWindow, HWND ageWindow, HWND phoneNumberWindow, HWND gpaWindow);

	StudentWindowCollection(int idWindowXCoord, int firstNameWindowxCoord, int lastNameWindowXCoord, int ageWindowXCoord, int phoneNumberWindowXCoord, int gpaWindowXCoord);

	StudentWindowCollection();


	HWND IdWindow;
	int IdWindowXCoord;
	int IdWindowYCoord;
	int IdWindowHeight;
	int IdWindowWidth;
	HWND FirstNameWindow;
	int FirstNameWindowXCoord;
	int FirstNameWindowYCoord;
	int FirstNameWindowHeight;
	int FirstNameWindowWidth;
	HWND LastNameWindow;
	int LastNameWindowXCoord;
	int LastNameWindowYCoord;
	int LastNameWindowHeight;
	int LastNameWindowWidth;
	HWND PhoneNumberWindow;
	int PhoneNumberWindowXCoord;
	int PhoneNumberWindowYCoord;
	int PhoneNumberWindowHeight;
	int PhoneNumberWindowWidth;
	HWND AgeWindow;
	int AgeWindowXCoord;
	int AgeWindowYCoord;
	int AgeWindowHeight;
	int AgeWindowWidth;
	HWND GpaWindow;
	int GpaWindowXCoord;
	int GpaWindowYCoord;
	int GpaWindowHeight;
	int GpaWindowWidth;

};
#endif





************************************************************************************************************************************************************************************************
StudentWindowCollection.cpp
************************************************************************************************************************************************************************************************

#include "StudentWindowCollection.h"


StudentWindowCollection::StudentWindowCollection(HWND idWindow,
	int idWindowXCoord,
	int idWindowYCoord,
	int idWindowHeight,
	int idWindowWidth,
	HWND firstNameWindow,
	int firstNameWindowXCoord,
	int firstNameWindowYCoord,
	int firstNameWindowHeight,
	int firstNameWindowWidth,
	HWND lastNameWindow,
	int lastNameWindowXCoord,
	int lastNameWindowYCoord,
	int lastNameWindowHeight,
	int lastNameWindowWidth,
	HWND phoneNumberWindow,
	int phoneNumberWindowXCoord,
	int phoneNumberWindowYCoord,
	int phoneNumberWindowHeight,
	int phoneNumberWindowWidth,
	HWND ageWindow,
	int ageWindowXCoord,
	int ageWindowYCoord,
	int ageWindowHeight,
	int ageWindowWidth,
	HWND gpaWindow,
	int gpaWindowXCoord,
	int gpaWindowYCoord,
	int gpaWindowHeight,
	int gpaWindowWidth
	)
{
	IdWindow = idWindow;
	FirstNameWindow = firstNameWindow;
	FirstNameWindowXCoord = firstNameWindowXCoord;
	LastNameWindow = lastNameWindow;
	LastNameWindowXCoord = lastNameWindowXCoord;
	AgeWindow = ageWindow;
	AgeWindowXCoord = ageWindowXCoord;
	PhoneNumberWindow = phoneNumberWindow;
	PhoneNumberWindowXCoord = phoneNumberWindowXCoord;
	GpaWindow = gpaWindow;
	GpaWindowXCoord = gpaWindowXCoord;

	IdWindowXCoord = idWindowXCoord;
	FirstNameWindowXCoord = firstNameWindowXCoord;
	LastNameWindowXCoord = lastNameWindowXCoord;
	AgeWindowXCoord = ageWindowXCoord;
	PhoneNumberWindowXCoord = phoneNumberWindowXCoord;
	GpaWindowXCoord = gpaWindowXCoord;

	IdWindowYCoord = idWindowYCoord;
	FirstNameWindowYCoord = firstNameWindowYCoord;
	LastNameWindowYCoord = lastNameWindowYCoord;
	AgeWindowYCoord = ageWindowYCoord;
	PhoneNumberWindowYCoord = phoneNumberWindowYCoord;
	GpaWindowYCoord = gpaWindowYCoord;

	IdWindowHeight = idWindowHeight;
	FirstNameWindowHeight = firstNameWindowHeight;
	LastNameWindowHeight = lastNameWindowHeight;
	AgeWindowHeight = ageWindowHeight;
	PhoneNumberWindowHeight = phoneNumberWindowHeight;
	GpaWindowHeight = gpaWindowHeight;

	IdWindowWidth = idWindowWidth;
	FirstNameWindowWidth = firstNameWindowWidth;
	LastNameWindowWidth = lastNameWindowWidth;
	AgeWindowWidth = ageWindowWidth;
	PhoneNumberWindowWidth = phoneNumberWindowWidth;
	GpaWindowWidth = gpaWindowWidth;


}

	StudentWindowCollection::StudentWindowCollection(
		int idWindowXCoord,
		int idWindowYCoord,
		int idWindowHeight,
		int idWindowWidth,
		int firstNameWindowXCoord,
		int firstNameWindowYCoord,
		int firstNameWindowHeight,
		int firstNameWindowWidth,
		int lastNameWindowXCoord,
		int lastNameWindowYCoord,
		int lastNameWindowHeight,
		int lastNameWindowWidth,
		int phoneNumberWindowXCoord,
		int phoneNumberWindowYCoord,
		int phoneNumberWindowHeight,
		int phoneNumberWindowWidth,
		int ageWindowXCoord,
		int ageWindowYCoord,
		int ageWindowHeight,
		int ageWindowWidth,
		int gpaWindowXCoord,
		int gpaWindowYCoord,
		int gpaWindowHeight,
		int gpaWindowWidth
		)
	{
		IdWindowXCoord = idWindowXCoord;
		FirstNameWindowXCoord = firstNameWindowXCoord;
		LastNameWindowXCoord = lastNameWindowXCoord;
		AgeWindowXCoord = ageWindowXCoord;
		PhoneNumberWindowXCoord = phoneNumberWindowXCoord;
		GpaWindowXCoord = gpaWindowXCoord;

		IdWindowYCoord = idWindowYCoord;
		FirstNameWindowYCoord = firstNameWindowYCoord;
		LastNameWindowYCoord = lastNameWindowYCoord;
		AgeWindowYCoord = ageWindowYCoord;
		PhoneNumberWindowYCoord = phoneNumberWindowYCoord;
		GpaWindowYCoord = gpaWindowYCoord;

		IdWindowHeight = idWindowHeight;
		FirstNameWindowHeight = firstNameWindowHeight;
		LastNameWindowHeight = lastNameWindowHeight;
		AgeWindowHeight = ageWindowHeight;
		PhoneNumberWindowHeight = phoneNumberWindowHeight;
		GpaWindowHeight = gpaWindowHeight;

		IdWindowWidth = idWindowWidth;
		FirstNameWindowWidth = firstNameWindowWidth;
		LastNameWindowWidth = lastNameWindowWidth;
		AgeWindowWidth = ageWindowWidth;
		PhoneNumberWindowWidth = phoneNumberWindowWidth;
		GpaWindowWidth = gpaWindowWidth;
}









StudentWindowCollection::StudentWindowCollection(HWND idWindow, int idWindowXCoord, HWND firstNameWindow, int firstNameWindowXCoord,
	HWND lastNameWindow, int lastNameWindowXCoord, HWND ageWindow, int ageWindowXCoord, HWND phoneNumberWindow, int phoneNumberWindowXCoord,
	HWND gpaWindow, int gpaWindowXCoord)
{

	IdWindow = idWindow;
	IdWindowXCoord = idWindowXCoord;
	FirstNameWindow = firstNameWindow;
	FirstNameWindowXCoord = firstNameWindowXCoord;
	LastNameWindow = lastNameWindow;
	LastNameWindowXCoord = lastNameWindowXCoord;
	AgeWindow = ageWindow;
	AgeWindowXCoord = ageWindowXCoord;
	PhoneNumberWindow = phoneNumberWindow;
	PhoneNumberWindowXCoord = phoneNumberWindowXCoord;
	GpaWindow = gpaWindow;
	GpaWindowXCoord = gpaWindowXCoord;
	
}




StudentWindowCollection::StudentWindowCollection(HWND idWindow, HWND firstNameWindow, HWND lastNameWindow, HWND ageWindow, HWND phoneNumberWindow, HWND gpaWindow)
{
	IdWindow = idWindow;
	FirstNameWindow = firstNameWindow;
	LastNameWindow = lastNameWindow;
	PhoneNumberWindow = phoneNumberWindow;
	AgeWindow = ageWindow;
	GpaWindow = gpaWindow;

}

StudentWindowCollection::StudentWindowCollection(int idWindowXCoord, int firstNameWindowXCoord, int lastNameWindowXCoord, int ageWindowXCoord, int phoneNumberWindowXCoord, int gpaWindowXCoord)
{
	IdWindowXCoord = idWindowXCoord;
	FirstNameWindowXCoord = firstNameWindowXCoord;
	LastNameWindowXCoord = lastNameWindowXCoord;
	AgeWindowXCoord = ageWindowXCoord;
	PhoneNumberWindowXCoord = phoneNumberWindowXCoord;
	GpaWindowXCoord = gpaWindowXCoord;

}

StudentWindowCollection::StudentWindowCollection() {}




