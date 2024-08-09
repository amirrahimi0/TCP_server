# TCP Server 
This code listens on a specific port defined in the fconfig.ini file and saves any incoming messages that start with predefined prefixes.
Messages with the prefix `p1` are of type `integer`, and messages with the prefix `p2` are of type `string`.

program that every two minutes sorts all the stored messages with the prefix p1 in ascending order and prints them in the output.

# Data Base Implementation 
For this project, I used SQLite to store the data as shown below:

Messages with the prefix `p1`: These are of type integer and are stored accordingly.

Messages with the prefix `p2`: These are of type string and are stored accordingly.


| m_id | m_type_int | m_data | m_int | m_date   |
|------|------------|--------|-------|----------|
| 1    | 1          | 12     | 12    | 21:21:10 |
| 2    | 1          | 2      | 2     | 21:21:15 |
| 3    | 1          | 123    | 123   | 21:21:19 |
| 4    | 0          | 33     | 0     | 21:21:28 |
| 5    | 1          | 333    | 333   | 21:21:33 |

**m_id**: This is the unique identifier for each message entry.

**m_type_int**: This column indicates the type of the message. In this dataset, the value of m_type_int can be either 0 or 1. A value of 0 signifies that the message is of type p2, which means that the message data (m_data) should be interpreted as a string. When the value is 1, the message is not of type p2 and may have a different interpretation.

**m_data**: This field contains the actual content of the message. The data type for this column is text, which means it can hold either numeric values or string values depending on the context provided by m_type_int.

**m_int**: This column is an integer representation of the message data (m_data). If m_type_int is 0, the m_int value might be 0 or irrelevant because the message data is a string (p2). When m_type_int is 1, the m_int contains a numeric interpretation of m_data.

**m_date**: This attribute records the time when the message was processed or logged. It’s stored in a text format representing the time in the “HH:MM:SS” format.
