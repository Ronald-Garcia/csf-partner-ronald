CONTRIBUTIONS

Cameron Kolisko and Ronald Garcia: both worked on the project using partner coding method, equal amounts of work
completed.

REPORT

Using the run_experiments shell script, we recieved the following results:
First iteration:
Test run with threshold 2097152

real    0m0.273s
user    0m0.261s
sys     0m0.012s

Second iteration:
Test run with threshold 1048576

real    0m0.154s
user    0m0.259s
sys     0m0.018s

Third iteration:
Test run with threshold 524288

real    0m0.116s
user    0m0.316s
sys     0m0.020s

Fourth iteration:
Test run with threshold 262144

real    0m0.111s
user    0m0.369s
sys     0m0.051s

Fifth iteration:
Test run with threshold 131072

real    0m0.113s
user    0m0.392s
sys     0m0.044s

Sixth iteration:
Test run with threshold 65536

real    0m0.112s
user    0m0.403s
sys     0m0.066s


Seventh iteration:
Test run with threshold 32768

real    0m0.118s
user    0m0.429s
sys     0m0.070s

Eigth iteration:
Test run with threshold 16384

real    0m0.123s
user    0m0.437s
sys     0m0.103s

We believe that we saw an increase in performance time in the second, third,
and fourth iterations because the program was able to utilize more of its cpu
cores to carry out the algorithim. After the fourth iteration, the performance
time began to become worse because there were no more cpu cores to subdivide the
running of the program among. This meant that there were uncessary fork calls
when all the cpu cores were in use, resulting in the child and parent running
sequentially rather than in parrallel. Because there is an overhead cost
associated with calling fork, and there is an increase in the number of 
unhelpful fork calls after the fourth iteration (for a computer with 8 cpu cores),
this would account for the increased run time of the fifth, sixth, seventh, and
eigth iterations.