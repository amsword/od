An implementation of the algorithms in the following paper. 

Jianfeng Wang, Heng Tao Shen, Shuicheng Yan, Nenghai Yu, Shipeng Li, and Jingdong Wang. 
"Optimized Distances for Binary Code Ranking". ACM Multimedia (ACM MM), 2014

If you have any question, please contact me (wjfustc@gmail.com/wjf2006@mail.ustc.edu.cn).

--------------------------
Running:
        1. open matlab and set the working directory as the location where the file (readme.txt) is located.
        2. run demo_optimized_distance.m, which downloads the data, generate the ground truth based on Euclidean distance, train the model, test the performance and report the comparision of mean average ratio. 
-------------------------
Note:
        1. In the demo, we only use MNIST dataset, which is the smallest reported in the paper. Please revise the corresponding routines if you are interested in other datasets.
        2. The Hamming distance can be calculated fast by a few machine instructions. However, the instructions are dependent on the platform. For simplicity, the lookup table is used to retrieve the distance in the demo codes. If Windows is used, you can try to uncomment the line 3 and line24-39 of the file c_code/HammingDistance.cpp, which will enable the fast calculation scheme. If the code can be compiled and the program can not run, the reason may be that the  instructions are not supported. In this case, please comment out these sentences. 
------------------------