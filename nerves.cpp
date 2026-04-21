#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;


/* Author: Oluwatosin Daudu, Massai Morgan */

/* g++ -I/usr/include nerves.cpp -o nervesapp -I /usr/local/lib -lmysqlcppconn */

/** Todo: polish functions and gear features to other tables */

sql::Driver             *driver;
sql::Connection         *con;
sql::Statement          *stmt;
sql::ResultSet          *res;
sql::PreparedStatement  *prep_stmt;

void findall();
void findbyid();
void insert();
void findbylayer();

int main()
{
    try {
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "");
        con->setSchema("nerves");

        int option = 0;

        while (option != 5) {
            cout << "\n1. Add a neuron\n";
            cout << "2. Find neuron by ID\n";
            cout << "3. Find neurons by Layer\n";
            cout << "4. Show all neurons\n";
            cout << "5. Exit\n\n";

            cout << "Choice: ";
            cin >> option;

            switch(option) {
                case 1: insert(); break;
                case 2: findbyid(); break;
                case 3: findbylayer(); break;
                case 4: findall(); break;
            }
        }

        delete con;

    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what() << endl;
    }

    return EXIT_SUCCESS;
}

void findall() {
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM Neurons");

    while (res->next()) {
        cout << "ID: " << res->getInt("Id") << " ";
        cout << "Name: " << res->getString("NeuronName") << " ";
        cout << "Weight: " << res->getDouble("NeuronWeight") << " ";
        cout << "Status: " << res->getString("NeuronStatus") << " ";
        cout << "Role: " << res->getString("NeuronRole") << " ";
        cout << "Layer: " << res->getInt("LayerId") << endl;
    }
}

void findbyid() {
    int id;
    cout << "Enter neuron ID: ";
    cin >> id;

    prep_stmt = con->prepareStatement(
        "SELECT * FROM Neurons WHERE Id = ?"
    );

    prep_stmt->setInt(1, id);
    res = prep_stmt->executeQuery();

    while (res->next()) {
        cout << res->getInt("Id") << " ";
        cout << res->getString("NeuronName") << " ";
        cout << res->getDouble("NeuronWeight") << " ";
        cout << res->getString("NeuronStatus") << " ";
        cout << res->getString("NeuronRole") << " ";
        cout << res->getInt("LayerId") << endl;
    }
}

void insert() 
{
    string name, status, role; double weight; int layer;

    cout << "Neuron Name: "; cin >> name;
    cout << "Weight: ";      cin >> weight;
    cout << "Status: ";      cin >> status;
    cout << "Role: ";        cin >> role;
    cout << "Layer ID: ";    cin >> layer;

    prep_stmt = con->prepareStatement(
        "INSERT INTO Neurons (NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId) "
        "VALUES (?, ?, ?, ?, ?)"
    );

    prep_stmt->setString(1, name);
    prep_stmt->setDouble(2, weight);
    prep_stmt->setString(3, status);
    prep_stmt->setString(4, role);
    prep_stmt->setInt(5, layer);

    prep_stmt->execute();

    cout << "Neuron inserted.\n";
}

void findbylayer() {
    int layer;

    cout << "Enter Layer ID: ";
    cin >> layer;

    prep_stmt = con->prepareStatement(
        "SELECT N.Id, N.NeuronName, L.LayerName "
        "FROM Neurons N JOIN Layer L ON N.LayerId = L.LayerId "
        "WHERE N.LayerId = ?"
    );

    prep_stmt->setInt(1, layer);
    res = prep_stmt->executeQuery();

    while (res->next()) {
        cout << res->getInt("Id") << " ";
        cout << res->getString("NeuronName") << " ";
        cout << res->getString("LayerName") << endl;
    }
}


