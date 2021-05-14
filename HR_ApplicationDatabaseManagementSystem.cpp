
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<occi.h>
#include<cstring>
#include<iomanip>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

//Employee Structure
struct Employee
{
    int employeeNumber;
    char lastName[50];
    char firstName[50];
    char extension[10];
    char email[100];
    char officecode[10];
    int reportsTo;
    char jobTitle[50];
};

//Prototypes

//Utilities

int getInt(const char* prompt = nullptr   // User entry prompt
);
int getInt(
    int min,   // minimum acceptable value
    int max,   // maximum acceptable value
    const char* prompt = nullptr,  // User entry prompt
    const char* errorMessage = nullptr, // Invalid value error message
    bool showRangeAtError = true    // display the range if invalud value entered 
);
//Program functions

int menu(void);

//This function receives an OCCI pointer (a reference variable to an Oracle database), an integer number as the employee number, and a pointer to a variable of type Employee. The function returns 0 if the employee does not exist. It returns 1 if the employee exists. 
int findEmployee(Connection*, int, struct Employee*);

//This function receives a Connection pointer (a reference variable to an Oracle database) and the members of a variable of type Employee and displays all members of the emp parameter. 
void displayEmployee(struct Employee emp);

//This function receives a pointer of type OCCI Conection (a reference variable to an Oracle database) and displays all employees’ information if it exists.
void displayAllEmployees(Connection* conn);

//This function gets the employee’s information from the user and stores the values in the corresponding members of the structure pointed by the emp pointer
void insertEmployee(struct Employee* emp);

//This function receives an OCCI pointer (a reference variable to an Oracle database) and a variable of type Employee and inserts the given employee information stored in the parameter emp to the employees table. 
void insertEmployee(Connection* conn, struct Employee emp);

//This function receives an OCCI pointer(a reference variable to an Oracle database) and an integer number as the employee numberand updates the phone extension for the given employee.
void updateEmployee(Connection* conn, int employeeNumber);

//This function receives an OCCI pointer (a reference variable to an Oracle database) and an integer number as the employee number and deletes a row with the given employee number from table employees. 
void deleteEmployee(Connection* conn, int employeeNumber);
//=================================class declarations======================================

//Menu
class Menu
{
    char* m_text; // holds the menu content dynamically
    int m_noOfSel;  // holds the number of options displayed in menu content
    void display()const;
public:
    Menu(const char* MenuContent, int NoOfSelections);
    virtual ~Menu();
    int& operator>>(int& Selection);
    //copy constructor
    Menu(const Menu& src);
    //copy assiginment operator
    Menu& operator=(const Menu&) = delete;
};
// displays the menu and returns the selected option
int menu(void)
{
    Menu m("********************* HR Menu *********************\n1) Find Employee\n2) Employees Report\n3) Add Employee\n4) Update Employee\n5) Remove Employee", 5);
    int selection;
    m >> selection;
    return selection;
}
//====================================================================================

int main()
{
    Environment* env = nullptr;
    Connection* conn = nullptr;

    string user = "";   //place your Oracle SQL developer user id here example: d**********5
    string pass = "";       // place your password here example: 2******6
    string constr = "";     //place the connection sting here example: myoracle12c.---text here----.ca:1521/oracle12c

    try
    {
        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(user, pass, constr);

        int employeeNumber = 0;
        Employee emp;
        int a = 1;
        while (a != 0)
        {
            a = menu();
            switch (a)
            {
            case 1:
                //cin >> employeeNumber;
                employeeNumber = getInt("Enter Employee Number: ");
                if (findEmployee(conn, employeeNumber, &emp))
                {
                    displayEmployee(emp);
                }
                else
                {
                    cout << "Employee " << employeeNumber << " does not exist." << endl;
                }
                break;
            case 2:
                displayAllEmployees(conn);
                break;
            case 3:
                insertEmployee(&emp);
                if (!findEmployee(conn, emp.employeeNumber, &emp))
                {
                    insertEmployee(conn, emp);
                }
                else
                {
                    cout << "\nAn employee with the same employee number exists.\n";
                }
                break;
            case 4:
                employeeNumber = getInt("Enter Employee Number : ");
                //cin >> employeeNumber;
                updateEmployee(conn, employeeNumber);
                break;
            case 5:
                employeeNumber = getInt("Enter Employee Number : ");
                //cin >> employeeNumber;
                deleteEmployee(conn, employeeNumber);
                break;
            case 0:
                cout << "Goodbye";
                break;
            default:
                cout << "This will never happen!!!!" << endl;
                break;
            }
            cout << endl;
        }
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
    }

    catch (SQLException& sqlExp)
    {
        cout << sqlExp.getErrorCode() << ": " << sqlExp.getMessage();
    }
    return 0;

}


//==================================Function Definitions=====================================

//======================================Utilities============================================

//This function performs a fool-proof integer entry from the console
int getInt(const char* prompt  // User entry prompt
)
{
    int number; // to store the input if it is an integer
    unsigned intlength = 0;
    char integer[12]; // to accomodate the number within the integer range (-2,147,483,647 to +2,147,483,647) 

    if (prompt != nullptr)
    {
        cout << prompt;
    }

    scanf(" %[^\n]s", integer);
    //to check if the first value satisfies condition to be an integer 
    while (!isdigit(integer[0]) && integer[0] != '+' && integer[0] != '-')
    {
        cout << "Bad integer value, try again: ";
        scanf(" %[^\n]s", integer);
    }
    //to check if all the values entered are numbers 
    while (intlength == 0)
    {
        for (unsigned i = 0; i < strlen(integer); i++)
        {
            if (i == 0 && (integer[i] == '+' || integer[i] == '-'))
            {
                intlength++;
            }
            if (isdigit(integer[i]))
            {
                intlength++;
            }
        }
        if (intlength != strlen(integer))
        {
            intlength = 0;
            cout << "Enter only an integer, try again: ";
            scanf(" %[^\n]s", integer);
        }
    }

    int i, //iterrator          
        lengthOfnum = strlen(integer),
        x = 0;
    //to convert the character string to integer
    if (integer[0] == '+' || integer[0] == '-' || integer[0] == '0' || isdigit(integer[0]))
    {
        if (integer[0] == '+' || integer[0] == '-')
        {
            for (i = 1; i < lengthOfnum; i++)
            {
                number = integer[i] - '0';
                if (integer[0] == '+' && (x > (2147483647) / 10 || x == (2147483647) / 10) && number > 7)
                {
                    x = 2147483647;
                    i = lengthOfnum;
                }
                else if (integer[0] == '-' && ((x > ((2147483646 / 10) + (2 / 10))) || (x == ((2147483646 / 10 + 2 / 10)) && number >= 8)))
                {
                    x = -2147483647 - 1;
                    i = lengthOfnum;
                }
                else
                {
                    x = x * 10 + number;
                }
            }
        }
        else if (isdigit(integer[0]))
        {
            for (i = 0; i < lengthOfnum; i++)
            {
                number = integer[i] - '0';
                if (isdigit(integer[0]) && (x > (2147483647) / 10 || x == (2147483647)) / 10 && number > 7)
                {
                    x = 2147483647;
                    i = lengthOfnum;
                }
                else
                {
                    x = x * 10 + number;
                }
            }
        }
        number = x;
    }
    else
    {
        number = 0;
    }
    if (integer[0] == '-')
    {
        //to avoid overflow
        if (x != -2147483647 - 1)
        {
            number = number * -1;
        }
    }
    return number;
}

//This function performs a fool - proof integer entry from the console within a range
int getInt(
    int min,   // minimum acceptable value
    int max,   // maximum acceptable value
    const char* prompt,  // User entry prompt
    const char* errorMessage, // Invalid value error message
    bool showRangeAtError    // display the range if invalud value entered 
)
{
    int value = getInt(prompt);
    while (value > max || value < min)
    {
        if (errorMessage != nullptr)
        {
            cerr << errorMessage;
        }
        if (showRangeAtError)
        {   // is of the form "[MM <= value <= XX]"
            cout << '[' << min << " <= " << "value" << " <= " << max << ']' << ": ";
        }
        value = getInt(); //call with no prompt
    }
    return value;
}
//=============================================================================================
//===============================Menu===================================================

//Constructor
Menu::Menu(const char* MenuContent, int NoOfSelections)
{
    m_text = nullptr;
    m_text = new char[strlen(MenuContent) + 1];
    strcpy(m_text, MenuContent);
    m_noOfSel = NoOfSelections;
}

//destructor
Menu::~Menu()
{
    delete[] m_text;
    m_text = nullptr;
}

//copy constructor
Menu::Menu(const Menu& src)
{
    m_noOfSel = src.m_noOfSel;
    m_text = nullptr;
    if (src.m_text != nullptr)
    {
        m_text = new char[strlen(src.m_text) + 1];
        strcpy(m_text, src.m_text);
    }
}

//Displays the Menu content
void Menu::display()const
{
    cout << m_text << endl;
    cout << "0) Exit" << endl;
}

//The member insertion operator 	
int& Menu::operator>>(int& Selection)
{
    display();
    Selection = getInt(0, m_noOfSel, "Enter an option (0-5): ", "Invalid option ");
    return Selection;
}
//====================================================================

//========================Program Functions===========================

int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp)
{
    int result = 0;

    string query = "SELECT employeenumber, lastname, firstname, extension, email, officecode, reportsto, jobtitle\
					FROM employees\
					WHERE employeenumber = :1";

    Statement* stmt = conn->createStatement();
    stmt->setSQL(query);
    stmt->setInt(1, employeeNumber);
    ResultSet* rs = stmt->executeQuery();

    if (rs->next())
    {
        emp->employeeNumber = rs->getInt(1);
        strcpy(emp->lastName, rs->getString(2).c_str());
        strcpy(emp->firstName, rs->getString(3).c_str());
        strcpy(emp->extension, rs->getString(4).c_str());
        strcpy(emp->email, rs->getString(5).c_str());
        strcpy(emp->officecode, rs->getString(6).c_str());
        emp->reportsTo = rs->getInt(7);
        strcpy(emp->jobTitle, rs->getString(8).c_str());
        result = 1;
    }
    conn->terminateStatement(stmt);
    return result;
};

void displayEmployee(struct Employee emp)
{
    cout << "\n-------------- Employee Information -------------" << endl;
    cout << "Employee Number: " << emp.employeeNumber << endl;
    cout << "Last Name: " << emp.lastName << endl;
    cout << "First Name: " << emp.firstName << endl;
    cout << "Extension: " << emp.extension << endl;
    cout << "Email: " << emp.email << endl;
    cout << "Office Code: " << emp.officecode << endl;
    cout << "Manager ID: " << emp.reportsTo << endl;
    cout << "Job Title: " << emp.jobTitle << endl;
};

void display(ResultSet* rs)
{
    //to display the hyphens above the the coloumn headers
    cout.setf(ios::right);
    cout.fill('-');
    cout << setw(8) << "   ";
    cout << setw(22) << "   ";
    cout << setw(35) << "   ";
    cout << setw(19) << "   ";
    cout << setw(12) << "   ";
    cout << setw(21) << '-' << endl;
    cout.unsetf(ios::right);

    //to display the column headers
    cout.setf(ios::left);
    cout.fill(' ');
    cout << setw(6) << "ID" << "  ";
    cout << setw(20) << "Employee Name" << "  ";
    cout << setw(33) << "Email " << "  ";
    cout << setw(17) << "Phone " << "  ";
    cout << setw(11) << "Extension " << " ";
    cout << setw(25) << "Manager Name " << endl;
    cout.unsetf(ios::left);

    //to display the hyphens below the the coloumn headers
    cout.setf(ios::right);
    cout.fill('-');
    cout << setw(8) << "   ";
    cout << setw(22) << "   ";
    cout << setw(35) << "   ";
    cout << setw(19) << "   ";
    cout << setw(12) << "   ";
    cout << setw(21) << '-' << endl;
    cout.unsetf(ios::right);

    //to print the values
    cout.setf(ios::left);
    do
    {
        cout.fill(' ');
        cout << setw(8) << rs->getInt(1);
        cout << setw(22) << rs->getString(2);
        cout << setw(35) << rs->getString(3);
        cout << setw(19) << rs->getString(4);
        cout << setw(12) << rs->getString(5);
        cout << setw(25) << rs->getString(6);
        cout << endl;
    } while (rs->next());
    cout.unsetf(ios::left);
}
void displayAllEmployees(Connection* conn)
{
    string query1 = "SELECT e.employeenumber, e.firstname || ' ' || e.lastname AS name, e.email, o.phone, e.extension, m.firstname || ' ' || m.lastname AS manager_name FROM employees e INNER JOIN offices o ON e.officecode = o.officecode LEFT OUTER JOIN employees m ON m.employeenumber = e.reportsto    ORDER BY employeenumber";
    Statement* stmt = conn->createStatement(query1);
    ResultSet* rs = stmt->executeQuery();

    //to check if the result set is empty
    if (!rs->next())
    {
        cout << "There is no employees’ information to be displayed." << endl;
    }
    else
    {
        display(rs);
    }
    conn->terminateStatement(stmt);
    cout << endl;
}

void insertEmployee(struct Employee* emp)
{
    strcpy(emp->officecode, "1");
    emp->reportsTo = 1002;

    cout << "\n-------------- New Employee Information -------------\n";
    emp->employeeNumber = getInt("Employee Number: ");
    cout << "Last Name: ";
    cin >> emp->lastName;
    cout << "First Name: ";
    cin >> emp->firstName;
    cout << "Extension: ";
    cin >> emp->extension;
    cout << "Email: ";
    cin >> emp->email;
    cout << "Office Code: " << emp->officecode << endl;
    cout << "Manager ID: " << emp->reportsTo << endl;
    cout << "Job Title: ";
    cin >> emp->jobTitle;
}

void insertEmployee(Connection* conn, struct Employee emp)
{
    string query = "INSERT INTO employees VALUES (:1,:2,:3,:4,:5,:6,:7,:8)";
    Statement* stmt = conn->createStatement();
    stmt->setSQL(query);
    //inserting the row
    stmt->setInt(1, emp.employeeNumber);
    stmt->setString(2, emp.lastName);
    stmt->setString(3, emp.firstName);
    stmt->setString(4, emp.extension);
    stmt->setString(5, emp.email);
    stmt->setString(6, emp.officecode);
    stmt->setInt(7, emp.reportsTo);
    stmt->setString(8, emp.jobTitle);
    stmt->executeUpdate();
    conn->terminateStatement(stmt);
    cout << "\nThe new employee is added successfully.\n";

}


void updateEmployee(Connection* conn, int employeeNumber)
{
    Employee emp;

    if (findEmployee(conn, employeeNumber, &emp))
    {
        cout << "Last Name: " << emp.lastName << endl;
        cout << "First Name: " << emp.firstName << endl;
        cout << "Extension: ";
        cin >> emp.extension;

        string query = "UPDATE employees SET extension = :1 WHERE employeenumber = :2";
        Statement* stmt = conn->createStatement();
        stmt->setSQL(query);
        stmt->setString(1, emp.extension);
        stmt->setInt(2, emp.employeeNumber);
        stmt->executeUpdate();

        conn->commit();
        conn->terminateStatement(stmt);
        cout << "The employee's extension is updated successfully.\n";
    }
    else
    {
        cout << "The employee with ID " << employeeNumber << " does not exist.\n";
    }
}

void deleteEmployee(Connection* conn, int employeeNumber)
{
    Employee emp;

    if (findEmployee(conn, employeeNumber, &emp))
    {
        string query = "DELETE FROM employees WHERE employeenumber = :1 ";
        Statement* stmt = conn->createStatement();
        stmt->setSQL(query);
        stmt->setInt(1, emp.employeeNumber);
        stmt->executeUpdate();

        conn->commit();
        conn->terminateStatement(stmt);
        cout << "The employee with ID " << emp.employeeNumber << " is deleted successfully.\n" << endl;
    }
    else
    {
        cout << "The employee with ID " << employeeNumber << " does not exist.\n";
    }
}
