# lcr

Implementaion of lcr algorithm published in the article:

R. Guerraoui, R. R. Levy, B. Pochon, and V. Quéma. Throughput optimal total order broadcast for cluster environments. ACM Trans. Comput. Syst., 28(2):5:1–5:32, July 2010

The code is coded under C/C++ just as a simple prototype, and used for experimentations. It is not a real implementation.

In order to execute the application:
 <ul>
  <li>Create a file for the participating IPs called 'all_IPs' in your home directory and fill it line by line.</li>
  <li>Create an empty folder called 'output' at the root of the project, i.e. beside 'src' folder.</li>
  <li>make</li>
  <li>./run</li>
</ul>

Notes:
 <ul>
  <li>To see additional options ./run -h</li>
  <li>The default interface is 'eth0', make sure to choose your preferred one.</li>
</ul>
