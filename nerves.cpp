
#include "nerves.h"

using namespace std;


/* Author: Oluwatosin Daudu, Massai Morgan */

/* g++ -I/usr/include nervesInterface.cpp nervesLayer.cpp nervesNeuron.cpp nerves.cpp -o nervesapp -I /usr/local/lib -lmysqlcppconn */


/** Todo: polish functions and gear features to other tables */

string padToLength(const string& str, size_t minLength) {
    if (str.length() >= minLength) return str;
    return str + string(minLength - str.length(), ' ');
}

static void connectManager(IManager& manager)
{
    manager.mySQLObject.driver = get_driver_instance();
    manager.mySQLObject.con = manager.mySQLObject.driver->connect("tcp://127.0.0.1:3306", "root", "");
    manager.mySQLObject.con->setSchema("nerves");
}

int main()
{
    NeuronManager neuronManager;
    LayerManager layerManager;

    try {
        connectManager(neuronManager); connectManager(layerManager);

        int mainOption = 0;
        while (mainOption != 3) {
            cout << "\n=== NERVES DB MENU ===\n";
            cout << "1. Neuron manager\n";
            cout << "2. Layer manager\n";
            cout << "3. Exit\n";
            cout << "Choice: "; cin >> mainOption;

            if (mainOption == 1) {
                int neuronOption = 0;
                while (neuronOption != 6) {
                    cout << "\n--- Neuron Manager ---\n";
                    cout << "1. Add a neuron\n";
                    cout << "2. Find neuron by ID\n";
                    cout << "3. Find neurons by Layer\n";
                    cout << "4. Show all neurons\n";
                    cout << "5. Manage neuron connections\n";
                    cout << "6. Back\n";
                    cout << "Choice: "; cin >> neuronOption;

                    switch (neuronOption) {
                        case 1: neuronManager.insert(); break;
                        case 2: neuronManager.findById(); break;
                        case 3: neuronManager.findWhere(); break;
                        case 4: neuronManager.showAll(); break;
                        case 5: { 
                            
                            cout << "\n1. Connect neurons\n";
                            cout << "2. Disconnect neurons\n";
                            cout << "3. View parent connections\n";
                            cout << "Choice: "; int connOption = 0; cin >> connOption;
                            if (connOption == 1) {
                                int src, dst;
                                cout << "Source neuron ID: "; cin >> src;
                                cout << "Destination neuron ID: "; cin >> dst;
                                neuronManager.connect(src, dst);
                            } else if (connOption == 2) {
                                int src, dst;
                                cout << "Source neuron ID: "; cin >> src;
                                cout << "Destination neuron ID: "; cin >> dst;
                                neuronManager.disconnect(src, dst);
                            } else if (connOption == 3) {
                                int nid;
                                cout << "Neuron ID: "; cin >> nid;
                                neuronManager.viewParentConnections(nid);
                            } else {
                                cout << "Invalid connection option.\n";
                            }
                        } break;
                        case 6: break;
                        default: cout << "Invalid neuron option.\n"; break;
                    }
                }
            } else if (mainOption == 2) {
                int layerOption = 0;
                while (layerOption != 6) {
                    cout << "\n--- Layer Manager ---\n";
                    cout << "1. View layers and neurons\n";
                    cout << "2. Set inputs\n";
                    cout << "3. Run layers\n";
                    cout << "4. Read outputs for a layer\n";
                    cout << "5. Show all layers/actions/receptors\n";
                    cout << "6. Back\n";
                    cout << "Choice: "; cin >> layerOption;

                    switch (layerOption)
                    {
                        case 1: layerManager.viewLayers(0); break;
                        case 2: layerManager.setInputs(); break;
                        case 3: layerManager.runLayers(); break;
                        case 4: cout << "Layer ID: "; int layerId; cin >> layerId;
                                layerManager.readOutputs(layerId);
                        break;
                        case 5: layerManager.showAll(); break;
                        case 6: break;
                        default: cout << "Invalid layer option.\n"; break;
                    }
                }
            } else if (mainOption == 3) {
                break;
            } else {
                cout << "Invalid menu option.\n";
            }
        }
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what() << endl;
    }

    return EXIT_SUCCESS;
}