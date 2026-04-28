#include "nerves.h"
#include <string>

using namespace std;

void IManager::showAllBy(GroupingType type) 
{
    mySQLObject.stmt = mySQLObject.con->createStatement();
    if(tableName != type) { /* Prepare statement ordering based on table */
        currentQuery = getStatementOrdering(type); /* Get class specific implementation */
    } else { /* If same table */
        currentQuery = "SELECT * FROM " + getTableName(); /* General print */
        if(tableName == GroupingType::ByNeuron || tableName == GroupingType::ByLayer) { /* Order by LayerId if present */
            currentQuery += " ORDER BY LayerId";
        }
    }
    
    mySQLObject.res = mySQLObject.stmt->executeQuery(currentQuery);
    
    cout << "\n|> [Displaying Table: `" << getTableName() << "`]\n\n";
    string border = "";
    while (mySQLObject.res->next()) {
        string line = getEntryString(); /* get Class specific implementation */       
        if (border.length() == 0) {
            border = line.length() > 0 ? string(line.length() - 2, '-') : "";
            border = " " + border + " ";
            cout << border << endl;
        }
        cout << line << endl;    
    }
    cout << border << endl;
}


void IManager::findById()
{
    int id; string tableIdName;
    std::cout << "Enter " << getTableName() << " ID: "; std::cin >> id;

    mySQLObject.stmt = mySQLObject.con->createStatement();    
    mySQLObject.res = mySQLObject.stmt->executeQuery("SELECT * FROM " + getTableName() + " WHERE " + getIdName() + " = " + to_string(id));
    
    cout << "\n|> [Displaying Entry from: `" << getTableName() << "` Table]\n\n";
    string border = "";
    while (mySQLObject.res->next()) {
        string line = getEntryString(); /* get Class specific implementation */       
        if (border.length() == 0) {
            border = line.length() > 0 ? string(line.length() - 2, '-') : "";
            border = " " + border + " ";
            cout << border << endl;
        }
        cout << line << endl;    
    }
    cout << border << endl;   
}

int IManager::getNextId()
{
    mySQLObject.stmt = mySQLObject.con->createStatement();
    mySQLObject.res = mySQLObject.stmt->executeQuery("SELECT MAX(" + getIdName() + ") AS MaxId FROM " + getTableName());
    if (mySQLObject.res->next()) {
        return mySQLObject.res->getInt("MaxId") + 1;
    }

    return 0;
}

void IManager::printCount()
{
    mySQLObject.stmt = mySQLObject.con->createStatement();
    mySQLObject.res = mySQLObject.stmt->executeQuery("SELECT COUNT(*) AS Count FROM " + getTableName());
    if (mySQLObject.res->next()) {
        cout << "[Total entries in " << getTableName() << ": " << mySQLObject.res->getInt("Count") << "]\n" << endl;
    }
}

void IManager::insert()
{
    /* Get the next available ID */
    int nextId = getNextId();
    
    /* Input varisbles */
    int i1, i2, i3, i4; string s1, s2, s3; double d1, d2;
    switch (tableName)
    {
        case GroupingType::ByNeuron: /* impl should insert connections, update layer neuron count */
            cout << "Enter Neuron Name: "; cin >> s1;
            cout << "Enter Neuron Weight: "; cin >> d1;
            cout << "Enter Neuron Status: "; cin >> s2;
            cout << "Enter Neuron Role: "; cin >> s3;
            cout << "Enter Layer ID: "; cin >> i1;
            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "INSERT INTO Neurons (Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId) "
                "VALUES (" + to_string(nextId) + ", " + s1 + ", " + to_string(d1) + ", " + s2 + ", " + s3 + ", " + to_string(i1) + ")"
            );

        break;
        case GroupingType::ByLayer: /* impl should insert connections */
            cout << "Enter Layer Name: "; cin >> s1;
            cout << "Enter Layer Function: "; cin >> s2;
            cout << "Enter Layer ID (if connection is between layers): "; cin >> i3;
            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "INSERT INTO Layer (LayerId, LayerName, LayerFunction, NeuronCount) "
                "VALUES (" + to_string(i3) + ", " + s1 + ", " + s2 + ", 0)"
            );
        break;
        case GroupingType::ByConnection: 
            cout << "Enter Connection Type (Layer/Neuron): "; cin >> s1;
            cout << "Enter From Neuron ID: "; cin >> i1;
            cout << "Enter To Neuron ID: "; cin >> i2;
            
            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "INSERT INTO Connections (ConnectionId, ConnectionType, FromId, ToId) "
                "VALUES (" + to_string(nextId) + ", " + s1 + ", " + to_string(i1) + ", " + to_string(i2) + ")"
            );
        break;
        case GroupingType::ByAction:
            cout << "Enter Action Name: "; cin >> s1;
            cout << "Enter Assigned Weight: "; cin >> d1;
            cout << "Enter Threshold: "; cin >> d2;
            /* cout << "Enter Unique Path: "; cin >> i1; */ i1 = 0; 
            /* TODO: Unique path should be generated based on neuron path in network, impl later */

            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "INSERT INTO Action (ActionId, ActionName, AssignedWeight, Threshold, UniquePath) "
                "VALUES (" + to_string(nextId) + ", " + s1 + ", " + to_string(d1) + ", " + to_string(d2) + ", " + to_string(i1) + ")"
            );
        break;
        case GroupingType::ByReceptor:
            cout << "Enter Neuron ID: "; cin >> i1;
            cout << "Enter Receptor Sign (+/-): "; cin >> s1;
            if(s1 == "+" || s1 == "POSITIVE") { i2 = 1; } else if (s1 == "-" || s1 == "NEGATIVE") {
                i2 = -1;
            } else {
                cout << "Invalid sign input. Defaulting to '+'.\n"; i2 = 1;
            }
            cout << "Enter Receptor IO Weight: "; cin >> i3;
            cout << "Is Active? (1 for true, 0 for false): "; cin >> i4;

            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "INSERT INTO Receptors (NeuronId, Sign, IO_Weight, Active) "
                "VALUES (" + to_string(i1) + ", " + to_string(i2) + ", " + to_string(i3) + ", " + to_string(i4) + ")"
            );
        break;
        default: break;
    }

    mySQLObject.prep_stmt->execute();

    cout << "[Insertion completed].\n";
}

void IManager::deleteById(int id) 
{   
    switch(tableName) /* delete associated data first */
    {
        case GroupingType::ByReceptor: /* same process for receptors */
        case GroupingType::ByNeuron: /* delete connections related to neuron */
            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "DELETE FROM Connections WHERE FromId = " + to_string(id) + " OR ToId = " + to_string(id)
            );
            mySQLObject.prep_stmt->execute();
            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "DELETE FROM Receptors WHERE NeuronId = " + to_string(id)
            );
            mySQLObject.prep_stmt->execute();
            /* decrement neuron count */
            mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                "SELECT LayerId FROM Neurons WHERE Id = " + to_string(id)
            );
            mySQLObject.res = mySQLObject.prep_stmt->executeQuery();
            if (mySQLObject.res->next()) {
                int layerId = mySQLObject.res->getInt("LayerId");
                mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
                    "UPDATE Layer SET NeuronCount = NeuronCount - 1 WHERE LayerId = " + to_string(layerId)
                );
                mySQLObject.prep_stmt->execute();
            }
            
        break;
        case GroupingType::ByLayer: /* delete neurons and connections related to layer */
            /* switch to neuron table to find neurons, cascade delete connections related to each neuron, then delete layer */
            tableName = GroupingType::ByNeuron; 
            deleteById(id);
            tableName = GroupingType::ByLayer;
        break;
        default: break;
    }

    mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
        "DELETE FROM " + getTableName() + " WHERE " + getIdName() + " = " + to_string(id)
    );
    mySQLObject.prep_stmt->execute();

    cout << "[Deletion completed].\n";
}

void IManager::findWhere()
{
    string condition;
    cout << "Enter condition for search (e.g. 'WHERE Id = 1'): "; 
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    std::getline(cin, condition);

    mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
        "SELECT * FROM " + getTableName() + " " + condition
    );

    mySQLObject.res = mySQLObject.prep_stmt->executeQuery();

    cout << "\n|> [Displaying Entry from: `" << getTableName() << "` Table]\n\n";
    string border = "";
    while (mySQLObject.res->next()) {
        string line = getEntryString(); /* get Class specific implementation */       
        if (border.length() == 0) {
            border = line.length() > 0 ? string(line.length() - 2, '-') : "";
            border = " " + border + " ";
            cout << border << endl;
        }
        cout << line << endl;    
    }
    cout << border << endl;   
}

void IManager::adjustById()
{
    int id; string column, value;
    cout << "Enter ID of entry to adjust: "; cin >> id;
    cout << "Enter column to adjust: "; cin >> column;
    cout << "Enter new value: "; cin >> value;

    mySQLObject.prep_stmt = mySQLObject.con->prepareStatement(
        "UPDATE " + getTableName() + " SET " + column + " = " + value + " WHERE " + getIdName() + " = " + to_string(id)
    );

    mySQLObject.prep_stmt->execute();

    cout << "[Adjustment completed].\n";
}