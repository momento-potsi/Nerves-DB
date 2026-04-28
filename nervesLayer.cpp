#include "nerves.h"
#include <string>

#include <tuple>
#include <vector>
#include <memory>
#include <map>

std::string LayerManager::getEntryString() /* TODO: Implement switch to match ordering options */
{
    std::string line;
    switch (tableName) {
        case GroupingType::ByLayer:
            line = "| LayerId: " + padToLength(std::to_string(mySQLObject.res->getInt("LayerId")), 5) + " ";
            line += "| LayerName: " + padToLength(mySQLObject.res->getString("LayerName"), 15) + " ";
            line += "| LayerFunction: " + padToLength(mySQLObject.res->getString("LayerFunction"), 15) + " ";
            line += "| NeuronCount: " + padToLength(std::to_string(mySQLObject.res->getInt("NeuronCount")), 10) + " |";
            break;
        case GroupingType::ByAction:
            line = "| ActionId: " + padToLength(std::to_string(mySQLObject.res->getInt("ActionId")), 8) + " ";
            line += "| ActionName: " + padToLength(mySQLObject.res->getString("ActionName"), 20) + " ";
            line += "| Threshold: " + padToLength(std::to_string(mySQLObject.res->getDouble("Threshold")), 10) + " ";
            line += "| UniquePath: " + padToLength(std::to_string(mySQLObject.res->getInt("UniquePath")), 10) + " ";
            line += "| AssignedWeight: " + padToLength(std::to_string(mySQLObject.res->getDouble("AssignedWeight")), 15) + " |";
            break;
        case GroupingType::ByReceptor:
            line = "| Active: " + padToLength(std::to_string(mySQLObject.res->getInt("Active")), 8) + " ";
            line += "| IO_Weight: " + padToLength(std::to_string(mySQLObject.res->getInt("IO_Weight")), 10) + " ";
            line += "| NeuronId: " + padToLength(std::to_string(mySQLObject.res->getInt("NeuronId")), 8) + " ";
            line += "| Sign: " + padToLength(std::to_string(mySQLObject.res->getInt("Sign")), 6) + " |";
            break;
        default:
            line = "| Unknown ordering |";
            break;
    }
    return line;
}


std::string LayerManager::getStatementOrdering(GroupingType type) {
    std::string stmt = "SELECT";
    switch (tableName) {
        case GroupingType::ByLayer:
            switch (type) {
                case GroupingType::ByConnection:
                    stmt += " Layer.LayerId, LayerName, LayerFunction, NeuronCount, ConnectionType, FromId, ToId";
                    stmt += " FROM Layer JOIN Connections ON Layer.LayerId = Connections.FromId ORDER BY Connections.ToId";
                    break;
                case GroupingType::ByAction: {
                    stmt += " Layer.LayerId, LayerName, LayerFunction, NeuronCount, ActionName, UniquePath";
                    stmt += " FROM Layer JOIN Action WHERE Layer.LayerId IN (";
                    sql::Statement* local_stmt = mySQLObject.con->createStatement();
                    sql::ResultSet* local_res = local_stmt->executeQuery("SELECT UniquePath FROM Action");
                    while (local_res->next()) {
                        std::string path = std::to_string(local_res->getInt("UniquePath"));
                        for (size_t i = 0; i < path.length(); i++) {
                            stmt += std::to_string(path[i] - '0');
                            if (path.length() != i + 1) {
                                stmt += ", ";
                            }
                        }
                    }
                    stmt += ")";
                    delete local_res;
                    delete local_stmt;
                } break;
                case GroupingType::ByReceptor:
                    stmt = "SELECT * FROM Layer JOIN Receptors ON Layer.LayerId = Receptors.NeuronId";  // Assuming NeuronId relates to LayerId or adjust
                    break;
                default:
                    stmt = "SELECT * FROM " + getTableName() + " ORDER BY LayerId";
                    break;
            }
            break;
        case GroupingType::ByAction:
            // Similar to NeuronManager, but for Action table
            switch (type) {
                case GroupingType::ByConnection:
                    stmt += " Action.ActionId, ActionName, Threshold, UniquePath, AssignedWeight, ConnectionType, FromId, ToId";
                    stmt += " FROM Action JOIN Connections ON Action.ActionId = Connections.FromId ORDER BY Connections.ToId";
                    break;
                case GroupingType::ByAction:
                    stmt = "SELECT * FROM " + getTableName() + " ORDER BY ActionId";
                    break;
                case GroupingType::ByReceptor:
                    stmt = "SELECT * FROM Action JOIN Receptors ON Action.ActionId = Receptors.NeuronId";
                    break;
                default:
                    stmt = "SELECT * FROM " + getTableName() + " ORDER BY ActionId";
                    break;
            }
            break;
        case GroupingType::ByReceptor:
            // Similar structure
            switch (type) {
                case GroupingType::ByConnection:
                    stmt += " Receptors.Active, IO_Weight, NeuronId, Sign, ConnectionType, FromId, ToId";
                    stmt += " FROM Receptors JOIN Connections ON Receptors.NeuronId = Connections.FromId ORDER BY Connections.ToId";
                    break;
                case GroupingType::ByAction: {
                    stmt += " Receptors.Active, IO_Weight, NeuronId, Sign, ActionName, UniquePath";
                    stmt += " FROM Receptors JOIN Action WHERE Receptors.NeuronId IN (";
                    sql::Statement* local_stmt2 = mySQLObject.con->createStatement();
                    sql::ResultSet* local_res2 = local_stmt2->executeQuery("SELECT UniquePath FROM Action");
                    while (local_res2->next()) {
                        std::string path = std::to_string(local_res2->getInt("UniquePath"));
                        for (size_t i = 0; i < path.length(); i++) {
                            stmt += std::to_string(path[i] - '0');
                            if (path.length() != i + 1) {
                                stmt += ", ";
                            }
                        }
                    }
                    stmt += ")";
                    delete local_res2;
                    delete local_stmt2;
                } break;
                case GroupingType::ByReceptor:
                    stmt = "SELECT * FROM " + getTableName();
                    break;
                default:
                    stmt = "SELECT * FROM " + getTableName() + " ORDER BY NeuronId";
                    break;
            }
            break;
        default:
            stmt = "SELECT * FROM " + getTableName() + " ORDER BY Id";
            break;
    }
    return stmt;
}
/* todo private layer functions */
void LayerManager::processLayer() {
    // Simulate activation starting from active receptors
    simulateActivation();
}

void LayerManager::runLayers() {
    // Run the network: view graph and simulate
    viewLayers(0);
    simulateActivation();
}

void LayerManager::viewLayers(int neuronId) { /* graph view */
    if (!mySQLObject.con) {
        std::cout << "No database connection.\n";
        return;
    }
    std::cout << "\n=== NEURAL NETWORK GRAPH ===\n\n";

    mySQLObject.stmt = mySQLObject.con->createStatement();
    mySQLObject.res = mySQLObject.stmt->executeQuery("SELECT LayerId, LayerName FROM Layer");

    while (mySQLObject.res->next()) {
        int layerId = mySQLObject.res->getInt("LayerId");
        std::string layerName = mySQLObject.res->getString("LayerName");

        std::cout << "[" << layerName << "]\n";

        // Query neurons in layer
        sql::PreparedStatement* prep = mySQLObject.con->prepareStatement("SELECT Id, NeuronName, NeuronWeight FROM Neurons WHERE LayerId = ?");
        prep->setInt(1, layerId);
        sql::ResultSet* res2 = prep->executeQuery();

        std::vector<int> neuronIds;
        while (res2->next()) {
            int id = res2->getInt("Id");
            std::string name = res2->getString("NeuronName");
            double weight = res2->getDouble("NeuronWeight");
            std::cout << "  (" << id << ") " << name << " [W=" << weight << "]\n";
            neuronIds.push_back(id);
        }

        std::cout << "\n";

        // Query connections from these neurons
        if (!neuronIds.empty()) {
            std::string inClause = "(";
            for (size_t i = 0; i < neuronIds.size(); i++) {
                inClause += std::to_string(neuronIds[i]);
                if (i < neuronIds.size() - 1) inClause += ",";
            }
            inClause += ")";

            sql::PreparedStatement* prep2 = mySQLObject.con->prepareStatement("SELECT FromId, ToId FROM Connections WHERE FromId IN " + inClause + " AND ConnectionType = 'NEURON'");
            sql::ResultSet* res3 = prep2->executeQuery();

            while (res3->next()) {
                int from = res3->getInt("FromId");
                int to = res3->getInt("ToId");
                std::cout << "    " << from << " ---> " << to << "\n";
            }

            delete res3;
            delete prep2;
        }

        std::cout << "\n---------------------------------\n";

        delete res2;
        delete prep;
    }
    delete mySQLObject.res;
    delete mySQLObject.stmt;
    mySQLObject.res = nullptr;
    mySQLObject.stmt = nullptr;
}

void LayerManager::setInputs() {
    if (!mySQLObject.con) {
        std::cout << "No database connection.\n";
        return;
    }
    // Set some receptors as active (e.g., first few)
    mySQLObject.stmt = mySQLObject.con->createStatement();
    mySQLObject.stmt->execute("UPDATE Receptors SET Active = 1 WHERE NeuronId <= 2");  // Example: activate first 2
    std::cout << "Inputs set.\n";
    delete mySQLObject.stmt;
    mySQLObject.stmt = nullptr;
}

void LayerManager::readOutputs(int layerId) { /* print, map to action, load input from outputs */
    if (!mySQLObject.con) {
        std::cout << "No database connection.\n";
        return;
    }
    std::cout << "\n=== OUTPUTS FOR LAYER " << layerId << " ===\n";

    sql::PreparedStatement* prep = mySQLObject.con->prepareStatement("SELECT Id, NeuronName, NeuronWeight, NeuronStatus FROM Neurons WHERE LayerId = ? AND NeuronRole = 'OUTPUT'");
    prep->setInt(1, layerId);
    sql::ResultSet* res = prep->executeQuery();

    while (res->next()) {
        int id = res->getInt("Id");
        std::string name = res->getString("NeuronName");
        double weight = res->getDouble("NeuronWeight");
        std::string status = res->getString("NeuronStatus");
        std::cout << "Output Neuron " << id << " (" << name << ") [W=" << weight << "] Status: " << status << "\n";
    }

    delete res;
    delete prep;
}

void LayerManager::simulateActivation() {
    if (!mySQLObject.con) {
        std::cout << "No database connection.\n";
        return;
    }
    std::cout << "\n=== ACTIVATION FLOW ===\n\n";

    std::map<int, double> signal;

    // Get active inputs from Receptors
    mySQLObject.stmt = mySQLObject.con->createStatement();
    mySQLObject.res = mySQLObject.stmt->executeQuery("SELECT NeuronId, IO_Weight FROM Receptors WHERE Active = 1");

    while (mySQLObject.res->next()) {
        int id = mySQLObject.res->getInt("NeuronId");
        double weight = mySQLObject.res->getDouble("IO_Weight");
        signal[id] = weight;
        std::cout << "Input Neuron " << id << " fires with signal " << signal[id] << "\n";
    }

    // Query connections
    delete mySQLObject.res;  // Delete previous res
    mySQLObject.res = mySQLObject.stmt->executeQuery("SELECT FromId, ToId FROM Connections WHERE ConnectionType = 'NEURON'");

    while (mySQLObject.res->next()) {
        int from = mySQLObject.res->getInt("FromId");
        int to = mySQLObject.res->getInt("ToId");

        if (signal.count(from)) {
            double propagated = signal[from] * 0.5;
            signal[to] += propagated;
            std::cout << from << " -> " << to << " | signal = " << propagated << "\n";
        }
    }

    std::cout << "\n=== OUTPUT SIGNALS ===\n";

    // Query output neurons
    sql::ResultSet* res2 = mySQLObject.stmt->executeQuery("SELECT Id FROM Neurons WHERE NeuronRole = 'OUTPUT'");

    while (res2->next()) {
        int id = res2->getInt("Id");
        if (signal.count(id)) {
            std::cout << "Output Neuron " << id << " final signal = " << signal[id] << "\n";
        }
    }

    delete res2;
    delete mySQLObject.res;
    delete mySQLObject.stmt;
    mySQLObject.res = nullptr;
    mySQLObject.stmt = nullptr;
}
