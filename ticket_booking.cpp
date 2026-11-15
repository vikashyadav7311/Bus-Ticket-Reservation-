#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <ctime>
#include <iomanip>
using namespace std;

const int MAX_SEATS = 50;
const int MAX_PER_BOOKING = 10;

struct Bus {
    int number;
    char source[50];
    char destination[50];
    int totalSeats;
    int availableSeats;
    float fare;
    bool seatTaken[MAX_SEATS + 1];
    Bus* next;
};

struct Booking {
    int id;
    char username[50];
    int busNumber;
    int seats;
    int seatNo[MAX_PER_BOOKING];
    Booking* next;
};

struct WaitNode {
    char username[50];
    int busNumber;
    int seats;
    WaitNode* next;
};

struct Transaction {
    char action[10];
    int busNumber;
    int seats;
    char username[50];
    int bookingId;
    int seatNo[MAX_PER_BOOKING];
    Transaction* next;
};

struct User {
    char username[50];
    char password[50];
};

Bus* busHead = NULL;
Booking* bookingHead = NULL;
WaitNode* waitFront = NULL;
WaitNode* waitRear = NULL;
Transaction* stackTop = NULL;
int nextBookingId = 1;
bool inputEnded = false;

int readInt();
void readText(char text[], int size);
int loginUser(User users[], int count, const char name[], const char pass[]);
void showMainMenu();
void showUserMenu(bool isAdmin);

Bus* createDefaultBuses();
Bus* findBus(int number);
void displayAllBuses();
void searchBuses();
void sortByFare();
void sortByNumber();
void displaySeatMap();
void addBus(const char adminName[]);
void deleteBus(const char adminName[]);
void saveBuses();
bool loadBuses();

Booking* findBooking(int id);
void addBookingToList(Booking* booking);
void removeBooking(int id);
Booking* makeBooking(Bus* bus, const char username[], int seats);
void releaseSeats(Bus* bus, Booking* booking);
bool showUserBookings(const char username[]);
void bookTicket(const char username[]);
void cancelTicket(const char username[]);
void saveBookings();
void loadBookings();

void enqueue(const char username[], int busNumber, int seats);
void removeWaitNode(WaitNode* target);
int waitlistSize();
void processWaitlist(Bus* bus);
void displayWaitlist(const char username[]);
void saveWaitlist();
void loadWaitlist();

void pushTransaction(const char action[], int busNumber, int seats, const char username[],
                     int bookingId, int seatNos[]);
void popTransaction();
void displayHistory();
void writeLog(const char action[], const char username[], int busNumber, int seats, int id);
void undoLast(const char username[]);

void statusMenu(const char username[]);
void saveAll();
void freeAll();

int main() {
    User users[4] = { {"user1", "123"}, {"user2", "123"}, {"user3", "123"}, {"admin", "admin123"} };
    bool loggedIn = false;
    bool isAdmin = false;
    char currentUser[50] = "";
    int failedAttempts = 0;
    int choice;
    char name[50];
    char pass[50];

    if (loadBuses() == true) {
        loadBookings();
        loadWaitlist();
    } else {
        busHead = createDefaultBuses();
    }

    while (inputEnded == false) {
        if (loggedIn == false) {
            showMainMenu();
            choice = readInt();
            if (inputEnded == true) {
                break;
            }
            if (choice == 1) {
                cout << "Username: ";
                readText(name, 50);
                cout << "Password: ";
                readText(pass, 50);
                if (loginUser(users, 4, name, pass) != -1) {
                    strcpy(currentUser, name);
                    loggedIn = true;
                    failedAttempts = 0;
                    if (strcmp(currentUser, "admin") == 0) {
                        isAdmin = true;
                    } else {
                        isAdmin = false;
                    }
                    cout << "\n***** Welcome, " << currentUser << "! *****\n";
                } else {
                    failedAttempts++;
                    cout << "\n....Login failed.....\n";
                    if (failedAttempts >= 3) {
                        cout << "Too many failed attempts. Program closed.\n";
                        break;
                    }
                    cout << (3 - failedAttempts) << " attempt(s) left.\n";
                }
            } else if (choice == 2) {
                cout << "\nGoodbye!\n";
                break;
            } else {
                cout << "\n*** Invalid choice! ***\n";
            }
        } else {
            showUserMenu(isAdmin);
            choice = readInt();
            if (inputEnded == true) {
                break;
            }
            if (choice == 1) {
                bookTicket(currentUser);
                saveAll();
            } else if (choice == 2) {
                cancelTicket(currentUser);
                saveAll();
            } else if (choice == 3) {
                statusMenu(currentUser);
            } else if (choice == 4) {
                displayWaitlist(currentUser);
            } else if (choice == 5) {
                displayHistory();
            } else if (choice == 6) {
                undoLast(currentUser);
                saveAll();
            } else if (choice == 7) {
                loggedIn = false;
                saveAll();
                cout << "\n*** Logged out ***\n";
            } else if (choice == 8 && isAdmin == true) {
                addBus(currentUser);
            } else if (choice == 9 && isAdmin == true) {
                deleteBus(currentUser);
            } else {
                cout << "\n*** Invalid choice! ***\n";
            }
        }
    }

    saveAll();
    freeAll();
    return 0;
}

int readInt() {
    int number;
    cin >> number;
    if (cin.eof()) {
        inputEnded = true;
        return -1;
    }
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
        return -1;
    }
    cin.ignore(1000, '\n');
    if (number < 0) {
        return -1;
    }
    return number;
}

void readText(char text[], int size) {
    cin.getline(text, size);
    if (cin.eof()) {
        inputEnded = true;
        text[0] = '\0';
        return;
    }
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

int loginUser(User users[], int count, const char name[], const char pass[]) {
    int i;
    for (i = 0; i < count; i++) {
        if (strcmp(users[i].username, name) == 0 && strcmp(users[i].password, pass) == 0) {
            return i;
        }
    }
    return -1;
}

void showMainMenu() {
    cout << "\n===== Main Menu =====\n";
    cout << "1. Login\n";
    cout << "2. Exit\n";
    cout << "Enter choice: ";
}

void showUserMenu(bool isAdmin) {
    cout << "\n===== Bus Ticket Reservation Menu =====\n";
    cout << "1. Book Ticket\n";
    cout << "2. Cancel Ticket\n";
    cout << "3. Check Status\n";
    cout << "4. View Waitlist\n";
    cout << "5. Transaction History\n";
    cout << "6. Undo Last Action\n";
    cout << "7. Logout\n";
    if (isAdmin == true) {
        cout << "8. Add New Bus (Admin only)\n";
        cout << "9. Delete Bus (Admin only)\n";
    }
    cout << "Enter choice: ";
}

Bus* createDefaultBuses() {
    const char* sources[5] = {"ISBT", "Clement Town", "Dehradun", "ISBT", "Dehradun"};
    const char* destinations[5] = {"Rishikesh", "Haridwar", "Mussoorie", "Paonta", "Vikasnagar"};
    int seats[5] = {43, 35, 30, 50, 25};
    float fares[5] = {80, 120, 70, 150, 60};
    Bus* head = NULL;
    Bus* tail = NULL;
    Bus* bus;
    int i, j;

    for (i = 0; i < 5; i++) {
        bus = new Bus;
        bus->number = i + 1;
        strcpy(bus->source, sources[i]);
        strcpy(bus->destination, destinations[i]);
        bus->totalSeats = seats[i];
        bus->availableSeats = seats[i];
        bus->fare = fares[i];
        for (j = 0; j <= MAX_SEATS; j++) {
            bus->seatTaken[j] = false;
        }
        bus->next = NULL;

        if (head == NULL) {
            head = bus;
            tail = bus;
        } else {
            tail->next = bus;
            tail = bus;
        }
    }
    return head;
}

Bus* findBus(int number) {
    Bus* temp = busHead;
    while (temp != NULL) {
        if (temp->number == number) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void printBusHeader() {
    cout << "No  | From         | To           | Fare   | Total | Available\n";
}

void printBusRow(Bus* temp) {
    cout << setw(3) << temp->number << " | " << left << setw(12) << temp->source << " | "
         << setw(12) << temp->destination << " | " << right << setw(6) << fixed
         << setprecision(2) << temp->fare << " | " << setw(5) << temp->totalSeats
         << " | " << setw(9) << temp->availableSeats << "\n";
}

void displayAllBuses() {
    Bus* temp = busHead;
    cout << "\n============== Bus List ==============\n";
    printBusHeader();
    while (temp != NULL) {
        printBusRow(temp);
        temp = temp->next;
    }
}

void makeLower(const char from[], char to[]) {
    int i = 0;
    while (from[i] != '\0') {
        to[i] = tolower(from[i]);
        i++;
    }
    to[i] = '\0';
}

void searchBuses() {
    char source[50];
    char destination[50];
    char lowSource[50];
    char lowDestination[50];
    char busSource[50];
    char busDestination[50];
    Bus* temp;
    int found = 0;

    cout << "Enter source (Enter to skip): ";
    readText(source, 50);
    cout << "Enter destination (Enter to skip): ";
    readText(destination, 50);
    makeLower(source, lowSource);
    makeLower(destination, lowDestination);

    cout << "\n============== Search Result ==============\n";
    printBusHeader();
    temp = busHead;
    while (temp != NULL) {
        makeLower(temp->source, busSource);
        makeLower(temp->destination, busDestination);
        if (strstr(busSource, lowSource) != NULL && strstr(busDestination, lowDestination) != NULL) {
            printBusRow(temp);
            found++;
        }
        temp = temp->next;
    }
    if (found == 0) {
        cout << "    (no bus found)\n";
    }
}

void swapBusData(Bus* a, Bus* b) {
    Bus copyOfA = *a;
    Bus* nextOfA = a->next;
    Bus* nextOfB = b->next;
    *a = *b;
    *b = copyOfA;
    a->next = nextOfA;
    b->next = nextOfB;
}

void sortByFare() {
    bool swapped = true;
    Bus* temp;
    while (swapped == true) {
        swapped = false;
        temp = busHead;
        while (temp != NULL && temp->next != NULL) {
            if (temp->fare > temp->next->fare) {
                swapBusData(temp, temp->next);
                swapped = true;
            }
            temp = temp->next;
        }
    }
}

void sortByNumber() {
    bool swapped = true;
    Bus* temp;
    while (swapped == true) {
        swapped = false;
        temp = busHead;
        while (temp != NULL && temp->next != NULL) {
            if (temp->number > temp->next->number) {
                swapBusData(temp, temp->next);
                swapped = true;
            }
            temp = temp->next;
        }
    }
}

void displaySeatMap() {
    int number;
    Bus* bus;
    int i;

    cout << "Enter Bus number: ";
    number = readInt();
    bus = findBus(number);
    if (bus == NULL) {
        cout << "\n*** Invalid number. ***\n";
        return;
    }

    cout << "\nSeat map of Bus #" << bus->number << " (" << bus->source << " to "
         << bus->destination << "), XX = booked\n";
    for (i = 1; i <= bus->totalSeats; i++) {
        if (bus->seatTaken[i] == true) {
            cout << "[XX]";
        } else {
            cout << "[" << setfill('0') << setw(2) << i << setfill(' ') << "]";
        }
        if (i % 5 == 0 || i == bus->totalSeats) {
            cout << "\n";
        } else {
            cout << " ";
        }
    }
}

void addBus(const char adminName[]) {
    char source[50];
    char destination[50];
    int total, fare, number, j;
    Bus* bus;
    Bus* last;
    Bus* temp;

    cout << "Enter source: ";
    readText(source, 50);
    cout << "Enter destination: ";
    readText(destination, 50);
    if (source[0] == '\0' || destination[0] == '\0') {
        cout << "\n*** Source and destination are required. ***\n";
        return;
    }

    cout << "Enter total seats (1-" << MAX_SEATS << "): ";
    total = readInt();
    if (total < 1 || total > MAX_SEATS) {
        cout << "\n*** Invalid number of seats. ***\n";
        return;
    }

    cout << "Enter fare: ";
    fare = readInt();
    if (fare <= 0) {
        cout << "\n*** Invalid fare. ***\n";
        return;
    }

    number = 1;
    temp = busHead;
    while (temp != NULL) {
        if (temp->number >= number) {
            number = temp->number + 1;
        }
        temp = temp->next;
    }

    bus = new Bus;
    bus->number = number;
    strcpy(bus->source, source);
    strcpy(bus->destination, destination);
    bus->totalSeats = total;
    bus->availableSeats = total;
    bus->fare = fare;
    for (j = 0; j <= MAX_SEATS; j++) {
        bus->seatTaken[j] = false;
    }
    bus->next = NULL;

    if (busHead == NULL) {
        busHead = bus;
    } else {
        last = busHead;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = bus;
    }

    saveBuses();
    writeLog("ADDBUS", adminName, number, total, 0);
    cout << "\n*** Bus #" << number << " added successfully! ***\n";
}

void deleteBus(const char adminName[]) {
    int number, answer;
    Bus* bus;
    Bus* previous;
    Booking* booking;
    WaitNode* wait;
    WaitNode* nextWait;

    displayAllBuses();
    cout << "Enter Bus number to delete: ";
    number = readInt();
    bus = findBus(number);
    if (bus == NULL) {
        cout << "\n*** Invalid number. ***\n";
        return;
    }
    if (busHead->next == NULL) {
        cout << "\n*** At least one bus must remain. ***\n";
        return;
    }

    booking = bookingHead;
    while (booking != NULL) {
        if (booking->busNumber == number) {
            cout << "\n*** Bus #" << number << " has active bookings. Cancel them first. ***\n";
            return;
        }
        booking = booking->next;
    }

    cout << "Delete Bus #" << number << " (" << bus->source << " to "
         << bus->destination << ")? (1 = Yes, 0 = No): ";
    answer = readInt();
    if (answer != 1) {
        cout << "\n*** Bus not deleted. ***\n";
        return;
    }

    if (bus == busHead) {
        busHead = bus->next;
    } else {
        previous = busHead;
        while (previous->next != bus) {
            previous = previous->next;
        }
        previous->next = bus->next;
    }
    delete bus;

    wait = waitFront;
    while (wait != NULL) {
        nextWait = wait->next;
        if (wait->busNumber == number) {
            removeWaitNode(wait);
        }
        wait = nextWait;
    }

    saveBuses();
    saveWaitlist();
    writeLog("DELBUS", adminName, number, 0, 0);
    cout << "\n*** Bus #" << number << " deleted successfully! ***\n";
}

void saveBuses() {
    ofstream file("buses.txt");
    Bus* temp = busHead;
    int i;

    while (temp != NULL) {
        file << temp->number << "\n";
        file << temp->source << "\n";
        file << temp->destination << "\n";
        file << temp->totalSeats << " " << temp->fare << "\n";
        for (i = 1; i <= temp->totalSeats; i++) {
            file << temp->seatTaken[i] << " ";
        }
        file << "\n";
        temp = temp->next;
    }
    file.close();
}

bool loadBuses() {
    ifstream file("buses.txt");
    Bus* tail = NULL;
    Bus* bus;
    int number, i, flag;

    if (!file) {
        return false;
    }

    while (file >> number) {
        bus = new Bus;
        bus->number = number;
        file.ignore();
        file.getline(bus->source, 50);
        file.getline(bus->destination, 50);
        file >> bus->totalSeats >> bus->fare;
        if (bus->totalSeats < 1 || bus->totalSeats > MAX_SEATS) {
            delete bus;
            break;
        }

        bus->availableSeats = 0;
        for (i = 0; i <= MAX_SEATS; i++) {
            bus->seatTaken[i] = false;
        }
        for (i = 1; i <= bus->totalSeats; i++) {
            file >> flag;
            if (flag == 1) {
                bus->seatTaken[i] = true;
            } else {
                bus->availableSeats++;
            }
        }
        bus->next = NULL;

        if (busHead == NULL) {
            busHead = bus;
        } else {
            tail->next = bus;
        }
        tail = bus;
    }
    file.close();

    if (busHead == NULL) {
        return false;
    }
    return true;
}

Booking* findBooking(int id) {
    Booking* temp = bookingHead;
    while (temp != NULL) {
        if (temp->id == id) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void addBookingToList(Booking* booking) {
    Booking* last;
    booking->next = NULL;
    if (bookingHead == NULL) {
        bookingHead = booking;
    } else {
        last = bookingHead;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = booking;
    }
}

void removeBooking(int id) {
    Booking* previous = NULL;
    Booking* current = bookingHead;

    while (current != NULL) {
        if (current->id == id) {
            if (previous == NULL) {
                bookingHead = current->next;
            } else {
                previous->next = current->next;
            }
            delete current;
            return;
        }
        previous = current;
        current = current->next;
    }
}

Booking* makeBooking(Bus* bus, const char username[], int seats) {
    Booking* booking = new Booking;
    int given = 0;
    int i;

    booking->id = nextBookingId;
    nextBookingId++;
    strcpy(booking->username, username);
    booking->busNumber = bus->number;
    booking->seats = seats;

    for (i = 1; i <= bus->totalSeats; i++) {
        if (given < seats && bus->seatTaken[i] == false) {
            bus->seatTaken[i] = true;
            booking->seatNo[given] = i;
            given++;
        }
    }
    bus->availableSeats = bus->availableSeats - seats;
    addBookingToList(booking);
    return booking;
}

void releaseSeats(Bus* bus, Booking* booking) {
    int i;
    for (i = 0; i < booking->seats; i++) {
        bus->seatTaken[booking->seatNo[i]] = false;
    }
    bus->availableSeats = bus->availableSeats + booking->seats;
}

void printSeatList(int seatNo[], int count) {
    int i;
    for (i = 0; i < count; i++) {
        cout << seatNo[i];
        if (i < count - 1) {
            cout << ", ";
        }
    }
}

bool showUserBookings(const char username[]) {
    Booking* temp = bookingHead;
    int count = 0;

    cout << "\n===== Your Bookings =====\n";
    while (temp != NULL) {
        if (strcmp(temp->username, username) == 0) {
            cout << "    ID " << temp->id << " | Bus #" << temp->busNumber << " | "
                 << temp->seats << " seat(s): ";
            printSeatList(temp->seatNo, temp->seats);
            cout << "\n";
            count++;
        }
        temp = temp->next;
    }
    if (count == 0) {
        cout << "    (you have no bookings)\n";
        return false;
    }
    return true;
}

void bookTicket(const char username[]) {
    int number, seats, answer;
    Bus* bus;
    Booking* booking;

    displayAllBuses();
    cout << "Enter Bus number to book: ";
    number = readInt();
    bus = findBus(number);
    if (bus == NULL) {
        cout << "\n*** Invalid number. ***\n";
        return;
    }

    cout << "Enter seats to book (1-" << MAX_PER_BOOKING << "): ";
    seats = readInt();
    if (seats < 1 || seats > MAX_PER_BOOKING) {
        cout << "\n*** Invalid number of seats. ***\n";
        return;
    }

    if (seats > bus->availableSeats) {
        cout << "\n*** Only " << bus->availableSeats << " seat(s) available. ***\n";
        cout << "Join the waitlist for this bus? (1 = Yes, 0 = No): ";
        answer = readInt();
        if (answer == 1) {
            enqueue(username, bus->number, seats);
            pushTransaction("WAIT", bus->number, seats, username, 0, NULL);
            writeLog("WAIT", username, bus->number, seats, 0);
            cout << "\n*** Added to waitlist at position " << waitlistSize() << ". ***\n";
        } else {
            cout << "\n*** Booking not done. ***\n";
        }
        return;
    }

    booking = makeBooking(bus, username, seats);
    pushTransaction("BOOK", bus->number, seats, username, booking->id, booking->seatNo);
    writeLog("BOOK", username, bus->number, seats, booking->id);
    cout << "\n*** Bus ticket booked successfully! ***\n";
    cout << "Booking ID: " << booking->id << " | Seat no.: ";
    printSeatList(booking->seatNo, booking->seats);
    cout << "\n";
}

void cancelTicket(const char username[]) {
    int id, busNumber, seats, i;
    int seatNo[MAX_PER_BOOKING];
    Booking* booking;
    Bus* bus;

    if (showUserBookings(username) == false) {
        return;
    }

    cout << "Enter Booking ID to cancel: ";
    id = readInt();
    booking = findBooking(id);
    if (booking == NULL || strcmp(booking->username, username) != 0) {
        cout << "\n*** No such booking found for your account. ***\n";
        return;
    }

    bus = findBus(booking->busNumber);
    busNumber = booking->busNumber;
    seats = booking->seats;
    for (i = 0; i < seats; i++) {
        seatNo[i] = booking->seatNo[i];
    }

    if (bus != NULL) {
        releaseSeats(bus, booking);
    }
    removeBooking(id);
    pushTransaction("CANCEL", busNumber, seats, username, id, seatNo);
    writeLog("CANCEL", username, busNumber, seats, id);
    cout << "\n*** Booking " << id << " cancelled. ***\n";

    if (bus != NULL) {
        processWaitlist(bus);
    }
}

void saveBookings() {
    ofstream file("bookings.txt");
    Booking* temp = bookingHead;
    int i;

    while (temp != NULL) {
        file << temp->id << " " << temp->username << " " << temp->busNumber << " " << temp->seats;
        for (i = 0; i < temp->seats; i++) {
            file << " " << temp->seatNo[i];
        }
        file << "\n";
        temp = temp->next;
    }
    file.close();
}

void loadBookings() {
    ifstream file("bookings.txt");
    Booking* booking;
    char username[50];
    int id, busNumber, seats, i;

    if (!file) {
        return;
    }

    while (file >> id >> username >> busNumber >> seats) {
        if (seats < 1 || seats > MAX_PER_BOOKING) {
            break;
        }
        booking = new Booking;
        booking->id = id;
        strcpy(booking->username, username);
        booking->busNumber = busNumber;
        booking->seats = seats;
        for (i = 0; i < seats; i++) {
            file >> booking->seatNo[i];
        }
        if (findBus(busNumber) == NULL) {
            delete booking;
        } else {
            addBookingToList(booking);
            if (id >= nextBookingId) {
                nextBookingId = id + 1;
            }
        }
    }
    file.close();
}

void enqueue(const char username[], int busNumber, int seats) {
    WaitNode* node = new WaitNode;
    strcpy(node->username, username);
    node->busNumber = busNumber;
    node->seats = seats;
    node->next = NULL;

    if (waitRear == NULL) {
        waitFront = node;
        waitRear = node;
    } else {
        waitRear->next = node;
        waitRear = node;
    }
}

void removeWaitNode(WaitNode* target) {
    WaitNode* previous = NULL;
    WaitNode* current = waitFront;

    while (current != NULL) {
        if (current == target) {
            if (previous == NULL) {
                waitFront = current->next;
            } else {
                previous->next = current->next;
            }
            if (waitRear == current) {
                waitRear = previous;
            }
            delete current;
            return;
        }
        previous = current;
        current = current->next;
    }
}

int waitlistSize() {
    WaitNode* temp = waitFront;
    int count = 0;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

void processWaitlist(Bus* bus) {
    WaitNode* wait = waitFront;
    WaitNode* nextWait;
    Booking* booking;

    while (wait != NULL) {
        nextWait = wait->next;
        if (wait->busNumber == bus->number && wait->seats <= bus->availableSeats) {
            booking = makeBooking(bus, wait->username, wait->seats);
            pushTransaction("AUTO", bus->number, booking->seats, booking->username,
                            booking->id, booking->seatNo);
            writeLog("AUTO", booking->username, bus->number, booking->seats, booking->id);
            cout << "*** Waitlist: " << booking->username << " got " << booking->seats
                 << " seat(s) on Bus #" << bus->number << " (Booking ID "
                 << booking->id << "). ***\n";
            removeWaitNode(wait);
        }
        wait = nextWait;
    }
}

void displayWaitlist(const char username[]) {
    WaitNode* temp = waitFront;
    int position = 1;

    if (temp == NULL) {
        cout << "\n    (waitlist is empty)\n";
        return;
    }
    cout << "\n===== Waitlist (first in line first) =====\n";
    while (temp != NULL) {
        cout << "    " << position << ". " << temp->username << " - Bus #" << temp->busNumber
             << " - " << temp->seats << " seat(s)";
        if (strcmp(temp->username, username) == 0) {
            cout << "  <-- you";
        }
        cout << "\n";
        position++;
        temp = temp->next;
    }
}

void saveWaitlist() {
    ofstream file("waitlist.txt");
    WaitNode* temp = waitFront;
    while (temp != NULL) {
        file << temp->username << " " << temp->busNumber << " " << temp->seats << "\n";
        temp = temp->next;
    }
    file.close();
}

void loadWaitlist() {
    ifstream file("waitlist.txt");
    char username[50];
    int busNumber, seats;

    if (!file) {
        return;
    }
    while (file >> username >> busNumber >> seats) {
        if (findBus(busNumber) != NULL && seats >= 1 && seats <= MAX_PER_BOOKING) {
            enqueue(username, busNumber, seats);
        }
    }
    file.close();
}

void pushTransaction(const char action[], int busNumber, int seats, const char username[],
                     int bookingId, int seatNos[]) {
    Transaction* node = new Transaction;
    int i;

    strcpy(node->action, action);
    node->busNumber = busNumber;
    node->seats = seats;
    strcpy(node->username, username);
    node->bookingId = bookingId;
    for (i = 0; i < MAX_PER_BOOKING; i++) {
        if (seatNos != NULL && i < seats) {
            node->seatNo[i] = seatNos[i];
        } else {
            node->seatNo[i] = 0;
        }
    }
    node->next = stackTop;
    stackTop = node;
}

void popTransaction() {
    Transaction* old;
    if (stackTop == NULL) {
        return;
    }
    old = stackTop;
    stackTop = stackTop->next;
    delete old;
}

void displayHistory() {
    Transaction* temp = stackTop;
    if (temp == NULL) {
        cout << "\n    (no transactions yet)\n";
        return;
    }
    cout << "\n===== Transaction History (latest first) =====\n";
    while (temp != NULL) {
        cout << "    [" << temp->action << "] Bus #" << temp->busNumber
             << " - " << temp->seats << " seat(s) by " << temp->username
             << " (Booking ID " << temp->bookingId << ")\n";
        temp = temp->next;
    }
}

void writeLog(const char action[], const char username[], int busNumber, int seats, int id) {
    ofstream file("transaction_log.txt", ios::app);
    time_t now = time(NULL);
    char* timeText = ctime(&now);

    timeText[strlen(timeText) - 1] = '\0';
    file << timeText << " | " << action << " | " << username << " | Bus " << busNumber
         << " | Seats " << seats << " | ID " << id << "\n";
    file.close();
}

void undoBooking(Transaction t, Bus* bus) {
    Booking* booking = findBooking(t.bookingId);

    if (booking != NULL && bus != NULL) {
        releaseSeats(bus, booking);
        removeBooking(t.bookingId);
    }
    popTransaction();
    writeLog("UNDO-BOOK", t.username, t.busNumber, t.seats, t.bookingId);
    cout << "\n*** Last booking undone. Seats released. ***\n";
    if (bus != NULL) {
        processWaitlist(bus);
    }
}

void undoCancel(Transaction t, Bus* bus) {
    Booking* booking;
    bool canRestore = true;
    int i;

    if (bus == NULL || findBooking(t.bookingId) != NULL) {
        canRestore = false;
    } else {
        for (i = 0; i < t.seats; i++) {
            if (bus->seatTaken[t.seatNo[i]] == true) {
                canRestore = false;
            }
        }
    }
    if (canRestore == false) {
        cout << "\n*** Cannot undo: those seats are no longer free. ***\n";
        return;
    }

    booking = new Booking;
    booking->id = t.bookingId;
    strcpy(booking->username, t.username);
    booking->busNumber = t.busNumber;
    booking->seats = t.seats;
    for (i = 0; i < t.seats; i++) {
        booking->seatNo[i] = t.seatNo[i];
        bus->seatTaken[t.seatNo[i]] = true;
    }
    bus->availableSeats = bus->availableSeats - t.seats;
    addBookingToList(booking);

    popTransaction();
    writeLog("UNDO-CANC", t.username, t.busNumber, t.seats, t.bookingId);
    cout << "\n*** Cancellation undone. Booking " << t.bookingId << " restored. ***\n";
}

void undoWait(Transaction t) {
    WaitNode* temp = waitFront;
    WaitNode* match = NULL;

    while (temp != NULL) {
        if (strcmp(temp->username, t.username) == 0 && temp->busNumber == t.busNumber
            && temp->seats == t.seats) {
            match = temp;
        }
        temp = temp->next;
    }
    if (match != NULL) {
        removeWaitNode(match);
    }
    popTransaction();
    writeLog("UNDO-WAIT", t.username, t.busNumber, t.seats, 0);
    cout << "\n*** Waitlist entry removed. ***\n";
}

void undoLast(const char username[]) {
    Transaction t;
    Bus* bus;

    if (stackTop == NULL) {
        cout << "\n*** Nothing to undo. ***\n";
        return;
    }
    if (strcmp(stackTop->username, username) != 0) {
        cout << "\n*** The last action belongs to another user. ***\n";
        return;
    }

    t = *stackTop;
    bus = findBus(t.busNumber);

    if (strcmp(t.action, "BOOK") == 0) {
        undoBooking(t, bus);
    } else if (strcmp(t.action, "CANCEL") == 0) {
        undoCancel(t, bus);
    } else if (strcmp(t.action, "WAIT") == 0) {
        undoWait(t);
    } else {
        cout << "\n*** Automatic waitlist booking cannot be undone. ***\n";
    }
}

void statusMenu(const char username[]) {
    int choice;

    while (inputEnded == false) {
        cout << "\n===== Check Status =====\n";
        cout << "1. View All Buses\n";
        cout << "2. Search Buses\n";
        cout << "3. Sort Buses by Fare\n";
        cout << "4. Sort Buses by Number\n";
        cout << "5. Seat Map of a Bus\n";
        cout << "6. My Bookings\n";
        cout << "0. Back\n";
        cout << "Enter choice: ";
        choice = readInt();

        if (choice == 1) {
            displayAllBuses();
        } else if (choice == 2) {
            searchBuses();
        } else if (choice == 3) {
            sortByFare();
            displayAllBuses();
        } else if (choice == 4) {
            sortByNumber();
            displayAllBuses();
        } else if (choice == 5) {
            displaySeatMap();
        } else if (choice == 6) {
            showUserBookings(username);
        } else if (choice == 0) {
            return;
        } else if (inputEnded == false) {
            cout << "\n*** Invalid choice! ***\n";
        }
    }
}

void saveAll() {
    saveBuses();
    saveBookings();
    saveWaitlist();
}

void freeAll() {
    Bus* oldBus;
    Booking* oldBooking;

    while (busHead != NULL) {
        oldBus = busHead;
        busHead = busHead->next;
        delete oldBus;
    }
    while (bookingHead != NULL) {
        oldBooking = bookingHead;
        bookingHead = bookingHead->next;
        delete oldBooking;
    }
    while (waitFront != NULL) {
        removeWaitNode(waitFront);
    }
    while (stackTop != NULL) {
        popTransaction();
    }
}
