#DISPATCH
*A robust C console application designed to simulate and manage an emergency dispatch system. The project implements complex custom data structures and algorithms from scratch to efficiently handle priority-based incident dispatching, crew availability, and intervention tracking.

*In the first part of the code, the structures specified in the assignment requirements are defined: unit as a simple structure; Incident and Intervention as doubly linked circular lists with a sentinel node; QueueInc and QueueUnit as queues for incidents and units; StackInt as a stack for interventions; and System as the parent structure containing all other structures. Functions like initIncident, initIntervention, initQueues, and initSystem handle initialization, with initSystem combining all individual initializations into a single function.

*Functions such as enqueueUnit, enqueueIncident, dequeueUnit, dequeueIncident, pushIntervention, and popIntervention serve as the core operations for adding elements to queues and stacks. Separate dequeue and enqueue functions were opted for units due to their simpler structure compared to incidents.

*The ADD_INCIDENT function creates a new node and adds it to the incident list. After setting the incident details (ID, description, priority), it checks which priority queue it belongs to and adds it using the enqueue function. 

*The CHECK_UNITS_AVAILABILITY function starts from the head of the availability queue and counts the units one by one to determine the total number.

*The DISPATCH function first checks if there are available units in the system queue. If so, it attempts to extract one incident at a time from the priority queues based on severity, ensuring the function handles the most severe accident first without moving to another priority queue while the current ones are not empty. Upon finding an incident in the queues, a unit is removed from the availability queue, an intervention is created with the necessary details, pushed onto the stack, and the unit is marked as unavailable.

*The UNDO_LAST_DISPATCH function traverses the intervention stack until it finds an incident with the "intervened" status. If found, its priority is checked and it is added back to the respective queue at the beginning. Afterwards, the unit is marked as available, added back to the queue, and the intervention is deleted from the stack. 

*The SOLVED_INCIDENT function traverses the intervention list until it finds the incident with the ID read from the file. Upon finding it, it verifies the status is "intervened", changes it to "solved", marks the unit as "available", and adds it back to the availability queue.

*The SHOW_UNIT and SHOW_INCIDENT functions traverse the vector and list respectively to search for the unit or incident with the ID read from the file, displaying all available details. 

*The SHOW_INTERVENTIONS function traverses the entire intervention list and sequentially displays details for each intervention.

*The free_system function is designed to release all dynamically allocated memory at the end of the program. It starts with the incident list (freeing both the description and the node itself), continues with the intervention list, priority/availability queues, and finally the stack, the units structure, and the system itself.

*In the Main function, the number of units and their details are saved first and added to the availability queue. Afterward, each command is read sequentially and compared against the existing functions. Upon a match, the subsequent variables are read conditionally and the respective function is called. Although most functions follow a similar implementation pattern, ADD_INCIDENT presented a more difficult case because the incident description is enclosed in quotes and is normally a phrase. The fscanf function stops at the first space, which is unsuitable here. To solve this problem, the space preceding the phrase was read in the previous fscanf, fgetc was used to read the exact next character ("), a special format was applied to fscanf to tell it to read anything up to the quotes, and it concluded with another fgetc call to strip the trailing quotes. After calling all functions read from the file, the files were closed and free_system was executed.
