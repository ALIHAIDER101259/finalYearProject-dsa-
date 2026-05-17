int main()
{
  systemLogin();

  FlightRecord *root = nullptr;

  root = createFlight(root, 101, 50, 15000);

  root = createFlight(root, 105, 40, 18000);

  root = createFlight(root, 202, 30, 22000);

  root = createFlight(root, 303, 25, 80000);

  int choice;

  while (true)
  {
    cout << "\n1.Display\n2.Book\n3.Cancel\n4.Save\n5.Exit\n6.Undo Log\n7.Undo\n8.Waitlist\n";
    cin >> choice;

    if (choice == 1)

      showFlightSchedule(root);

    else if (choice == 2)

      processBooking(root);

    else if (choice == 3)

      processCancellation(root);

    else if (choice == 4)

      exportData(root);

    else if (choice == 5)

      break;

    else if (choice == 6)

      showUndoLog();

    else if (choice == 7)

      undoLastCancellation(root);

    else if (choice == 8)
      showWaitlist();

    else if (choice == 9)
    {
      string name;

      cin.ignore();
      cout << "Enter Name: ";

      getline(cin, name);

      searchPassengerByName(root, name);
    }
  }

  return 0;
}