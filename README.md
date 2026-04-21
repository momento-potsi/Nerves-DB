# Nerves-DB
> Application to model neural-like network

## BY: Oluwatosin Daudu

> [!info] Model of Nerve-like network using SQL & C++


> [!info] Background information
> * **Application Domain**: Education/Entertainment
>   
> * _What will your database store?_ 
> 
> > Our database will store data related to how a neural network would be structured
> > This would comprise of ...
> - [ ] Neurons table, I/O Receptors table, Action table, Layers table, Connections
> 	- [ ] Neurons - id, weights, name, status, role (strength/weak, create/remover), 
> 	- [ ] Receptors - neuron id, input/output weight, active?, +/-,
> 	- [ ] Action - action name, threshold, unique path, weight
> 	- [ ] Layers - layer index, name, function, neuron_count
> 	- [ ] Connections - id, from_id, to_id, type (layer/neuron connection)
> * **Purpose**:
> 	* _Problem Addressed_ - application addresses understanding gap by providing a **tangible, interactive representation of neural networks using a database-backed model**, allowing users to explore how neurons, layers, and connections behave in real time
> 	* _Intended Users/Use case_ - Educators or curious students
> 	* _Importance_ - Not only visually breaks down aspect of neural networks but also allows for creativity in subject
> * **Future Enhancements**:
> 	* _Scaling? (Ex: additional tables/documents, additional relationships)_ - I could potentially scale this up by adding tables for functional groups to emulate parts of biological brains
> 	* _Additional data to store_ - Learning Parameters and potentially Annotations



  
# ER Diagram

* Rough Draft of Nerves ERD

![[myerd.drawio.png]]

## Final SQL Structure Version 

> [!todo] Title
> - [ ] Todo apply fixes to sql; 
> - [ ] Implement C++ Application Layer

- Are you sure all your tables are normalized?

```SQL
show tables;
+------------------+
| Tables_in_nerves |
+------------------+
| Action           |
| Connections      |
| Layer            |
| Neurons          |
| Receptors        |
+------------------+
5 rows in set (0.00 sec)

mysql>     describe Layer;
+---------------+---------+------+-----+---------+-------+
| Field         | Type    | Null | Key | Default | Extra |
+---------------+---------+------+-----+---------+-------+
| LayerId       | int(11) | NO   | PRI | NULL    |       |
| LayerName     | text    | NO   |     | NULL    |       |
| LayerFunction | text    | NO   |     | NULL    |       |
| NeuronCount   | int(11) | NO   |     | NULL    |       |
+---------------+---------+------+-----+---------+-------+
4 rows in set (0.00 sec)

mysql>     SELECT * FROM Layer;
+---------+-------------------+---------------+-------------+
| LayerId | LayerName         | LayerFunction | NeuronCount |
+---------+-------------------+---------------+-------------+
|       0 | Input Layer [IL]  | MAPIN         |           2 |
|       1 | Hiden Layer [HL1] | ADD           |           2 |
|      99 | Output Layer [OL] | MAPOUT        |           2 |
+---------+-------------------+---------------+-------------+
3 rows in set (0.00 sec)

mysql>     describe Neurons;
+--------------+---------------+------+-----+---------+-------+
| Field        | Type          | Null | Key | Default | Extra |
+--------------+---------------+------+-----+---------+-------+
| Id           | int(11)       | NO   | PRI | NULL    |       |
| NeuronName   | text          | NO   |     | NULL    |       |
| NeuronWeight | decimal(10,2) | NO   |     | NULL    |       |
| NeuronStatus | text          | NO   |     | NULL    |       |
| NeuronRole   | text          | NO   |     | NULL    |       |
| LayerId      | int(11)       | NO   | MUL | NULL    |       |
+--------------+---------------+------+-----+---------+-------+
6 rows in set (0.00 sec)

mysql>     SELECT * FROM Neurons;
+----+------------+--------------+--------------+------------+---------+
| Id | NeuronName | NeuronWeight | NeuronStatus | NeuronRole | LayerId |
+----+------------+--------------+--------------+------------+---------+
|  0 | INPUT1     |        10.00 | INACTIVE     | INPUT      |       0 |
|  1 | INPUT2     |        10.00 | INACTIVE     | INPUT      |       0 |
| 10 | HL1 Neuron |        10.00 | INACTIVE     | PROCESS    |       1 |
| 11 | HL2 Neuron |        10.00 | INACTIVE     | PROCESS    |       1 |
| 98 | OUTPUT2    |        10.00 | INACTIVE     | OUTPUT     |      99 |
| 99 | OUTPUT1    |        10.00 | INACTIVE     | OUTPUT     |      99 |
+----+------------+--------------+--------------+------------+---------+
6 rows in set (0.00 sec)

mysql>     describe Connections;
+----------------+------------------------+------+-----+---------+----------------+
| Field          | Type                   | Null | Key | Default | Extra          |
+----------------+------------------------+------+-----+---------+----------------+
| ConnectionId   | int(11)                | NO   | PRI | NULL    | auto_increment |
| ConnectionType | enum('NEURON','LAYER') | NO   |     | NULL    |                |
| FromId         | int(11)                | NO   |     | NULL    |                |
| ToId           | int(11)                | NO   |     | NULL    |                |
+----------------+------------------------+------+-----+---------+----------------+
4 rows in set (0.00 sec)

mysql>     SELECT * FROM Connections;
+--------------+----------------+--------+------+
| ConnectionId | ConnectionType | FromId | ToId |
+--------------+----------------+--------+------+
|            1 | NEURON         |      0 |   10 |
|            2 | NEURON         |      0 |   11 |
|            3 | NEURON         |      1 |   10 |
|            4 | NEURON         |      1 |   11 |
|            5 | NEURON         |     10 |   98 |
|            6 | NEURON         |     10 |   99 |
|            7 | NEURON         |     11 |   98 |
|            8 | NEURON         |     11 |   99 |
|            9 | LAYER          |      0 |    1 |
|           10 | LAYER          |      1 |   99 |
+--------------+----------------+--------+------+
10 rows in set (0.00 sec)

mysql>     describe Receptors;
+-----------+------------+------+-----+---------+-------+
| Field     | Type       | Null | Key | Default | Extra |
+-----------+------------+------+-----+---------+-------+
| NeuronId  | int(11)    | NO   | PRI | NULL    |       |
| Active    | tinyint(1) | NO   |     | NULL    |       |
| Sign      | int(11)    | NO   |     | NULL    |       |
| IO_Weight | int(11)    | NO   |     | NULL    |       |
+-----------+------------+------+-----+---------+-------+
4 rows in set (0.00 sec)

mysql>     SELECT * FROM Receptors;
+----------+--------+------+-----------+
| NeuronId | Active | Sign | IO_Weight |
+----------+--------+------+-----------+
|        0 |      0 |    1 |        10 |
|        1 |      0 |    1 |        10 |
|       98 |      0 |    1 |        10 |
|       99 |      0 |    1 |        10 |
+----------+--------+------+-----------+
4 rows in set (0.00 sec)

mysql>     describe Action;
+----------------+---------------+------+-----+---------+----------------+
| Field          | Type          | Null | Key | Default | Extra          |
+----------------+---------------+------+-----+---------+----------------+
| ActionId       | int(11)       | NO   | PRI | NULL    | auto_increment |
| ActionName     | text          | NO   |     | NULL    |                |
| Threshold      | decimal(10,2) | NO   |     | NULL    |                |
| UniquePath     | int(11)       | NO   |     | NULL    |                |
| AssignedWeight | decimal(10,2) | NO   |     | NULL    |                |
+----------------+---------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)

mysql>     SELECT * FROM Action;
+----------+---------------+-----------+------------+----------------+
| ActionId | ActionName    | Threshold | UniquePath | AssignedWeight |
+----------+---------------+-----------+------------+----------------+
|        1 | Reinforcement |     20.00 |          0 |          50.00 |
|        2 | Punishment    |     20.00 |          0 |          50.00 |
|        3 | Investigate   |     10.00 |          0 |          15.00 |
|        4 | Re-Adjust     |     15.00 |          0 |          20.00 |
|        5 | Get Insight   |      5.00 |          0 |           0.00 |
+----------+---------------+-----------+------------+----------------+
5 rows in set (0.00 sec)
```

---

Sample Database Operation Example

```sql
  CREATE VIEW

  pianoplayers AS

  SELECT FirstName, LastName FROM musicians

  JOIN instrumentsplayed ON musicians.Id = instrumentsplayed.Musician

  WHERE instrumentsplayed.Instrument = 'Piano';

  

  INSERT INTO musicians (Id, FirstName, LastName, Born) VALUES (4, 'Art', 'Tatum', 1909);

  INSERT INTO instrumentsplayed (Id, Musician, Instrument) VALUES (5, 4, 'Piano');

  

  update instrumentsplayed set Instrument='drums' where Id=5;

  

  ALTER TABLE musicians MODIFY COLUMN Id INT NOT NULL AUTO_INCREMENT;

  

  INSERT INTO musicians (FirstName, LastName, Born) VALUES ('Bill', 'Evans', 1908);

  
  

  DELIMITER $$

  CREATE PROCEDURE saxes()

  BEGIN

  

    SELECT FirstName, LastName FROM musicians

    JOIN instrumentsplayed ON musicians.Id = instrumentsplayed.Musician

    WHERE instrumentsplayed.Instrument = 'Saxophone';

  

  END

  $$

  DELIMITER ;
```
