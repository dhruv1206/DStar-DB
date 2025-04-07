// // src/main.cpp
// #include <iostream>
// #include "../include/Database/Database.h"
// #include <thread>
// #include <mutex>
// #include <vector>

// using namespace std;

// struct Test
// {
//     int abc;
//     string bcs;
//     bool check;
// };
// ostream &operator<<(ostream &os, const Test &t)
// {
//     os << "Test(" + t.abc << ", " + t.bcs + ", " + (t.check ? "True" : "False") + ");";
//     return os;
// }

// int main()
// {
//     Database db(100); // 100 MB cache size
//     try
//     {
//         // const string recordId = "user1";
//         // // Insert a new record with id "user1"
//         // db.insertRecord(recordId);
//         // auto record = db.getRecord(recordId);

//         // Test t;
//         // t.abc = 1;
//         // t.bcs = "asdf";
//         // t.check = true;
//         // // Dynamically add fields to the record
//         // record->setField("Name", t);
//         // record->setField("Age", 30);
//         // record->setField("Email", string("alice@example.com"));

//         // // Retrieve and print some fields
//         // cout << "Name: " << record->getField<Test>("Name") << endl;
//         // cout << "Age: " << record->getField<int>("Age") << endl;
//         // cout << "Email: " << record->getField<string>("Email") << endl;

//         // // Delete a field and try to access it (will throw an error)
//         // record->removeField("Email");
//         // // Uncomment the following line to test error handling:
//         // // cout << "Email: " << record->getField<string>("Email") << endl;

//         // // Delete the record from the database
//         // db.deleteRecord(recordId);
//         // int a;
//         // cin >> a;
//         // Input some data to test the database
//         const int TOTAL_COUNT = 10000000;
//         const unsigned int NUM_THREADS = thread::hardware_concurrency();
//         const int chunk = TOTAL_COUNT / NUM_THREADS;
//         vector<thread> threads;

//         // Optional: protect console output if needed.
//         mutex coutMutex;

//         // Lambda to process a chunk of iterations.
//         auto worker = [&](int start, int end)
//         {
//             // Each thread can use its own random seed.
//             srand((unsigned)time(nullptr) ^ (start));
//             for (int i = start; i < end; i++)
//             {
//                 try
//                 {

//                     lock_guard<mutex> lock(coutMutex);
//                     cout << "Processing: " << i << " / " << end << endl;

//                     string id = to_string(rand()) + "_" + to_string(i);
//                     string recordId = "user" + id;
//                     db.insertRecord(recordId);
//                     auto record = db.getRecord(recordId);
//                     Test t;
//                     t.abc = 1;
//                     t.bcs = "asdf";
//                     t.check = true;
//                     // Dynamically add fields to the record
//                     record->setField("Name", t);
//                     record->setField("Age", 30);
//                 }
//                 catch (const std::exception &e)
//                 {
//                     lock_guard<mutex> lock(coutMutex);
//                     cerr << "Error: " << e.what() << '\n';
//                     break;
//                 }
//             }
//         };

//         // Create threads and assign work.
//         for (unsigned int i = 0; i < NUM_THREADS; i++)
//         {
//             try
//             {
//                 int start = i * chunk;
//                 int end = (i == NUM_THREADS - 1) ? TOTAL_COUNT : start + chunk;
//                 threads.emplace_back(worker, start, end);
//             }
//             catch (const std::exception &e)
//             {
//                 std::cerr << e.what() << '\n';
//             }
//         }

//         // Join threads.
//         for (auto &t : threads)
//         {

//             t.join();
//         }

//         // Dummy data entry end
//         while (true)
//         {
//             cout << "Current Cache Usage: " << db.getCacheSize() << " MB" << " / " << 100 << "MB" << endl;
//             cout << "Enter 1 to select a Record" << endl;
//             cout << "Enter 2 to create a Record" << endl;
//             cout << "Enter 3 to exit." << endl;
//             int input;
//             cin >> input;
//             if (input != 1 && input != 2 && input != 3)
//             {
//                 cout << "Invalid Input" << endl;
//                 continue;
//             }
//             if (input == 3)
//             {
//                 cout << "Exiting..." << endl;
//                 break;
//             }
//             if (input == 1)
//             {
//                 cout << "Enter Record ID: " << endl;
//                 string recordId;
//                 cin >> recordId;
//                 auto record = db.getRecord(recordId);
//                 cout << "Record ID: " << record->getId() << " selected!" << endl;
//                 while (true)
//                 {
//                     cout << "Current Cache Usage: " << db.getCacheSize() << " MB" << " / " << 100 << "MB" << endl;
//                     cout << "Enter 1 to get a field" << endl;
//                     cout << "Enter 2 to set a field" << endl;
//                     cout << "Enter 3 to remove a field" << endl;
//                     cout << "Enter 4 to exit." << endl;
//                     int input2;
//                     cin >> input2;
//                     if (input2 != 1 && input2 != 2 && input2 != 3 && input2 != 4)
//                     {
//                         cout << "Invalid Input" << endl;
//                         continue;
//                     }
//                     if (input2 == 4)
//                     {
//                         cout << "Exiting..." << endl;
//                         break;
//                     }
//                     if (input2 == 1)
//                     {
//                         cout << "Enter Field Name: " << endl;
//                         string fieldName;
//                         cin >> fieldName;
//                         try
//                         {
//                             auto value = record->getField<Test>(fieldName);
//                             cout << "Field Value: " << value << endl;
//                         }
//                         catch (const exception &ex)
//                         {
//                             cerr << "Error: " << ex.what() << endl;
//                         }
//                     }
//                     else if (input2 == 2)
//                     {
//                         cout << "Enter Field Name: " << endl;
//                         string fieldName;
//                         cin >> fieldName;
//                         Test t = {1, "test", true};
//                         record->setField(fieldName, t);
//                         cout << "Field Set!" << endl;
//                     }
//                     else if (input2 == 3)
//                     {
//                         cout << "Enter Field Name: " << endl;
//                         string fieldName;
//                         cin >> fieldName;
//                         try
//                         {
//                             record->removeField(fieldName);
//                             cout << "Field Removed!" << endl;
//                         }
//                         catch (const exception &ex)
//                         {
//                             cerr << "Error: " << ex.what() << endl;
//                         }
//                     }
//                 }
//             }
//             else if (input == 2)
//             {
//                 cout << "Enter Record ID: " << endl;
//                 string recordId;
//                 cin >> recordId;
//                 db.insertRecord(recordId);
//                 auto record = db.getRecord(recordId);
//                 cout << "Record ID: " << record->getId() << " created!" << endl;
//             }
//         }
//     }
//     catch (const exception &ex)
//     {
//         cerr << "Error: " << ex.what() << endl;
//     }

//     return 0;
// }
