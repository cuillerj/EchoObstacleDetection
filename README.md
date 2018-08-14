# EchoObstacleDetection
an asynchronous obstacle detection for robot

This aim of this tool is to provide an asynchronous echo measurement system that can be used by a robot when moving.
It provides distance measurements and can also monitor that distances are under defined threshold and even a combination of mersurments.
If not provide an interrupt to the main code.
It can manage from 1 to 4 echos components. It has been designed and tested with HY-SRF05 and HC SR04 module. It must work as well with HC SR03.
Usage limitation: when using more than 1 HCSR modules, their location must be such that the echos will not interfer.

This software is free.
