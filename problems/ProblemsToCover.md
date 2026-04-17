In LLD interviews, companies usually don’t care whether you memorized patterns—they care whether you can model entities, relationships, extensibility, concurrency, and clean APIs.

Top LLD Use Cases Asked in Interviews (Most Common)
Tier 1 — Very Frequently Asked
1. Parking Lot System

Classic object modeling problem.

Concepts tested:

Classes, enums
Strategy pattern (slot allocation)
Inheritance (vehicle types)
Payment system
State management

Entities: ParkingLot, Floor, Slot, Vehicle, Ticket

2. Elevator System

Excellent for state machines + scheduling.

Concepts tested:

State pattern
Scheduling algorithms
Multi-threading discussion
Request queues

Entities: Elevator, Button, Request, Controller

3. Cab Booking / Ride Sharing

Popular in product companies.

Concepts tested:

Matching logic
Observer/events
Strategy pricing
Location models

Entities: Rider, Driver, Trip, Payment

4. BookMyShow / Movie Ticket Booking

Tests concurrency heavily.

Concepts tested:

Seat locking
Race conditions
Transactions
Search/filtering

Entities: User, Show, Seat, Booking, Payment

5. ATM Machine

Simple but rich in state transitions.

Concepts tested:

State pattern
Cash dispenser chain
Validation flow

Entities: ATM, Card, BankAccount, Transaction

Tier 2 — Common Mid/Advanced
6. Library Management System

Good for CRUD modeling.

Entities: Book, Member, Loan, Reservation

7. Vending Machine

Small version of ATM.

Concepts tested: State + inventory + payment.

8. Notification System

Very common in backend roles.

Concepts tested:

Observer
Strategy
Retry mechanism

Entities: Notification, EmailSender, SMS, Push

9. Food Delivery (Swiggy/Zomato)

Broader version of cab booking.

Entities: Customer, Restaurant, Order, DeliveryPartner

10. Hotel Booking / Flight Booking

Reservation + inventory + payment.

Tier 3 — Strong Interview Differentiators
11. Rate Limiter

Excellent for backend interviews.

Concepts tested:

Sliding window
Token bucket
Thread safety

12. Cache (LRU/LFU)

Frequently asked.

Concepts tested:

HashMap + DLL
O(1) design

13. Chess / Snake & Ladder / Tic Tac Toe

Tests game modeling.

14. Splitwise

Tests debt graph / settlement.

15. File System

Tree structures + permissions.

Top 10 Must Prepare (Best ROI)
Parking Lot
Elevator
BookMyShow
Cab Booking
ATM
Notification System
Vending Machine
Library System
Cache (LRU)
Rate Limiter


What Interviewer Actually Checks
Core Skills:
Correct class design
OOP principles
Extensibility
Clean interfaces
Edge cases
Thread safety
DB/API thinking
Tradeoffs
Best Design Patterns to Use in LLD Interviews
Strategy → pricing / allocation
Observer → notifications/events
State → ATM/elevator/order states
Factory → object creation
Singleton → managers/controllers
Decorator → add logging/retry
Chain of Responsibility → validations
My Recommended Preparation Order (For You)


Since you're aiming for strong interviews:

Week 1
Parking Lot, ATM, Vending Machine

Week 2
Elevator, Cab Booking, BookMyShow

Week 3
Notification, Cache, Rate Limiter

Week 4
Splitwise, File System, Food Delivery


If you have only 3 days before interview:

Do these only:

Parking Lot
Elevator
BookMyShow
ATM
Rate Limiter



| #  | Problem                     | Common Companies    | What They Test                | Useful Patterns                    |
| -- | --------------------------- | ------------------- | ----------------------------- | ---------------------------------- |
| 1  | Parking Lot                 | Microsoft, Amazon   | OOP basics, extensibility     | Strategy, Factory, Singleton       |
| 2  | Elevator System             | Uber, Amazon        | Scheduling, state transitions | State, Strategy                    |
| 3  | BookMyShow / Ticket Booking | Microsoft, Flipkart | Concurrency, locking          | State, Observer                    |
| 4  | Cab Booking / Uber          | Uber, Microsoft     | Matching, pricing, events     | Strategy, Observer, Factory        |
| 5  | ATM Machine                 | Amazon              | Workflow, state machine       | State, Chain of Responsibility     |
| 6  | Vending Machine             | Microsoft           | Inventory + states            | State, Factory                     |
| 7  | Notification Service        | Amazon              | Multi-channel architecture    | Observer, Strategy, Factory        |
| 8  | Splitwise                   | Flipkart, Amazon    | Relationships, settlements    | Strategy                           |
| 9  | Food Delivery               | Swiggy/Uber style   | Order lifecycle               | State, Observer                    |
| 10 | Library Management          | Microsoft           | CRUD design                   | Factory                            |
| 11 | Cache (LRU/LFU)             | Amazon, Uber        | Data structures + API         | Singleton                          |
| 12 | Rate Limiter                | Uber, Amazon        | Scalability, thread safety    | Strategy                           |
| 13 | Chess / Tic Tac Toe         | Microsoft           | Modeling, rules engine        | State                              |
| 14 | File System                 | Amazon              | Tree structure                | Composite                          |
| 15 | Logger Framework            | Microsoft           | Extensible pipelines          | Chain of Responsibility, Singleton |
