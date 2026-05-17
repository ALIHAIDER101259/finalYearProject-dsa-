#include <iostream>

#include <string>

#include <fstream>

#include <stdexcept>

using namespace std;

struct Passenger
{
  int passengerId;

  string fullName;

  int age;

  int seatNo;

  Passenger *next;
};

struct FlightRecord

{
  int flightID;

  int maxCapacity;

  int reservedSeats;

  double ticketPrice;

  Passenger *passengerList;

  FlightRecord *left, *right;
};

struct UndoLog
{
  int fID;
  int pID;
  string pName;
  UndoLog *next;
};

UndoLog *undoStack = nullptr;

struct WaitlistNode
{
  string name;
  int fID;
  WaitlistNode *next;
};

WaitlistNode *queueFront = nullptr;
WaitlistNode *queueRear = nullptr;

// ---------------- LOGIN ----------------
void systemLogin()
{
  string user;

  string pass;
  int attempts = 0;

  while (attempts < 3)
  {
    cout << "\n----------- SYSTEM AUTHENTICATION -----------" << endl;
    cout << "Username: ";
    cin >> user;
    cout << "Password: ";
    cin >> pass;

    try
    {
      if (user == "admin" && pass == "riphah123")
      {

        cout << "\nAccess Granted!" << endl;
        return;
      }
      else
      {

        throw runtime_error("Invalid Credentials!");
      }
    }
    catch (const runtime_error &e)
    {
      attempts++;

      cout << "Error: " << e.what() << endl;

      cout << "Attempts left: " << (3 - attempts) << endl;

      if (attempts == 3)
      {
        cout << "System Locked!" << endl;
        exit(0);
      }
    }
  }
}

// ---------------- STACK ----------------
void addToUndoStack(int f, int id, string n)
{

  UndoLog *newNode = new UndoLog();
  newNode->fID = f;
  newNode->pID = id;
  newNode->pName = n;
  newNode->next = undoStack;
  undoStack = newNode;
}

// ---------------- QUEUE ----------------
void addToWaitlist(string n, int f)
{
  WaitlistNode *newNode = new WaitlistNode();
  newNode->name = n;
  newNode->fID = f;
  newNode->next = nullptr;

  if (queueRear == nullptr)
  {
    queueFront = queueRear = newNode;
  }
  else
  {
    queueRear->next = newNode;
    queueRear = newNode;
  }
}

// ---------------- FILE SAVE ----------------
void saveToDisk(FlightRecord *root, ofstream &file)
{
  if (root == nullptr)
    return;

  saveToDisk(root->left, file);

  Passenger *temp = root->passengerList;
  while (temp != nullptr)
  {
    file << root->flightID << " "
         << temp->passengerId << " "
         << temp->age << " "
         << temp->seatNo << " "
         << temp->fullName << endl;
    temp = temp->next;
  }

  saveToDisk(root->right, file);
}

void exportData(FlightRecord *root)
{
  ofstream file("Database.txt");

  if (!file)
  {
    cout << "File error!" << endl;
    return;
  }

  saveToDisk(root, file);
  file.close();

  cout << "Data saved successfully!" << endl;
}

// ---------------- BST FLIGHT ----------------
FlightRecord *createFlight(FlightRecord *root, int id, int cap, double price)
{
  if (root == nullptr)
  {
    FlightRecord *newNode = new FlightRecord();
    newNode->flightID = id;
    newNode->maxCapacity = cap;
    newNode->ticketPrice = price;
    newNode->reservedSeats = 0;
    newNode->passengerList = nullptr;
    newNode->left = newNode->right = nullptr;
    return newNode;
  }

  if (id < root->flightID)
    root->left = createFlight(root->left, id, cap, price);
  else
    root->right = createFlight(root->right, id, cap, price);

  return root;
}

FlightRecord *findFlight(FlightRecord *root, int id)
{
  if (root == nullptr || root->flightID == id)
    return root;

  if (id < root->flightID)
    return findFlight(root->left, id);

  return findFlight(root->right, id);
}

// ---------------- DISPLAY FLIGHTS ----------------
void showFlightSchedule(FlightRecord *root)
{
  if (root == nullptr)

    return;

  showFlightSchedule(root->left);

  cout << "Flight ID: " << root->flightID
       << " | Seats Left: " << (root->maxCapacity - root->reservedSeats)
       << " | Price: " << root->ticketPrice << endl;

  showFlightSchedule(root->right);
}

// ---------------- BOOKING ----------------
void processBooking(FlightRecord *root)
{
  int id;
  cout << "Enter Flight ID: ";
  cin >> id;

  FlightRecord *target = findFlight(root, id);

  if (target == nullptr)
  {
    cout << "Flight not found!" << endl;
    return;
  }

  if (target->reservedSeats < target->maxCapacity)
  {
    Passenger *p = new Passenger();

    cin.ignore();

    cout << "Name: ";
    getline(cin, p->fullName);

    cout << "Age: ";
    cin >> p->age;

    cout << "ID: ";
    cin >> p->passengerId;

    p->seatNo = ++target->reservedSeats;
    p->next = target->passengerList;
    target->passengerList = p;

    cout << "Booking Confirmed!" << endl;
  }
  else
  {
    string n;
    cin.ignore();

    cout << "Full: ";
    getline(cin, n);

    addToWaitlist(n, id);
    cout << "Added to waitlist!" << endl;
  }
}

// ---------------- CANCELLATION ----------------
void processCancellation(FlightRecord *root)
{
  int fID;

  int pID;

  cout << "Flight ID: ";
  cin >> fID;

  cout << "Passenger ID: ";
  cin >> pID;

  FlightRecord *target = findFlight(root, fID);
  if (target == nullptr)
    return;

  Passenger *curr = target->passengerList, *prev = nullptr;

  while (curr != nullptr && curr->passengerId != pID)
  {
    prev = curr;
    curr = curr->next;
  }

  if (curr != nullptr)
  {
    addToUndoStack(fID, curr->passengerId, curr->fullName);

    if (prev == nullptr)

      target->passengerList = curr->next;
    else

      prev->next = curr->next;

    delete curr;

    target->reservedSeats--;

    cout << "Cancelled!" << endl;
  }
}

// =====================================================
// ================= NEW FUNCTIONS ======================
// =====================================================

// 1. Show Undo History
void showUndoLog()
{
  cout << "\n--- CANCEL HISTORY ---\n";

  if (undoStack == nullptr)
  {
    cout << "Empty\n";
    return;
  }

  UndoLog *temp = undoStack;

  while (temp != nullptr)
  {
    cout << temp->fID << " | "
         << temp->pID << " | "
         << temp->pName << endl;

    temp = temp->next;
  }
}

void searchPassengerByName(FlightRecord *root, string name)
{
  if (root == nullptr)
    return;

  searchPassengerByName(root->left, name);

  Passenger *temp = root->passengerList;

  while (temp != nullptr)
  {
    if (temp->fullName == name)
    {
      cout << "\nPassenger Found!" << endl;

      cout << "Flight ID: " << root->flightID << endl;

      cout << "Passenger ID: " << temp->passengerId << endl;

      cout << "Name: " << temp->fullName << endl;

      cout << "Age: " << temp->age << endl;

      cout << "Seat No: " << temp->seatNo << endl;
      return;
    }
    temp = temp->next;
  }

  searchPassengerByName(root->right, name);
}

// 2. Undo Last Cancel
void undoLastCancellation(FlightRecord *root)
{
  if (undoStack == nullptr)
  {
    cout << "Nothing to undo\n";
    return;
  }

  UndoLog *temp = undoStack;
  FlightRecord *f = findFlight(root, temp->fID);

  if (f != nullptr)
  {
    Passenger *p = new Passenger();

    p->passengerId = temp->pID;

    p->fullName = temp->pName;
    p->age = 0;

    p->seatNo = ++f->reservedSeats;

    p->next = f->passengerList;
    f->passengerList = p;

    cout << "Undo Done!\n";
  }

  undoStack = undoStack->next;
  delete temp;
}

// 3. Show Waitlist
void showWaitlist()
{
  cout << "\n--- WAITLIST ---\n";

  if (queueFront == nullptr)
  {
    cout << "Empty\n";
    return;
  }

  WaitlistNode *temp = queueFront;

  while (temp != nullptr)
  {
    cout << temp->fID << " | " << temp->name << endl;
    temp = temp->next;
  }
}
