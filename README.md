online logistic regression
===

This is a simple project aimed to do online logistic regression.
We hope it can make learning with big data easier for individuals 
and small groups. 

History
-------

It was started as a course project in 
[Statistical Learning](http://www.math.pku.edu.cn/teachers/yaoy/Spring2013/).
And then improved and tested in the 2nd Season of 
[iPinyou RTB Contest](http://contest.ipinyou.com/).

Structure
---------

In its simple structure, there are mainly two parts:

1. Feeder
	
   A Feeder is responsible for preparing and providing data for learning.
   Sources are located in `src/feeder` subdirectory.
   Currently a feeder for the 
   [iPinyou RTB Contest](http://contest.ipinyou.com/)
   offline data is implemented.

2. Learner

   A Learner takes parsed data and update model according learning rules.
   This is usually the main part of the learning algorithm.
   Sources are located in `src/learner` subdirectory.
   Currently only a truncated SGD method 
   (See [TrSGD](http://arxiv.org/abs/0806.4686).)
   for logistic regression is implemented.

Usage
-----

We have provided the code we used in the 
[iPinyou RTB Contest](http://contest.ipinyou.com/)
as an example.
This example is located at `examples/rtb2a`.
Suppose you already have the datafiles, then

1. Concatenate the clk files as a single file, put it at
   `data/clk.txt` ( Suppose you already changed your current directory to 
   `examples/rtb2a` ). Concatenate the conv files
   and put it at `data/conv.txt`.

2. Edit `imp.list` and replace the filenames with your datafiles' 
   real locations. (note that soft link may fail.)

3. Edit `param_learner.txt` to adjust learning parameters.

Now if you haven't build the binary, type `make` to build the main program.
Make sure that your C++ compiler supports `-std=c++0x` (or `-std=c++11`).
If everything goes fine, you can then start the learning process.

4. Run `./main imp.list 30000000 0` to learn the logistic regression model
   with 30000000 iterations of truncated SGD.

Note that by default the program will check the existence of `model.txt`
and auto-load it to continue the training. If you want to learn a new model,
you should delete the old file or move it to somewhere else.

To do
-----

1. More Learner

   We hope we can implement more algorithm for more models. This may include
   but not be limited to LSE, SVM, MF, NN.

1. More Feeder

   Possible support for CSV, gzipped/bziped txt file, etc.

2. Parallel support

   We are planning to add parallel support for single machine 
   (multiple threads) in the future. 


FAQ
---

1. Why do you use SGD instead of some 2nd order method?

   We are in favor of SGD instead of some 2nd order method 
   because SGD is more efficient when we have enormous data but
   limited time and computation resources. A good reference is
   [L.Bottou, Large-Scale Machine Learning with Stochastic Gradient Descent] (http://leon.bottou.org/publications/pdf/compstat-2010.pdf)

