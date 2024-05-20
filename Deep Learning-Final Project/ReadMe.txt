Benett Holmstrom's Final COSE 474 Project: vehicle make + model recognition

Before you run it, check that you have these dependencies installed on your machine, or else it will not run:
- keras
- OpenCV
- Numpy
- Tensorflow

Next, you need to download two .tgz files that will unpack Stanford's Cars Dataset into the main folder, "Final".
Please see the link attached to the submission's comment. 

Running the program:
- cd to folder ".../Final" to ensure you are in the right place
- Unpack the dataset and preprocess it by calling:

<python pre_process.py>

- Then train the data:

<python train.py>

- You can display the graphs and statistics of the program by calling:

<tensorboard --logdir path_to_current_dir/logs>

- Display the confusion matrices:

<python analyze.py>

- You can train specific images of your choosing if you want. Call:

<python demo.py --i [path to your image]

If any errors occur or you believe that the program is not running as intended, please reach me at
benettholmstrom@gmail.com

