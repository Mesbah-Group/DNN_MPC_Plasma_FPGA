%% script to load data and write contents of NN to text file

% modify the filename here to load a different file
load('DNN_training.mat')

% generate filename for the output text file
filename = 'dnn.txt';

% assume the NN is under the variable name 'net'
net2txt(net, filename);