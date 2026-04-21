DROP database IF EXISTS nerves;
CREATE database nerves;
USE nerves;

-- Authors: Oluwatosin Daudu, Massai Morgan --
-- ---------------------------------------------------------------------------- --
-- Database stores data related to how a neural network / brain would be structured.
-- Additionally, the program operates on the database to try to demonstrate how
-- thinking and actions would be carried out.

-- Tables: Neurons, Receptors, Action, Layer, Connections
-- ---------------------------------------------------------------------------- --

-- Actions (Results) from the network/receptors
CREATE TABLE Action 
(
    ActionId       INT                NOT NULL auto_increment,
    ActionName     TEXT               NOT NULL,
    -- minimum absolute weight needed for action
    Threshold      DECIMAL(10, 2)     NOT NULL,
    -- last set of neuron ids resulting in path (123 -> neurons with id 1 -> 2 -> 3)
    UniquePath     INT                NOT NULL, 
    -- Strength of Action (just barely over threshold?)
    AssignedWeight DECIMAL(10, 2)     NOT NULL, 
    
    PRIMARY KEY (ActionId)
);

CREATE TABLE Layer 
(
    LayerId       INT  NOT NULL,
    LayerName     TEXT NOT NULL,
    LayerFunction TEXT NOT NULL, -- Make enum?
    NeuronCount   INT  NOT NULL,
    PRIMARY KEY (LayerId)
);

-- Tables must declared after to reference other tables.

-- Each neural node in network
CREATE TABLE Neurons 
( 
    -- Had to put `Neuron` prefix to avoid keywords.
    Id           INT            NOT NULL,
    -- Node name / Identifier
    NeuronName   TEXT           NOT NULL, 
    -- Weight / effect neuron has in network
    NeuronWeight DECIMAL(10, 2) NOT NULL,
    -- Neuron current status (Recieving, Processing, Sending, Inactive)
    NeuronStatus TEXT           NOT NULL, 
    -- Strengthen/Weaken network signal? Process? Create/Remove nodes/connections?
    NeuronRole   TEXT           NOT NULL, 
    -- Neuron Layer
    LayerId      INT            NOT NULL,  -- ADDED

    PRIMARY KEY (Id),
    FOREIGN KEY (LayerId) REFERENCES Layer(LayerId)
);

-- Neurons that are Receptors (determines Input/Output into neural network)
CREATE TABLE Receptors 
(
    NeuronId  INT     NOT NULL,
    -- Affects network currently?
    Active    BOOLEAN NOT NULL,
    -- Negative means less likely input/output from receptor
    Sign      INT     NOT NULL, 
    IO_Weight INT     NOT NULL,

    PRIMARY KEY (NeuronId),
    FOREIGN KEY (NeuronId) REFERENCES Neurons(Id)
);

-- Connections in the network
CREATE TABLE Connections 
(
    ConnectionId   INT                     NOT NULL auto_increment,
    -- Connects 2 neurons/layers
    ConnectionType ENUM('NEURON', 'LAYER') NOT NULL, 
    -- following 2 either REFERENCES NeuronId or LayerId
    FromId         INT                     NOT NULL, 
    ToId           INT                     NOT NULL,    

    PRIMARY KEY (ConnectionId)
);



-- Initial Insertions:

-- > Actions (Meant to model things like: Classification, Controls, Decisions)
-- >  > If UniquePath == 0 -> no action yet
INSERT INTO Action (ActionName, Threshold, UniquePath, AssignedWeight) VALUES ('Reinforcement', 20.0, 0, 50.0);
INSERT INTO Action (ActionName, Threshold, UniquePath, AssignedWeight) VALUES ('Punishment',    20.0, 0, 50.0);
INSERT INTO Action (ActionName, Threshold, UniquePath, AssignedWeight) VALUES ('Investigate',   10.0, 0, 15.0);
  -- Can add more Adjust actions to model choices
INSERT INTO Action (ActionName, Threshold, UniquePath, AssignedWeight) VALUES ('Re-Adjust',     15.0, 0, 20.0);
INSERT INTO Action (ActionName, Threshold, UniquePath, AssignedWeight) VALUES ('Get Insight',    5.0, 0, 0.0);

-- > Layers 
INSERT INTO Layer (LayerId, LayerName, LayerFunction, NeuronCount) VALUES (0, 'Input Layer [IL]',  'MAPIN',  0);
INSERT INTO Layer (LayerId, LayerName, LayerFunction, NeuronCount) VALUES (1, 'Hiden Layer [HL1]', 'ADD',    0);
INSERT INTO Layer (LayerId, LayerName, LayerFunction, NeuronCount) VALUES (99, 'Output Layer [OL]', 'MAPOUT', 0);

-- > Neurons (Intially set up for 10 neurons per 10 layers)
INSERT INTO Neurons (Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId) -- Input Neurons
            VALUES  (0, 'INPUT1',   10.0,         'INACTIVE',   'INPUT',    0);
INSERT INTO Neurons (Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId)
            VALUES  (1, 'INPUT2',   10.0,         'INACTIVE',   'INPUT',    0);
UPDATE Layer SET NeuronCount = 2 WHERE LayerId = 0;

INSERT INTO Neurons (Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId) -- Layer 1 Neurons
            VALUES  (10, 'HL1 Neuron', 10.0,       'INACTIVE',   'PROCESS',  1);
INSERT INTO Neurons (Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId)
            VALUES  (11, 'HL2 Neuron', 10.0,       'INACTIVE',   'PROCESS',  1);
UPDATE Layer SET NeuronCount = 2 WHERE LayerId = 1;

INSERT INTO Neurons (Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId) -- Output Neurons
            VALUES  (98, 'OUTPUT2',   10.0,        'INACTIVE',   'OUTPUT',   99); -- Map to `Reinforcement` action
INSERT INTO Neurons (Id, NeuronName, NeuronWeight, NeuronStatus, NeuronRole, LayerId)
            VALUES  (99, 'OUTPUT1',   10.0,        'INACTIVE',   'OUTPUT',   99); -- Map to `Punishment` action
UPDATE Layer SET NeuronCount = 2 WHERE LayerId = 99;

-- > Receptors
-- >  > Active (0 - false, 1 - true), Sign (1 -> +, -1 -> -)
INSERT INTO Receptors (NeuronId, Active, Sign, IO_Weight) VALUES (0, 0, 1, 10.0); -- Input neurons
INSERT INTO Receptors (NeuronId, Active, Sign, IO_Weight) VALUES (1, 0, 1, 10.0);
INSERT INTO Receptors (NeuronId, Active, Sign, IO_Weight) VALUES (98, 0, 1, 10.0); -- Output neurons
INSERT INTO Receptors (NeuronId, Active, Sign, IO_Weight) VALUES (99, 0, 1, 10.0);


-- > Connections (only go forward through layers/network)
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 0, 10); -- Neuron0
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 0, 11);
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 1, 10); -- Neuron1
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 1, 11);
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 10, 98); -- Neuron10
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 10, 99);
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 11, 98); -- Neuron11
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('NEURON', 11, 99);

INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('LAYER', 0, 1); -- Layers
INSERT INTO Connections (ConnectionType, FromId, ToId) VALUES ('LAYER', 1, 99);


/* To see everything.
    show tables;
    describe Layer;
    SELECT * FROM Layer;
    describe Neurons;
    SELECT * FROM Neurons;
    describe Connections;
    SELECT * FROM Connections;
    describe Receptors;
    SELECT * FROM Receptors;
    describe Action;
    SELECT * FROM Action;
*/
