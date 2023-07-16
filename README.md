# SensorReporter
A platformio library / framework to try something. 


# How to use
See examples


## Worker
Workers are independent, producing data at times. This can be a sensor or serial read.

## Handler
Handlers rely on the produced data by the workers. They can process the data, 
like sending it to a remote API or over bluetooth.

## Supervisor
Supervisors have an overview of the complete state of the system, they can see what 
workers produced new data, and which handlers have completed their processing. 
A supervisor implementation can be an lcd screen or a LED or something. 