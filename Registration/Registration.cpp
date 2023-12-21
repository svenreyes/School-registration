// Program to implement a class registration system
// Program to implement a class registration system

/*
* Nathaniel Ramirez
* Sven reyes
* Talal Elhitu
* Kenneth Rouse
* Philip Teague
* Wesley Matusik
*/
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int STUIDSIZE = 5;		// Maximum size for a student ID
const int COURSEIDSIZE = 9;		// Maximum size for a course ID

const char STUFILE[] = "students.txt";
const char COURSEFILE[] = "courses.txt";

// Structure used to define a course registration
struct Course
{
	char id[COURSEIDSIZE];
	int section;
	int credit;
};

// Type definition to define the data type for the list data
typedef Course ListItemType;

// Node structure
struct Node
{
	ListItemType item;
	Node* next;
};

// Structure used to associate a student with the classes they are taking
struct Student
{
	char id[STUIDSIZE];
	Node* courses;
};

//Function Declaration starts here

//This function takes a reference to an integer to store the number of students
//This function reads the students file and counts the number of students in the file, makes the dynamic array using the numStu variable, and then fills the dynamic array with
//the students, it then return the array as a student pointer to allow the array to be used outside of this function
Student* studentGet(int& numStu);

//this function takes the student array and the number of students
//this function reads the course file and builds a linked list of each students schedule utilizing markers in the file to know were the one schedule begins and on ends
//this function creates the list from back to front while keeping the courses in the correct order
//uses the number of students to keep array reading and writing in the bounds of the array
void readCourses(Student stuList[], int numStu);

//this function takes the student array and the number of students
//this function goes through the student array and writes down the courses of each student into the courses file to save any changes to a students schedule
//by this program, and uses the number of students to keep the array reading in the bounds of the array
void saveChanges(Student stuList[], int numStu);

// Function to find course for selected student and add them to the schedule
// student - Reference to the selected Student structure
void findCourses(Student stuList[], int stuNum);

// Function to add a new course to the beginning of the linked list
// courseList - Reference to the pointer pointing to the head of the linked list
// newCourse - Reference to the Course structure representing the new course to be added
void addCourse(Node*& courseList, const Course& newCourse);

// Function to select a student by ID
// stuList - Array of students
// stuNum - Number of students in the array
// selectedStudentIndex - Reference to the index of the selected student in the array
void selectID(Student stuList[], int& stuNum, int& selectedStudentIndex);

// Function to delete the linked list in order to avoid memory leaks
// stuList - Array of students
// stuNum - Number of students in the array
void cleanUP(Student stuList[], int stuNum);

// Function to display the schedule of a selected student
// stuList - Array of students
// selectedIndex - Index of the selected student in the array
// courseNum - Reference to the number of courses (updated in the function)
void displaySchedule(Student stuList[], int selectedIndex, int& courseNum);

//this function works to find the courses a student is taking, display them and let you choose which 
// you want removed from the list
void removeCourse(Student stuList[], int index);

// Binary search to find a student by ID in a sorted array of students
// stuList - Array of students
// left - Left index of the search range
// right - Right index of the search range
// targetID - Student ID to search for
// Returns the index of the target student if found, otherwise -1
int binarySearch(const Student stuList[], int left, int right, const char targetID[]);

int main()
{
	int choice;
	int stuNum = 0;
	int selectedIndex = -1;
	int courseTotal = 0;


	Student* stuList = studentGet(stuNum);

	readCourses(stuList, stuNum);

	do
	{
		cout << "1. Select Student by ID\n";
		cout << "2. Display Selected Student's Schedule\n";
		cout << "3. Add a Course to Student's Schedule\n";
		cout << "4. Remove a Course from Student's Schedule\n";
		cout << "5. Exit\n";
		cout << endl;
		cout << "Select: ";

		// Gets the numeric entry from the menu
		cin >> choice;

		// Makes the 'enter' key that was pressed after the numeric entry be ignored
		cin.ignore();

		switch (choice)
		{
		case 1:
			selectID(stuList, stuNum, selectedIndex);
			break;
		case 2:
			displaySchedule(stuList, selectedIndex, courseTotal);
			break;
		case 3:
			findCourses(stuList, selectedIndex);
			break;
		case 4:
			removeCourse(stuList, selectedIndex);
			break;
		}

	} while (choice != 5);
	saveChanges(stuList, stuNum);

	cleanUP(stuList, stuNum);

	return 0;
}

Student* studentGet(int& numStu)
{
	char check = ' ';
	char line[STUIDSIZE];

	ifstream inFile(STUFILE);

	//this checks if the student file opened properly
	if (!inFile.is_open())
	{
		cerr << "\nERROR: Input file cannot be opened\n";
		inFile.close();
		exit(1);
	}

	//this loops through the file for as long as there are still characters and counts the number of new lines as since there is only one student per line
	//the number of new lines is equal to the number of students (most times)
	while (inFile.get(check))
	{
		if (check == '\n')
		{
			numStu++;
		}
	}

	//clears the eof state flag and sets the read position back to the beginning
	inFile.clear();
	inFile.seekg(0, ios::beg);

	//creates the dynamic student array
	Student* stuList = new Student[numStu];

	//loops through the array, since it has the same number of positions as the file has students and reads each ID and gives it to the students
	for (int x = 0; x < numStu; x++)
	{
		inFile.getline(line, STUIDSIZE);
		snprintf(stuList[x].id, STUIDSIZE, line);
	}

	inFile.close();
	//returning the dynamic student array so it can be used outside the function
	return stuList;
}

void readCourses(Student stuList[], int numStu)
{

	char line[COURSEIDSIZE];

	int courseSect = 0;
	int creditHrs = 0;
	int counter = 0;
	ifstream inFile(COURSEFILE);

	//checks to see if the course file opened properly and if it didn't it sets all of the students courses to null 
	if (!inFile.is_open())
	{
		cerr << "Error restoring course file\n\n";
		for (counter; counter < numStu; counter++)
		{
			stuList[counter].courses = NULL;
		}
	}
	else
	{
		//creates a temporary node pointer
		Node* temp = NULL;

		//this loops through all of the students in the array
		for (counter = 0; counter < numStu; counter++)
		{
			//this sets the "head" to null
			stuList[counter].courses = NULL;

			//this gets the course id text on the first line
			inFile.getline(line, COURSEIDSIZE);
			
			//this makes sure that if the first thing that is encountered for a student is end to leave that student with courses as null and move on to the next one
			//and if it isn't end to then create the head node and fill it with the data it needs, set the next to null, then see if there are more classes for the student
			//if there are no more classes for this student, as in END is the next thing grabbed, it moves on to the next student
			//if there are more classes it uses the temp to create a new node after the node the head points to, fill it with the data it needs from the file and sets the next to null 
			//while grabing the next line to see if there are more classes
			if (strcmp(line, "END") != 0)
			{
				stuList[counter].courses = new Node;
				snprintf(stuList[counter].courses->item.id, COURSEIDSIZE, line);
				inFile >> courseSect;
				stuList[counter].courses->item.section = courseSect;
				inFile >> creditHrs;
				stuList[counter].courses->item.credit = creditHrs;
				stuList[counter].courses->next = NULL;
				temp = stuList[counter].courses;

				inFile.ignore(numeric_limits<streamsize>::max(), '\n');

				inFile.getline(line, COURSEIDSIZE);

				while (strcmp(line, "END") != 0)
				{
					temp->next = new Node;
					temp = temp->next;
					snprintf(temp->item.id, COURSEIDSIZE, line);
					inFile >> courseSect;
					temp->item.section = courseSect;
					inFile >> creditHrs;
					temp->item.credit = creditHrs;
					temp->next = NULL;
					inFile.ignore(numeric_limits<streamsize>::max(), '\n');
					inFile.getline(line, COURSEIDSIZE);
				}
			}

		}
	}
	inFile.close();
}

void saveChanges(Student stuList[], int numStu)
{
	ofstream outFile(COURSEFILE);

	//head and temp node pointers created so that the actual head of the lists doesn't get moved on accident
	Node* head = NULL;
	Node* temp = NULL;

	//loops through the student array  and sets the head and temp to the head of the linked list for that student
	for (int rows = 0; rows < numStu; rows++)
	{
		head = stuList[rows].courses;
		temp = head;
		//loops through the linked list as long as the end hasn't been reached and prints out the information at each node into the course file
		while (temp != NULL)
		{
			outFile << temp->item.id << "\n";
			outFile << temp->item.section << "\n";
			outFile << temp->item.credit << "\n";
			temp = temp->next;
		}
		outFile << "END\n";
	}
	outFile.close();
}

void findCourses(Student stuList[], int stuNum)
{	//see if student was selected
	if (stuNum == -1)
	{
		cout << "\nNo student selected\n" << endl;
	}
	else
	{
		// Get course information
		Course course{};
		cout << "Enter course ID: ";
		cin >> course.id;

		cout << "Enter section number: ";
		cin >> course.section;

		cout << "Enter credit hours: ";
		cin >> course.credit;
		cout << endl;

		// function call to add course to schedule
		addCourse(stuList[stuNum].courses, course);
	}
}

void displaySchedule(Student stuList[], int selectedIndex, int& courseNum)
{
	// Check if a student has been selected
	if (selectedIndex == -1)
	{
		cout << "\nNo student selected\n" << endl;
		return;
	}

	// Get the linked list of courses for the selected student
	Node* current = stuList[selectedIndex].courses;

	// Check if the student is not registered for any courses
	if (current == NULL)
	{
		cout << "\nStudent is not registered for any courses\n" << endl;
		return;
	}
	//This is to reset the coursenum
	courseNum = 1;
	cout << "\n";
	while (current != NULL)
	{
		cout << courseNum << "." << current->item.id << ", Section: " << current->item.section << ", Credit Hours: " << current->item.credit << endl;
		current = current->next;
		courseNum++;
	}
	printf("\n");
}

void addCourse(Node*& courseList, const Course& newCourse)
{
	// Create a new node for the new course
	Node* newNode = new Node;
	// Assign the new course to the item of the new node
	newNode->item = newCourse;
	// Set the next pointer of the new node to the current head of the linked list
	newNode->next = courseList;
	// Update the head of the linked list
	courseList = newNode;
}

void selectID(Student stuList[], int& stuNum, int& selectedStudentIndex)
{	// Input variable for the target student ID
	char targetID[STUIDSIZE];

	// Prompt the user to enter a student ID
	cout << "Enter student ID: ";
	cin >> targetID;

	// Perform binary search to find the student in the array
	int result = binarySearch(stuList, 0, stuNum - 1, targetID);

	// Check the result of the search
	if (result != -1)
	{
		// Display the student is found
		cout << "Student selected\n\n";
		// Update selected student index
		selectedStudentIndex = result;
	}
	else
	{
		// Display the student was not found
		cout << "Student ID not found\n\n";
		// Reset the selected student index
		selectedStudentIndex = -1;
	}
}

int binarySearch(const Student stuList[], int left, int right, const char targetID[])
{	// Continue the search as long as the left index is less than or equal to the right index
	if (left <= right)
	{	// Calculate the middle index of the current search range
		int mid = left + (right - left) / 2;

		if (strcmp(stuList[mid].id, targetID) == 0)
		{
			// Found the target student
			return mid;
		}
		else if (strcmp(stuList[mid].id, targetID) < 0)
		{
			return binarySearch(stuList, mid + 1, right, targetID);
		}
		else
		{
			return binarySearch(stuList, left, mid - 1, targetID);
		}
	}

	// Target student ID not found
	return -1;
}

void removeCourse(Student stuList[], int index)
{
	int courseNumber = 0;
	int numberOfCourse = 0;

	//Checking to see if a student was selected
	if (index == -1)
	{
		cout << "\nNo student has been selected\n" << endl;
	}
	else
	{
		// Displaying the student schedule to see which course they want to remove
		displaySchedule(stuList, index, numberOfCourse);

		// checking to see if the student has any courses to actually chose from
		if (numberOfCourse == 0)
		{
			cout << "There are no courses to select" << endl;
			return;
		}

		//Getting the number from the operator for which course to remove
		// The number is in corelation to the course in the schedule like 1,2,3
		cout << "Enter the number of the course you want to remove" << endl;
		cin >> courseNumber;
		

		//checking to make sure the number they inputed is a number on thier schedule
		if (courseNumber < 1 && courseNumber > numberOfCourse)
		{
			cout << "Invalid course to be removed" << endl;

			return;
		}

		//checking to see if the course they are wanting to remove is the first one
		Node* temp = stuList[index].courses;

		if (courseNumber == 1)
		{
			stuList[index].courses = temp->next;
			delete temp;
			cout << "Course removed\n" << endl;
			return;
		}
		//checking to see if the course they want to remove is anything other then the 1st

		for (int i = 1; temp != NULL && i < courseNumber - 1; i++)
		{
			temp = temp->next;
		}

		if (temp == NULL || temp->next == NULL)
		{
			cout << "Course removed\n" << endl;
			return;
		}

		/* This part of the function finds the node that the course is on, and removes it.
		It takes the previous node moves it to the next if neccessary or the next node and moves it down
		 also works for the last node in the list as well */

		Node* next = temp->next->next;

		delete temp->next;

		temp->next = next;

		}


}

void cleanUP(Student stuList[], int numStu)
{
	// This for loop goes through all of the elements
	for (int i = 0; i < numStu; i++)
	{
		// This checks through all of the elements in the linked list and checks if it is the last element
		// If the pointer to the next element isn't null, then it deletes the data in the memory adress
		while (stuList[i].courses != NULL)
		{
			Node* temp = stuList[i].courses->next;
			delete stuList[i].courses;
			stuList[i].courses = temp;
		}
	}

	delete[] stuList;
}