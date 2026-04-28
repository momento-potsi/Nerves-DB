#include "nerves.h"
#include <string>

using namespace std;

string NeuronManager::getEntryString() /* TODO: Implement switch to match ordering options */
{
    string line = "| ID: " + padToLength(to_string(mySQLObject.res->getInt("Id")), 4) + " ";
        line += padToLength("| Name: ", 10) + padToLength(mySQLObject.res->getString("NeuronName"), 10) + " ";
        line += padToLength("| Weight: ", 10) + padToLength(to_string(mySQLObject.res->getDouble("NeuronWeight")), 10) + " ";
        line += padToLength("| Status: ", 10) + padToLength(mySQLObject.res->getString("NeuronStatus"), 10) + " ";
        line += padToLength("| Role: ", 10) + padToLength(mySQLObject.res->getString("NeuronRole"), 10) + " ";
        line += padToLength("| Layer: ", 10) + padToLength(to_string(mySQLObject.res->getInt("LayerId")), 10) + " |";
    return line;
}



void NeuronManager::insert()
{
    IManager::insert(); 
    /* check if prev */
    /* find all neurons in previous layer and create connections */
    /* check if next */
    /* find all neurons in next layer and create connections */
}


string NeuronManager::getStatementOrdering(GroupingType type)
{
    string stmt = "SELECT";
    switch (type) 
    {
        case GroupingType::ByConnection:
            stmt += " Neurons.Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId, ConnectionType, FromId, ToId";
            stmt += " FROM Neurons JOIN Connections ON Neurons.Id = Connections.FromId ORDER BY Connections.ToId";
        break;
        case GroupingType::ByAction: /* Group by action's unique path in neurons */
            stmt += " Neurons.Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, ActionName, UniquePath";
            stmt += " FROM Neurons JOIN Action WHERE Neurons.Id IN ("; /* "#, "*/
            mySQLObject.res = mySQLObject.stmt->executeQuery("SELECT UniquePath FROM Action"); /* get unique path */
            while (mySQLObject.res->next()) 
            {
                string path = to_string(mySQLObject.res->getInt("UniquePath"));
                for (size_t i = 0; i < path.length(); i++) { /* Each integer (id) in the path */
                    stmt += to_string(path[i]); 
                    if(path.length() != i + 1) { /* omit last comma */
                        stmt += ", ";
                    }
                }
            }
            stmt += ")";
        break;
        case GroupingType::ByReceptor: 
            stmt = "SELECT * FROM Neurons JOIN Receptors ON NeuronId = Id";
        break;
        default: /* Fall Through Case - general print */
            stmt = "SELECT * FROM " + getTableName() + " ORDER BY LayerId";
        break;
    }

    return stmt;
}

/* TODO: do functions for tables not covered by default tables () */