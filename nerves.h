#pragma once

/* Author: Oluwatosin Daudu */
/* File Description: Declaration of Managing Classes */


/* SQL Includes */
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;



struct SQLHelper { /* struct to group SQL objects, use through one of Managers */
    sql::Driver             *driver;
    sql::Connection         *con;
    sql::Statement          *stmt;
    sql::ResultSet          *res;
    sql::PreparedStatement  *prep_stmt;
};

enum class GroupingType { /* Enum to specify current table or Grouping order */
    ByNeuron,
    ByLayer,
    ByConnection,
    ByAction,
    ByReceptor
};

string padToLength(const string& str, size_t minLength);

/* Abstract base class for database management */
class IManager
{
protected:
    GroupingType tableName;
    std::string  currentQuery;

    /* @function: Formatted string of current entry in result set */
    virtual std::string getEntryString() = 0; 
    /* @function: Return prepared statement for displaying table based on class impl */
    virtual std::string getStatementOrdering(GroupingType type) = 0;
 
public:
    SQLHelper    mySQLObject;
    
    /* @param type: Show Entries in table using order/grouping/join */
    virtual void showAllBy(GroupingType type); 
    /* @function: fixed SELECT-All output */
    virtual void showAll() { showAllBy(tableName); }
    /* @param id: Get entry in table by Primary Key Id */
    virtual void findById(); 
    /* @param condition: String should be parameter of SELECT-WHERE clause */
    virtual void findWhere();
    /* @function: Insert a new entry into the table */
    virtual void insert(); 
    /* @function: Delete an entry from the table by Primary Key Id */
    virtual void deleteById(int id); 
    /* @function: Update an entry in the table by Primary Key Id */
    virtual void adjustById(); 
    virtual void printCount(); /* Print count of entries in current table */

    int    getNextId();
    string getIdName() {
        switch (tableName) {
            case GroupingType::ByNeuron:     return "Id";
            case GroupingType::ByLayer:      return "LayerId";
            case GroupingType::ByConnection: return "ConnectionId";
            case GroupingType::ByAction:     return "ActionId";
            case GroupingType::ByReceptor:   return "NeuronId";
            default:                         return "Id";
        }
    }

    std::string getTableName() 
    {
        switch (tableName) {
            case GroupingType::ByNeuron:     return "Neurons";
            case GroupingType::ByLayer:      return "Layer";
            case GroupingType::ByConnection: return "Connections";
            case GroupingType::ByAction:     return "Action";
            case GroupingType::ByReceptor:   return "Receptors";
            default:                         return "NULL";
        }
    }

    void changeTable(GroupingType newTable) {
        tableName = newTable;
        cout << "\n|> [Changed Working Table: `" << getTableName() << "`]\n";
    }

    IManager() { cout << "\n|> [Accessed Working Table: `" << getTableName() << "`]\n"; }

    virtual ~IManager() {
        /*if(mySQLObject.prep_stmt != nullptr) delete mySQLObject.prep_stmt;
        if(mySQLObject.res != nullptr)       delete mySQLObject.res;
        if(mySQLObject.stmt != nullptr)      delete mySQLObject.stmt;
        if(mySQLObject.con != nullptr)       delete mySQLObject.con;
        */
    }

};

class NeuronManager: public IManager /* conns, neurons */
{
public:
    string getEntryString() override;
    void insert() override; /* custom for neuron (each new insert must start full connections between layers) */
    string getStatementOrdering(GroupingType type) override;
    /* call base insert then populate connections */
    void connect   (int srcId, int destId);
    void disconnect(int srcId, int destId); 
    void viewParentConnections(int neuronId);

    NeuronManager() {
        tableName = GroupingType::ByNeuron;
        cout << "\n|> [Accessed Working Table: `" << getTableName() << "`]\n";
    }
};


class LayerManager: public IManager /* layers (IO, actions), processing/functions */
{
private:
    void simulateActivation();

public:
    string getEntryString() override;
    string getStatementOrdering(GroupingType type) override;
    /* todo private layer functions */
    void processLayer();
    void runLayers(); 
    void viewLayers(int neuronId); /* graph view */

    void setInputs();
    void readOutputs(int layerId); /* print, map to action, load input from outputs */

    LayerManager() {
        tableName = GroupingType::ByLayer;
        std::cout << "\n|> [Accessed Working Table: `" << getTableName() << "`]\n";
    }
};