
#include "nerves.h"

using namespace std;


/* Author: Oluwatosin Daudu, Massai Morgan */

/* g++ -I/usr/include nerves.cpp -o nervesapp -I /usr/local/lib -lmysqlcppconn */


/** Todo: polish functions and gear features to other tables */


int main()
{
    IManager* manager = new NeuronManager();
    try {
        manager->mySQLObject.driver = get_driver_instance();
        manager->mySQLObject.con = manager->mySQLObject.driver->connect("tcp://127.0.0.1:3306", "root", "");
        manager->mySQLObject.con->setSchema("nerves");

        int option = 0;
        /* Todo: General table options -> show all, add, find by, adjust, insert, delete; */
        /* Set input, map out, run network*/
        while (option != 5)
        {
            cout << "\n1. Add a neuron\n";
            cout << "2. Find neuron by ID\n";
            cout << "3. Find neurons by Layer\n";
            cout << "4. Show all neurons\n";
            cout << "5. Exit\n\n";

            cout << "Choice: "; cin >> option;

            switch(option) {
                case 1: manager->insert(); break;
                case 2: manager->findById(); break;
                case 3: manager->findBy(); break;
                case 4: manager->showAll(); break;
            }
        }

        delete manager;
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what() << endl;
        
        if(manager != nullptr) {
            delete manager;
        }
    }

    return EXIT_SUCCESS;
}




