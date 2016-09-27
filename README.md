# lcr

Implementaion of lcr algorithm published in the article:

R. Guerraoui, R. R. Levy, B. Pochon, and V. Quéma. Throughput optimal total order broadcast for cluster environments. ACM Trans. Comput. Syst., 28(2):5:1–5:32, July 2010

The code is coded under C/C++ just as a simple prototype, and used for experimentations. It is not a real implementation.

In order to execute the application:
1. Create a file for the participating IPs called 'all_IPs' in your home directory and fill it line by line.
2. Create an empty folder called 'output' at the root of the project, i.e. beside 'src' folder.
2. make
3. ./run

Notes:
1. To see additional options ./run -h
2. The default interface is 'eth0', make sure to choose your preferred one.
