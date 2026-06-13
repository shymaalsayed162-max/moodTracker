#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// ===========================structures needed===================================================
struct date {
    int day;
    int month;
    int year;
};
struct MoodInfo {
    string MoodType;
    string Note;
    int MoodLevel;
};
struct MoodEntry {
    date TodaysDate;
    MoodInfo TodaysMood;
};
struct UserAccount {
    int userID;
    string username;
    string password;
};
struct MoodStatistics {
    int totalEntries;
    int happyCount, sadCount, angryCount, stressedCount, calmCount;
    double averageMoodLevel;
};
// ===========================declarations of functions===================================================

int mainMenu();
void saveData(MoodEntry moods[], int entryCount);
void loadData(MoodEntry moods[], int& entryCount);
void addMood();
void editMood();
void deleteMood();
int manageMoodMenu();
void displayAll();
void searchMood();
void search_by_type();
void search_by_date();
void CalcAverageMoodLevel(int idx);
void MoodAnalysis(MoodStatistics stat);
void InitializeStatistics();
void initialize_users();
bool login();
void logout();
void sign_Up();
string quotes();
// ===============================initialization===========================================================
const int MAX_MOODS = 100;
const int MAX_USERS = 10;
const int MAX_MONTHS = 12;
const int entries = 100;
int moodCount = 0;
MoodEntry moods[MAX_MOODS] = {
                            {{1,6,2026} ,{"Happy","it was my birthday ",5} },
                            {{2,6,2026},{"Angry","all my plans got ruined",2} },
                            {{3, 6,2026} ,{"stressed","i feel overwhelmed",2}},
                            {{15,6,2026 },{"Sad","i feel alone",1} },
                            {{16,7, 2026} ,{"Calm","i trust the process",4}},
                            {{20,7,2026},{"Calm","i handled things well",4} },
                            {{25, 7,2026},{"Stressed","i have exams",1 } },
                            {{5,8,2026},{"Happy","i achieved something", 5} },
                            {{30,8,2026}, {"Calm","i found my balance",4} },
                            {{8,9,2026},{"Sad","i feel tired",1}}
};
int ArrCount = 10;
int user_count = 0;
int current_login_userID = -1;
UserAccount users[10];
MoodStatistics statistics[12];
// ==========================================================================================
int main() {
    srand(time(0));
    loadData(moods, moodCount);

    InitializeStatistics();
    bool isUser = false;
    initialize_users();
    int choice;

    // The first step is logging in or creating an account.
    do {
        cout << "Choose 1: for login, 2: for sign up, 3: for exit\n";
        cin >> choice;

        if (choice == 1) {
            isUser = login();
            if (isUser == true) {
                cout << "START MOOD TRACKING\n";

                //The second stage: The list of basic programs opens only after logging in.
                int moodChoice;
                do {
                    moodChoice = mainMenu();
                    switch (moodChoice) {
                    case 1:
                    {
                        int subChoice = manageMoodMenu();
                        if (subChoice == 1) {
                            addMood();
                            InitializeStatistics();
                        }
                        else if (subChoice == 2) {
                            editMood();
                            InitializeStatistics();
                        }
                        else if (subChoice == 3) {
                            deleteMood();
                            InitializeStatistics();
                        }
                        else if (subChoice == 4) {
                            cout << "Going back to main menu.." << endl;
                        }

                        break;
                    }
                    case 2:
                        displayAll();
                        break;
                    case 3:
                        searchMood();
                        break;
                    case 4: {
                        cout << "Which month's mood do you want to analyze?" << endl;
                        int month;
                        cin >> month;
                        // Validate month
                        while (month < 1 || month > 12) {
                            cout << "Invalid month entered! Please enter a number from 1 to 12: ";
                            cin >> month;
                        }
                        //Check for data for this month 
                        if (statistics[month - 1].totalEntries == 0) {
                            cout << "No Entries for this month!" << endl;
                        }
                        else {
                            CalcAverageMoodLevel(month - 1);
                            MoodAnalysis(statistics[month - 1]);
                        }
                        break;
                    }
                    case 5:
                        cout << "Saving data and exiting...\n";
                        saveData(moods, moodCount);
                        break;
                    default:
                        cout << "Invalid choice! Try again.\n";
                    }
                } while (moodChoice != 5);


                break;
            }
        }
        else if (choice == 2) {
            sign_Up();
        }
        else if (choice == 3) {
            logout();
            break;
        }
        else {
            cout << "Invalid input" << endl;
        }
    } while (true);

    cout << "Thank you for using our app, goodbye!\n";
    return 0;
}
//Function to display the main navigation menu and get user choice
int mainMenu()
{
    int choice;
    do {
        cout << "\n--- Digital Mood Tracker System ---\n";
        cout << "1. Add/Edit/Delete Mood\n";
        cout << "2. View All Moods\n";
        cout << "3. Search Mood\n";
        cout << "4. View Mood Analysis\n";
        cout << "5. save data and Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (choice < 1 || choice>5)
            cout << "Invalid choice! Please enter a number between 1 and 5.\n";

    } while (choice < 1 || choice>5);
    return choice;
}
//====================================================================================================================
// Function for managing submenu (Add/Edit/Delete)
int manageMoodMenu() {
    int subChoice;
    do {
        cout << "\n--- Manage Your Moods ---\n";
        cout << "1. Add New Mood\n";
        cout << "2. Edit Existing Mood\n";
        cout << "3. Delete a Mood\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> subChoice;

        if (subChoice < 1 || subChoice > 4) {
            cout << "Invalid choice! Please enter a number between 1 and 4.\n";
        }
    } while (subChoice < 1 || subChoice > 4);

    return subChoice;
}
//===========================================================================================================================
void loadData(MoodEntry moods[], int& entryCount)
{
    entryCount = 10;
    ifstream inFile("moods_data.txt");

    if (inFile.is_open())
    {
        string line;

        while (getline(inFile, line) && entryCount < MAX_MOODS)
        {
            size_t pos = line.find("#");

            string dataPart = line;
            string notePart = "";

            if (pos != string::npos)
            {
                dataPart = line.substr(0, pos);
                notePart = line.substr(pos + 1);
            }

            stringstream ss(dataPart);

            if (ss >> moods[entryCount].TodaysDate.day
                >> moods[entryCount].TodaysDate.month
                >> moods[entryCount].TodaysDate.year
                >> moods[entryCount].TodaysMood.MoodType
                >> moods[entryCount].TodaysMood.MoodLevel)
            {
                moods[entryCount].TodaysMood.Note = notePart;
                entryCount++;

            }
        }

        inFile.close();
    }
}
void saveData(MoodEntry moods[], int entryCount) //Function to save all mood entries from the array into a text file?
{
    ofstream outFile("moods_data.txt");        //Open The Writing File
    if (outFile.is_open())
    {
        for (int i = 0; i < entryCount; i++)
        {
            outFile << moods[i].TodaysDate.day << " "
                << moods[i].TodaysDate.month << " "
                << moods[i].TodaysDate.year << " "
                << moods[i].TodaysMood.MoodType << " "
                << moods[i].TodaysMood.MoodLevel << " #"
                << moods[i].TodaysMood.Note << endl;
        }
        outFile.close();
        cout << "Data saved successfuly\n";
    }
    else
    {
        cout << "Error! The file could not be opened for saving.\n";
    }
}
// ==============================================================================================
void addMood()
{
    //reading the mood of the user
    cout << "Please enter today's date (day / month )";
    cin >> moods[ArrCount].TodaysDate.day >> moods[ArrCount].TodaysDate.month;
    //if the user entered a wrong date (month or week ) he must rewrite it
    while (moods[ArrCount].TodaysDate.month > 12 or moods[ArrCount].TodaysDate.month < 1)
    {
        cout << "wrong month please re-enter today's date (day / month ) ";
        cin >> moods[ArrCount].TodaysDate.day >> moods[ArrCount].TodaysDate.month;
    }
    while ((moods[ArrCount].TodaysDate.month == 1 or moods[ArrCount].TodaysDate.month == 3 or
        moods[ArrCount].TodaysDate.month == 5 or moods[ArrCount].TodaysDate.month == 7 or
        moods[ArrCount].TodaysDate.month == 8 or moods[ArrCount].TodaysDate.month == 10 or
        moods[ArrCount].TodaysDate.month == 12) and moods[ArrCount].TodaysDate.day > 31 or
        moods[ArrCount].TodaysDate.day < 1)
    {
        cout << "wrong day this month is \"from 1 to 31\" , please re - enter today's date (day / month ) ";
        cin >> moods[ArrCount].TodaysDate.day >> moods[ArrCount].TodaysDate.month;
    }
    while ((moods[ArrCount].TodaysDate.month == 2) and (moods[ArrCount].TodaysDate.day > 28 or moods[ArrCount].TodaysDate.day < 1))
    {
        cout << "wrong day this month is \"from 1 to 28\" , please re - enter today's date (day / month ) ";
        cin >> moods[ArrCount].TodaysDate.day >> moods[ArrCount].TodaysDate.month;
    }
    while ((moods[ArrCount].TodaysDate.month == 4 or moods[ArrCount].TodaysDate.month == 6 or
        moods[ArrCount].TodaysDate.month == 9 or moods[ArrCount].TodaysDate.month == 11) and (
            moods[ArrCount].TodaysDate.day > 30 or moods[ArrCount].TodaysDate.day < 1))
    {
        cout << "wrong day this month is \"from 1 to 30\" , please re - enter today's date (day / month ) ";
        cin >> moods[ArrCount].TodaysDate.day >> moods[ArrCount].TodaysDate.month;
    }
    for (int i = 0; i < (ArrCount); i++)
    {
        if (moods[ArrCount].TodaysDate.day == moods[i].TodaysDate.day and moods[ArrCount].TodaysDate.month == moods[i].TodaysDate.month)
        {
            cout << "This date already exists! Please re-enter today's date (day / month ): ";
            cin >> moods[ArrCount].TodaysDate.day >> moods[ArrCount].TodaysDate.month;
        }
    }
    moods[ArrCount].TodaysDate.year = 2026;
    cout << "Enter your mood type ( Happy - Sad - Angry - Stressed - Calm)";
    cin >> moods[ArrCount].TodaysMood.MoodType;
    cout << "write more notes about your mood (what happened today - who you met today - why you have this mood ) ";
    cin.ignore();
    getline(cin, moods[ArrCount].TodaysMood.Note);
    cout << "from what you've written rate your mood out of 5";
    cin >> moods[ArrCount].TodaysMood.MoodLevel;
    //if the user entered the mood level wrong he must rewrite it
    while (moods[ArrCount].TodaysMood.MoodLevel < 1 or moods[ArrCount].TodaysMood.MoodLevel > 5)
    {
        cout << "please re-enter your mood level (from 1 to 5 only accepted)";
        cin >> moods[ArrCount].TodaysMood.MoodLevel;
    }
    //a quote is printed if he had a bad mood today
    if (moods[ArrCount].TodaysMood.MoodLevel < 4)
    {
        string quote = quotes();
        cout << endl << endl << quote << endl << endl;
    }
    ArrCount++;
    moodCount++;
}
void InitializeStatistics() { //initializes values for statistics at the begining of the program
    for (int i = 0; i < 12; i++) {
        statistics[i].totalEntries = 0;
        statistics[i].happyCount = 0;
        statistics[i].sadCount = 0;
        statistics[i].angryCount = 0;
        statistics[i].stressedCount = 0;
        statistics[i].calmCount = 0;
        statistics[i].averageMoodLevel = 0;
    }
    for (int i = 0; i < ArrCount; i++) {
        if ((moods[i].TodaysMood.MoodType == "Happy") || (moods[i].TodaysMood.MoodType == "happy") || (moods[i].TodaysMood.MoodType == "HAPPY")) {
            statistics[moods[i].TodaysDate.month - 1].happyCount++;
        }
        else if ((moods[i].TodaysMood.MoodType == "Sad") || (moods[i].TodaysMood.MoodType == "sad") || (moods[i].TodaysMood.MoodType == "SAD")) {
            statistics[moods[i].TodaysDate.month - 1].sadCount++;
        }
        else if ((moods[i].TodaysMood.MoodType == "Angry") || (moods[i].TodaysMood.MoodType == "ANGRY") || (moods[i].TodaysMood.MoodType == "angry")) {
            statistics[moods[i].TodaysDate.month - 1].angryCount++;
        }
        else if ((moods[i].TodaysMood.MoodType == "Stressed") || (moods[i].TodaysMood.MoodType == "stressed") || (moods[i].TodaysMood.MoodType == "STRESSED")) {
            statistics[moods[i].TodaysDate.month - 1].stressedCount++;
        }
        else {
            statistics[moods[i].TodaysDate.month - 1].calmCount++;
        }
        statistics[moods[i].TodaysDate.month - 1].totalEntries++;
    }
}
void displayAll()//display all moods of all days which is entered
{
    cout << "(Day/Month/Year)" << "\t|" << "Your Mood Rating" << "\t|" << "Your Mood Type"
        << "\t\t|" << "Notes About Your Mood" << endl;
    //displaying all moods
    for (int i = 0; i < ArrCount; i++)
    {
        if (moods[i].TodaysDate.day == 0 and moods[i].TodaysDate.month == 0 and
            moods[i].TodaysDate.year == 0 and moods[i].TodaysMood.MoodLevel == 0 and
            moods[i].TodaysMood.MoodType == "" and moods[i].TodaysMood.Note == "")
        {
            break; //break displaying on the days which isn't read
        }
        cout << "( " << moods[i].TodaysDate.day << "/ " << moods[i].TodaysDate.month
            << "/ " << moods[i].TodaysDate.year << ")" << "\t\t|" << moods[i].TodaysMood.MoodLevel
            << "\t\t\t|" << moods[i].TodaysMood.MoodType << "\t\t\t|" << moods[i].TodaysMood.Note << endl;
    }
}
void CalcAverageMoodLevel(int idx) { //calculates the average of all moods throughout the month
    int count = 0;
    double sum = 0.0;

    for (int i = 0; i < ArrCount; i++) {
        if (moods[i].TodaysDate.month - 1 == idx) {
            sum += moods[i].TodaysMood.MoodLevel;
            count++;
        }
    }

    if ((count == 0)) {
        statistics[idx].averageMoodLevel = 0;
    }
    else {
        statistics[idx].averageMoodLevel = sum / count;
    }
}
string quotes() { //displays motivational quotes for user if they were sad,angry or stressed most of the month
    int randomNumber = rand() % 7;
    string quote;
    if (randomNumber == 0) {
        quote = "Your sadness is a gift. Don’t reject it. Don’t rush it. Live it fully and use it as fuel to change and grow. Maxime Lagacé";
    }
    else if (randomNumber == 1) {
        quote = "An arrow can only be shot by pulling it backward. So, when life is dragging you back with difficulties, it means that it’s going to launch you into something great.";
    }
    else if (randomNumber == 2) {
        quote = "Your hardest times often lead to the greatest moments of your life. Keep going. Tough situations build strong people in the end. Roy T. Bennett";
    }
    else if (randomNumber == 3) {
        quote = "Feeling lost, crazy, and desperate belongs to a good life as much as optimism, certainty, and reason. Alain de Botton";
    }
    else if (randomNumber == 4) {
        quote = "Do not be sad about what you do not have. Instead, be happy about what you do have. If you will be sad about what you don’t have, you will not be happy about what you do have. Leo Tolstoy";
    }
    else if (randomNumber == 5) {
        quote = "There is only one way to happiness, and that is to cease worrying about things which are beyond the power of our will. Epictetus";
    }
    else {
        quote = "We tend to forget that happiness doesn’t come as a result of getting something we don’t have, but rather of recognizing and appreciating what we do have. Frederick Keonig";
    }
    return quote;
}

void MoodAnalysis(MoodStatistics stat) { //displays mood entries for each month
    cout << "Total Entries of the Month = " << stat.totalEntries << "." << endl;
    float happy = (float)stat.happyCount / (float)stat.totalEntries;
    cout << "You were happy " << happy * 5 << " out of 5 this month." << endl;
    float sad = (float)stat.sadCount / (float)stat.totalEntries;
    cout << "You were sad " << sad * 5 << " out of 5 this month." << endl;
    float angry = (float)stat.angryCount / (float)stat.totalEntries;
    cout << "You were angry " << angry * 5 << " out of 5 this month." << endl;
    float calm = (float)stat.calmCount / (float)stat.totalEntries;
    cout << "You were calm " << calm * 5 << " out of 5 this month." << endl;
    float stressed = (float)stat.stressedCount / (float)stat.totalEntries;
    cout << "You were stressed " << stressed * 5 << " out of 5 this month." << endl;
    cout << "Average mood level this month was " << stat.averageMoodLevel << " out of 5." << endl;
    cout << "Your most occurring mood during this month was: ";
    float maxMood = stressed;
    string moodType = "Stress";
    if (maxMood < happy) { //displays most occuring mood for month
        maxMood = happy;
        moodType = "Happiness";
    }
    if (maxMood < sad) {
        maxMood = sad;
        moodType = "Sadness";
    }
    if (maxMood < angry) {
        maxMood = angry;
        moodType = "Anger";
    }
    if (maxMood < calm) {
        maxMood = calm;
        moodType = "Calm";
    }
    cout << moodType << endl;
    if ((moodType == "Sadness") || (moodType == "Stress") || (moodType == "Anger")) {
        cout << quotes() << endl;
    }
}

// ============================================================================================
// sign up function

void sign_Up() {
    if (user_count >= 10) {
        cout << "maximum number of users reached" << endl;
    }

    UserAccount new_user;
    new_user.userID = user_count + 1;
    cout << "Enter username:";
    cin >> new_user.username;

    for (int i = 0; i < user_count; i++)
    {
        if (users[i].username == new_user.username) {
            cout << "Username already exists!!" << endl;
            return;
        }
    }

    cout << "Enter the password\n";
    cin >> new_user.password;
    users[user_count] = new_user;
    user_count++;
    cout << "Account created successfully!\n";
}
//Login function

bool login() {
    string uname, pass;
    cout << "\n___Log In_\n";
    cout << "Username:\n";
    cin >> uname;
    cout << "Password:\n";
    cin >> pass;
    for (int i = 0; i < user_count; i++)
    {
        if (users[i].username == uname && users[i].password == pass)
        {
            current_login_userID = users[i].userID;
            cout << "welcome__" << users[i].username << "!\n";
            return true;
        }
    }
    cout << "Invalid username or password\n";
    return false;
}

// logout function 
void logout() {
    current_login_userID = -1;
    cout << "logged out successfully/n";

}
//pre-stored items
void initialize_users() {
    users[0] = { 101,"mariam","1720" };
    users[1] = { 102,"user2","2007" };
    user_count = 2;
}
void searchMood() {
    cout << "\nSearch by:\n1. Date\n2. Mood Type\nChoose (1-2): ";
    int searchChoice;
    cin >> searchChoice;

    if (searchChoice == 1) {
        search_by_date();
    }
    else if (searchChoice == 2) {
        search_by_type();
    }
    else {
        cout << "Invalid choice!\n";
    }
}

void search_by_type() {
    char mood_type;
    cout << "Enter the mood type to search for (H: Happy, S: Sad, A: Angry, T: Stressed, C: Calm): ";
    cin >> mood_type;

    bool found = false;
    for (int findmood = 0; findmood < MAX_MOODS; findmood++) {
        char firstLetter = toupper(moods[findmood].TodaysMood.MoodType[0]);
        if ((toupper(mood_type) == 'T' && firstLetter == 'S' && moods[findmood].TodaysMood.MoodType != "Sad" && moods[findmood].TodaysMood.MoodType != "sad") || // Differentiate Sad & Stressed
            firstLetter == toupper(mood_type)) {
            cout << "Mood on " << moods[findmood].TodaysDate.day << "/" << moods[findmood].TodaysDate.month
                << ": " << moods[findmood].TodaysMood.MoodType << " | Level " << moods[findmood].TodaysMood.MoodLevel
                << " | Notes: " << moods[findmood].TodaysMood.Note << endl;
            found = true;
        }
    }
    if (!found) cout << "No entries found for this mood type.\n";
}

void search_by_date() {
    int day, month;
    cout << "Enter the date to search for (day month): ";
    cin >> day >> month;

    if (day > 31 || day < 1 || month > 12 || month < 1) {
        cout << "Invalid date.\n";
        return;
    }

    bool found = false;
    for (int finddate = 0; finddate < MAX_MOODS; finddate++) {
        if (moods[finddate].TodaysDate.day == day && moods[finddate].TodaysDate.month == month) {
            cout << "Mood on " << moods[finddate].TodaysDate.day << "/" << moods[finddate].TodaysDate.month
                << ": " << moods[finddate].TodaysMood.MoodType << " | Level " << moods[finddate].TodaysMood.MoodLevel
                << " | Notes: " << moods[finddate].TodaysMood.Note << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "No mood entry found for the specified date.\n";
    }
}


//=============================================================================================================
// function updateMood definition
// step 1 : Ask for date
// step 2 : validate date
// step 3 : search
// step 4 : display current entry
// step 5 : ask what to edit
// step 6 : confirmation

void  editMood()
{
    char tryAgain = 'n';
    do {
        // step 1 : Ask for date
        int searchDay, searchMonth;
        cout << "Please enter the date you want to edit" << endl;
        cin >> searchDay >> searchMonth;


        // step 2 : validate date
            // first validate month
        while (searchMonth > 12 || searchMonth < 1)
        {
            cout << "Invalid month! Please re-enter the date you want to edit" << endl;
            cin >> searchDay >> searchMonth;
        }

        // second validate day
        while (searchDay > 31 || searchDay < 1)
        {
            cout << "Invalid day! Please re-enter the date you want to edit" << endl;
            cin >> searchDay >> searchMonth;
        }


        // step 3 : search
        bool found = false;
        for (int i = 0; i < ArrCount; i++)
        {
            if (moods[i].TodaysDate.day == searchDay && moods[i].TodaysDate.month == searchMonth)
            {

                found = true;


                // step 4 : display current entry
                cout << "Entry found! Here are the current details:" << endl;
                cout << "Day:" << moods[i].TodaysDate.day << endl;
                cout << "Month:" << moods[i].TodaysDate.month << endl;
                cout << "Mood Type:" << moods[i].TodaysMood.MoodType << endl;
                cout << "Mood Level:" << moods[i].TodaysMood.MoodLevel << endl;
                cout << "Note:" << moods[i].TodaysMood.Note << endl;


                // step 5 : ask what to edit
                int editChoice;
                cout << "What do you want to edit?" << endl;
                cout << "1. Mood Type" << endl;
                cout << "2. Mood Level" << endl;
                cout << "3. Note" << endl;
                cout << "4. Edit All" << endl;
                cin >> editChoice;

                switch (editChoice)
                {
                case 1:
                    cout << "Enter new mood type:" << endl;
                    cin >> moods[i].TodaysMood.MoodType;
                    break;
                case 2:
                    cout << "Enter new mood level: " << endl;
                    cin >> moods[i].TodaysMood.MoodLevel;
                    break;
                case 3:
                    cout << "Enter new note: " << endl;
                    cin.ignore();
                    getline(cin, moods[i].TodaysMood.Note);

                    break;
                case 4:
                    cout << "Enter new mood type:" << endl;
                    cin >> moods[i].TodaysMood.MoodType;
                    cout << "Enter new mood level: " << endl;
                    cin >> moods[i].TodaysMood.MoodLevel;
                    cout << "Enter new note: " << endl;
                    cin.ignore();
                    getline(cin, moods[i].TodaysMood.Note);
                    break;
                default:
                    cout << "Invalid choice!" << endl;

                }
                // step 6 : confirmation 
                cout << "Mood updated successfully!" << endl;
                break;
            }



        }
        if (found == false) {
            cout << "No entry found!" << endl;
            cout << "Do you want to try again? (y/n): " << endl;
            cin >> tryAgain;
        }


    } while (tryAgain == 'y' || tryAgain == 'Y');
}
//======================================================================================================================
// function deleteMood definition 
// step 1 : ask for date 
// step 2 : validate date
// step 3 : search
// step 4 : display current entry 
// step 5 :ask for confirmation 
// step 6 :delete the entry and re-arrange the array
// step 7 :confirm 
void deleteMood()
{
    char tryAgain = 'n';
    do
    {

        // step 1 : Ask for date
        int searchDay, searchMonth;
        cout << "Please enter the date you want to delete" << endl;
        cin >> searchDay >> searchMonth;

        // step 2 : validate date
           // first validate month
        while (searchMonth > 12 || searchMonth < 1)
        {
            cout << "Invalid month! Please re-enter the date you want to edit" << endl;
            cin >> searchDay >> searchMonth;
        }

        // second validate day
        while (searchDay > 31 || searchDay < 1)
        {
            cout << "Invalid day! Please re-enter the date you want to edit" << endl;
            cin >> searchDay >> searchMonth;
        }
        // step 3 : search
        bool found = false;
        for (int i = 0; i < ArrCount; i++)
        {
            if (moods[i].TodaysDate.day == searchDay && moods[i].TodaysDate.month == searchMonth)
            {

                found = true;


                // step 4 : display current entry
                cout << "Entry found! Here are the current details:" << endl;
                cout << "Day:" << moods[i].TodaysDate.day << endl;
                cout << "Month:" << moods[i].TodaysDate.month << endl;
                cout << "Mood Type:" << moods[i].TodaysMood.MoodType << endl;
                cout << "Mood Level:" << moods[i].TodaysMood.MoodLevel << endl;
                cout << "Note:" << moods[i].TodaysMood.Note << endl;

                // step 5 :ask for confirmation
                char confirm;
                cout << "Are you sure you want to delete this entry? (y/n) :" << endl;
                cin >> confirm;
                if (confirm == 'y' || confirm == 'Y')
                {
                    // step 6 : delete and re-arrange
                    for (int j = i; j < ArrCount - 1; j++)
                    {
                        moods[j] = moods[j + 1];
                    }
                    ArrCount--;
                    moodCount--;

                    // step 7 : confirm
                    cout << "Mood deleted successfully!" << endl;
                }



                break;
            }
        }


        if (found == false) {
            cout << "No entry found!" << endl;
            cout << "Do you want to try again? (y/n): " << endl;
            cin >> tryAgain;
        }


    } while (tryAgain == 'y' || tryAgain == 'Y');
}