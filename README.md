# 🚌 Bus Ticket Reservation System (C++ & DSA)

A console-based bus ticket reservation system written in C++. It supports user login, seat booking and cancellation, a waitlist, undo, and admin controls for managing buses. All data is stored in plain text files, so bookings are kept between runs.

The project is built from scratch using **linked lists, a queue, and a stack** instead of STL containers, which makes it a good example of core data structures in practice.

---

## ✨ Features

**For all users**
- Login with username and password (the program exits after 3 failed attempts)
- Book up to 10 seats in a single booking; seats are assigned automatically (lowest free seat numbers first)
- Cancel a booking using its Booking ID
- View all buses, search by source and/or destination, and sort by fare or bus number
- View a seat map of any bus (`[XX]` = booked)
- View your own bookings
- Join a **waitlist** when a bus doesn't have enough free seats; you are booked automatically when seats free up
- View transaction history (latest first)
- **Undo** your last action (booking, cancellation, or waitlist entry)

**Admin only**
- Add a new bus (source, destination, seats, fare)
- Delete a bus (only if it has no active bookings, and at least one bus must remain)

---

## 🧱 Data Structures Used

| Feature | Data Structure |
|---|---|
| Buses | Singly linked list (`Bus`) |
| Bookings | Singly linked list (`Booking`) |
| Waitlist | Queue, first come first served (`WaitNode`, with front/rear pointers) |
| Transaction history and undo | Stack (`Transaction`) |
| Seat availability | Boolean array per bus (`seatTaken[]`) |
| Sorting | Bubble sort on the linked list (by fare or bus number) |

---

## ⚙️ Getting Started

### Requirements
- A C++ compiler such as `g++` (C++11 or later)

### Compile
```bash
g++ ticket_booking.cpp -o ticket_booking
```

### Run
```bash
./ticket_booking        # Linux / macOS
ticket_booking.exe      # Windows
```

---

## 🔑 Default Login Credentials

| Role | Username | Password |
|---|---|---|
| User | `user1` | `123` |
| User | `user2` | `123` |
| User | `user3` | `123` |
| Admin | `admin` | `admin123` |

> These are hardcoded for demo purposes. See [Limitations](#-limitations).

---

## 🚍 Default Buses

On the first run (when no `buses.txt` exists), these buses are created:

| No | From | To | Fare | Seats |
|---|---|---|---|---|
| 1 | ISBT | Rishikesh | 80 | 43 |
| 2 | Clement Town | Haridwar | 120 | 35 |
| 3 | Dehradun | Mussoorie | 70 | 30 |
| 4 | ISBT | Paonta | 150 | 50 |
| 5 | Dehradun | Vikasnagar | 60 | 25 |

---

## 📋 Menu Overview

```
===== Main Menu =====
1. Login
2. Exit

===== Bus Ticket Reservation Menu =====
1. Book Ticket
2. Cancel Ticket
3. Check Status
4. View Waitlist
5. Transaction History
6. Undo Last Action
7. Logout
8. Add New Bus (Admin only)
9. Delete Bus (Admin only)

===== Check Status =====
1. View All Buses
2. Search Buses
3. Sort Buses by Fare
4. Sort Buses by Number
5. Seat Map of a Bus
6. My Bookings
0. Back
```

### Sample seat map
```
Seat map of Bus #3 (Dehradun to Mussoorie), XX = booked
[XX] [XX] [03] [04] [05]
[06] [07] [08] [09] [10]
...
```

---

## 💾 Data Files

The program creates and updates these files in the folder where it runs:

| File | Purpose |
|---|---|
| `buses.txt` | Bus details and seat status |
| `bookings.txt` | All active bookings |
| `waitlist.txt` | Waitlist queue |
| `transaction_log.txt` | Timestamped log of every action (BOOK, CANCEL, WAIT, AUTO, UNDO-*, ADDBUS, DELBUS) |

Delete `buses.txt`, `bookings.txt`, and `waitlist.txt` to reset the system to its default state.

---

## 🔄 How Key Features Work

**Waitlist**
If you request more seats than are available, you can join the waitlist. Whenever seats are freed (by a cancellation or an undo), the waitlist is scanned in order and any entry that now fits is booked automatically.

**Undo**
Undo works on the most recent action, and only if that action was made by the logged-in user.
- Undo `BOOK`: releases the seats
- Undo `CANCEL`: restores the booking, if those exact seats are still free
- Undo `WAIT`: removes the waitlist entry
- Automatic waitlist bookings (`AUTO`) cannot be undone

**Admin rules for deleting a bus**
A bus can't be deleted if it has active bookings or if it is the only bus left. Waitlist entries for a deleted bus are removed.

---

## 📁 Project Structure

```
.
├── ticket_booking.cpp     # Complete source code
├── README.md
└── (generated at runtime)
    ├── buses.txt
    ├── bookings.txt
    ├── waitlist.txt
    └── transaction_log.txt
```

---

## ⚠️ Limitations

- Credentials are hardcoded and stored in plain text; there is no registration or password hashing
- Usernames with spaces are not supported (they would break the bookings file format)
- Transaction history (used for undo) is kept in memory only and resets when the program closes; the permanent record is `transaction_log.txt`
- Maximum 50 seats per bus and 10 seats per booking
- Fares are entered as whole numbers when adding a bus
- Console only, with no date or time scheduling for trips

---

## 🚀 Possible Improvements

- User registration and password hashing
- Trip dates and times
- Seat selection by the user
- Replace bubble sort with a faster sorting algorithm
- Add a GUI or web front end

---

## 👤 Author

**Archit Semwal**
BJMC Student

---

## 📄 License

This project is open source and available for educational use. Add a license of your choice (for example MIT) before publishing.
