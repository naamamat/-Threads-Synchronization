# Threads-Synchronization

## Overview
This project is designed to provide hands-on experience with concurrent programming and synchronization mechanisms. It simulates a producer-consumer system with multiple producers, dispatchers, co-editors, and a screen manager, utilizing various bounded buffer queues to synchronize their interactions.

## System Architecture
- **Producers**: Multiple producers create products and communicate with the dispatcher via their individual queues.
- **Dispatcher**: The dispatcher processes messages from the producer queues and sends them to three Co-Editors through designated queues for each message type.
- **Co-Editors**: These components manage messages from the dispatcher and communicate with the Screen Manager via a shared queue.
- **Screen Manager**: The screen manager retrieves messages from the Co-Editors' shared queue and displays the system's output.

## Bounded Buffer
Both the Producer queues and the shared queue for the Co-Editors are bounded buffers, supporting efficient concurrent message passing between components.

## Configuration File
The configuration file defines the setup for each producer and the Co-Editors' shared queue. The format is as follows:

```plaintext
PRODUCER 1
[number of products]
queue size = [size]

PRODUCER 2
[number of products]
queue size = [size]

...

PRODUCER n
[number of products]
queue size = [size]

Co-Editor queue size = [size]


### Example Configurations
Here are examples of valid configuration files:

- **Option 1**:
  ```plaintext
  PRODUCER 1
  30
  queue size = 5
  PRODUCER 2
  25
  queue size = 3

- **Option 2**:
 ```plaintext
PRODUCER 3
16
queue size = 30
Co-Editor queue size = 17

These configurations specify the number of products each producer will create and the size of each producer queue and the Co-Editors' shared queue.

How to Run
Follow these steps to compile and execute the project:

Navigate to the project directory:

bash
Copy code
cd Threads-Synchronization
Build the project using make:

bash
Copy code
make
Run the program with the configuration file:

bash
Copy code
./ex3.out conf.txt
Ensure that the configuration file conf.txt is in the project directory or specify the correct path to it.
