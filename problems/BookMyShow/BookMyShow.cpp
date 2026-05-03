/*
Objects

1. BookMyShowSystem
- vector<Movie*> movies
- vector<Theater*> theaters
- void addMovie(Movie* movie)
- void addTheater(Theater* theater)
- void bookTicket(User* user, Show* show, vector<Seat*> seats)
- void cancelTicket(Ticket* ticket)

2. Movie
- int movieId
- string title
- string genre
- int duration
- vector<Show*> shows
- void addShow(Show* show)
- void removeShow(Show* show)

3. Theater
- int theaterId
- string name
- string location
- vector<Screen*> screens
- void addScreen(Screen* screen)
- void removeScreen(Screen* screen)

4. Screen
- int screenId
- string name
- vector<Seat*> seats
- vector<Show*> shows
- void addShow(Show* show)
- void removeShow(Show* show)

5. Show
- int showId
- Movie* movie
- Screen* screen
- string date
- string showTime
- vector<Seat*> availableSeats
- void bookSeats(vector<Seat*> seats)
- void cancelSeats(vector<Seat*> seats)

6. Seat
- int seatId
- int row
- int column
- bool isBooked
- double price
- void book()
- void cancel()
-> NormalSeat
-> PremiumSeat
-> VIPSeat

7. User
- int userId
- string name
- string email
- vector<Ticket*> tickets
- PaymentMethod* paymentMethod
- void addTicket(Ticket* ticket)
- void removeTicket(Ticket* ticket)

8. Ticket
- int ticketId
- User* user
- Show* show
- vector<Seat*> bookedSeats
- double totalAmount
- string status
- void cancel()

9. PaymentMethod
- void processPayment(double amount)
-> CreditCardPayment
-> UpiPayment

*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Movie;
class Theater;
class Screen;
class Show;
class Seat;
class Ticket;
class User;
class PaymentMethod;

class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;
    virtual bool processPayment(double amount) = 0;
};

class CreditCardPayment : public PaymentMethod {
public:
    bool processPayment(double amount) override {
        cout << "Processing credit card payment of $" << amount << "\n";
        return true;
    }
};

class UpiPayment : public PaymentMethod {
public:
    bool processPayment(double amount) override {
        cout << "Processing UPI payment of $" << amount << "\n";
        return true;
    }
};

class Seat {
protected:
    int seatId;
    int row;
    int column;
    bool isBookedFlag;
    bool isLockedFlag;
    double price;

public:
    Seat(int seatId, int row, int column, double price)
        : seatId(seatId), row(row), column(column), isBookedFlag(false), isLockedFlag(false), price(price) {}

    virtual ~Seat() = default;

    int getSeatId() const { return seatId; }
    int getRow() const { return row; }
    int getColumn() const { return column; }
    double getPrice() const { return price; }
    bool isBooked() const { return isBookedFlag; }
    bool isLocked() const { return isLockedFlag; }

    bool lock() {
        if (isBookedFlag || isLockedFlag) {
            return false;
        }
        isLockedFlag = true;
        return true;
    }

    void unlock() {
        if (!isBookedFlag) {
            isLockedFlag = false;
        }
    }

    void book() {
        isBookedFlag = true;
        isLockedFlag = false;
    }

    void cancel() {
        isBookedFlag = false;
        isLockedFlag = false;
    }

    virtual string getSeatType() const = 0;
};

class NormalSeat : public Seat {
public:
    NormalSeat(int seatId, int row, int column, double price)
        : Seat(seatId, row, column, price) {}

    string getSeatType() const override { return "Normal"; }
};

class PremiumSeat : public Seat {
public:
    PremiumSeat(int seatId, int row, int column, double price)
        : Seat(seatId, row, column, price) {}

    string getSeatType() const override { return "Premium"; }
};

class VIPSeat : public Seat {
public:
    VIPSeat(int seatId, int row, int column, double price)
        : Seat(seatId, row, column, price) {}

    string getSeatType() const override { return "VIP"; }
};

class Movie {
private:
    int movieId;
    string title;
    string genre;
    int duration;
    vector<Show*> shows;

public:
    Movie(int movieId, const string& title, const string& genre, int duration)
        : movieId(movieId), title(title), genre(genre), duration(duration) {}

    int getMovieId() const { return movieId; }
    const string& getTitle() const { return title; }
    const string& getGenre() const { return genre; }
    int getDuration() const { return duration; }

    void addShow(Show* show) {
        if (find(shows.begin(), shows.end(), show) == shows.end()) {
            shows.push_back(show);
        }
    }

    void removeShow(Show* show) {
        shows.erase(remove(shows.begin(), shows.end(), show), shows.end());
    }
};

class Screen {
private:
    int screenId;
    string name;
    vector<Seat*> seats;
    vector<Show*> shows;

public:
    Screen(int screenId, const string& name) : screenId(screenId), name(name) {}

    int getScreenId() const { return screenId; }
    const string& getName() const { return name; }

    void addSeat(Seat* seat) {
        seats.push_back(seat);
    }

    const vector<Seat*>& getSeats() const { return seats; }

    void addShow(Show* show) {
        if (find(shows.begin(), shows.end(), show) == shows.end()) {
            shows.push_back(show);
        }
    }

    void removeShow(Show* show) {
        shows.erase(remove(shows.begin(), shows.end(), show), shows.end());
    }
};

class Show {
private:
    int showId;
    Movie* movie;
    Screen* screen;
    string date;
    string showTime;
    vector<Seat*> availableSeats;

public:
    Show(int showId, Movie* movie, Screen* screen, const string& date, const string& showTime)
        : showId(showId), movie(movie), screen(screen), date(date), showTime(showTime) {
        if (screen) {
            availableSeats = screen->getSeats();
        }
    }

    int getShowId() const { return showId; }
    Movie* getMovie() const { return movie; }
    Screen* getScreen() const { return screen; }
    const string& getDate() const { return date; }
    const string& getShowTime() const { return showTime; }

    const vector<Seat*>& getAvailableSeats() const { return availableSeats; }

    bool lockSeats(const vector<Seat*>& seatsToLock) {
        for (Seat* seat : seatsToLock) {
            if (!seat || seat->isBooked() || seat->isLocked()) {
                return false;
            }
            if (find(availableSeats.begin(), availableSeats.end(), seat) == availableSeats.end()) {
                return false;
            }
        }

        for (Seat* seat : seatsToLock) {
            seat->lock();
            availableSeats.erase(remove(availableSeats.begin(), availableSeats.end(), seat), availableSeats.end());
        }

        return true;
    }

    void unlockSeats(const vector<Seat*>& seatsToUnlock) {
        for (Seat* seat : seatsToUnlock) {
            if (seat && seat->isLocked()) {
                seat->unlock();
                if (!seat->isBooked() && find(availableSeats.begin(), availableSeats.end(), seat) == availableSeats.end()) {
                    availableSeats.push_back(seat);
                }
            }
        }
    }

    bool bookLockedSeats(const vector<Seat*>& seatsToBook) {
        for (Seat* seat : seatsToBook) {
            if (!seat || !seat->isLocked() || seat->isBooked()) {
                return false;
            }
        }

        for (Seat* seat : seatsToBook) {
            seat->book();
        }

        return true;
    }

    bool cancelSeats(const vector<Seat*>& seatsToCancel) {
        for (Seat* seat : seatsToCancel) {
            if (!seat || !seat->isBooked()) {
                return false;
            }
        }

        for (Seat* seat : seatsToCancel) {
            seat->cancel();
            if (find(availableSeats.begin(), availableSeats.end(), seat) == availableSeats.end()) {
                availableSeats.push_back(seat);
            }
        }

        return true;
    }
};

class Ticket {
private:
    int ticketId;
    User* user;
    Show* show;
    vector<Seat*> bookedSeats;
    double totalAmount;
    string status;

public:
    Ticket(int ticketId, User* user, Show* show, const vector<Seat*>& seats)
        : ticketId(ticketId), user(user), show(show), bookedSeats(seats), totalAmount(0.0), status("Booked") {
        for (Seat* seat : seats) {
            if (seat) {
                totalAmount += seat->getPrice();
            }
        }
    }

    int getTicketId() const { return ticketId; }
    User* getUser() const { return user; }
    Show* getShow() const { return show; }
    const vector<Seat*>& getBookedSeats() const { return bookedSeats; }
    double getTotalAmount() const { return totalAmount; }
    const string& getStatus() const { return status; }

    bool cancel() {
        if (status == "Cancelled") {
            return false;
        }
        if (!show->cancelSeats(bookedSeats)) {
            return false;
        }
        status = "Cancelled";
        return true;
    }
};

class User {
private:
    int userId;
    string name;
    string email;
    vector<Ticket*> tickets;
    PaymentMethod* paymentMethod;

public:
    User(int userId, const string& name, const string& email, PaymentMethod* paymentMethod)
        : userId(userId), name(name), email(email), paymentMethod(paymentMethod) {}

    int getUserId() const { return userId; }
    const string& getName() const { return name; }
    const string& getEmail() const { return email; }
    PaymentMethod* getPaymentMethod() const { return paymentMethod; }

    void addTicket(Ticket* ticket) {
        if (ticket && find(tickets.begin(), tickets.end(), ticket) == tickets.end()) {
            tickets.push_back(ticket);
        }
    }

    void removeTicket(Ticket* ticket) {
        tickets.erase(remove(tickets.begin(), tickets.end(), ticket), tickets.end());
    }

    const vector<Ticket*>& getTickets() const { return tickets; }
};

class Theater {
private:
    int theaterId;
    string name;
    string location;
    vector<Screen*> screens;

public:
    Theater(int theaterId, const string& name, const string& location)
        : theaterId(theaterId), name(name), location(location) {}

    int getTheaterId() const { return theaterId; }
    const string& getName() const { return name; }
    const string& getLocation() const { return location; }

    void addScreen(Screen* screen) {
        if (find(screens.begin(), screens.end(), screen) == screens.end()) {
            screens.push_back(screen);
        }
    }

    void removeScreen(Screen* screen) {
        screens.erase(remove(screens.begin(), screens.end(), screen), screens.end());
    }
};

class BookMyShowSystem {
private:
    vector<Movie*> movies;
    vector<Theater*> theaters;
    vector<Ticket*> tickets;
    int nextTicketId = 1;

public:
    void addMovie(Movie* movie) {
        if (movie && find(movies.begin(), movies.end(), movie) == movies.end()) {
            movies.push_back(movie);
        }
    }

    void addTheater(Theater* theater) {
        if (theater && find(theaters.begin(), theaters.end(), theater) == theaters.end()) {
            theaters.push_back(theater);
        }
    }

    void bookTicket(User* user, Show* show, const vector<Seat*>& seats) {
        if (!user || !show || seats.empty()) {
            cout << "Unable to book ticket: missing user, show, or seats.\n";
            return;
        }

        if (!show->lockSeats(seats)) {
            cout << "Unable to book ticket: seats could not be locked.\n";
            return;
        }

        double amount = 0.0;
        for (Seat* seat : seats) {
            if (!seat) {
                cout << "Unable to book ticket: invalid seat selection.\n";
                show->unlockSeats(seats);
                return;
            }
            amount += seat->getPrice();
        }

        if (!user->getPaymentMethod()->processPayment(amount)) {
            cout << "Payment failed for user " << user->getName() << "\n";
            show->unlockSeats(seats);
            return;
        }

        if (!show->bookLockedSeats(seats)) {
            cout << "Seats could not be booked for show " << show->getShowId() << " after payment.\n";
            show->unlockSeats(seats);
            return;
        }

        Ticket* ticket = new Ticket(nextTicketId++, user, show, seats);
        tickets.push_back(ticket);
        user->addTicket(ticket);

        cout << "Ticket booked successfully. Ticket ID: " << ticket->getTicketId() << " Total: $" << ticket->getTotalAmount() << "\n";
    }

    void cancelTicket(Ticket* ticket) {
        if (!ticket || ticket->getStatus() == "Cancelled") {
            cout << "Ticket cannot be cancelled.\n";
            return;
        }

        if (!ticket->cancel()) {
            cout << "Unable to cancel ticket " << ticket->getTicketId() << "\n";
            return;
        }

        ticket->getUser()->removeTicket(ticket);
        cout << "Ticket " << ticket->getTicketId() << " cancelled successfully.\n";
    }

    ~BookMyShowSystem() {
        for (Ticket* ticket : tickets) {
            delete ticket;
        }
    }
};

int main() {
    CreditCardPayment creditCard;
    User user1(1, "Alice", "alice@example.com", &creditCard);

    Movie movie1(1, "The Great Escape", "Action", 130);
    Theater theater1(1, "Cinema City", "Downtown");
    Screen screen1(1, "Screen 1");

    vector<Seat*> seats;
    seats.push_back(new NormalSeat(1, 1, 1, 10.0));
    seats.push_back(new PremiumSeat(2, 1, 2, 15.0));
    seats.push_back(new VIPSeat(3, 1, 3, 20.0));

    for (Seat* seat : seats) {
        screen1.addSeat(seat);
    }

    Show show1(1, &movie1, &screen1, "2026-05-05", "19:30");
    movie1.addShow(&show1);
    screen1.addShow(&show1);

    Theater theater2(2, "Prime Theater", "Uptown");
    theater2.addScreen(&screen1);

    BookMyShowSystem system;
    system.addMovie(&movie1);
    system.addTheater(&theater2);

    vector<Seat*> seatsToBook = { seats[0], seats[2] };
    system.bookTicket(&user1, &show1, seatsToBook);

    if (!user1.getTickets().empty()) {
        Ticket* ticket = user1.getTickets().front();
        system.cancelTicket(ticket);
    }

    for (Seat* seat : seats) {
        delete seat;
    }

    return 0;
}

