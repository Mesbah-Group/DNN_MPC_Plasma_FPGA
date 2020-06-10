%% script to load data and write contents of NN to text file

% modify the filename here to load a different file
load('DNN_training.mat', 'net', 'xscale_max', 'xscale_min')

% generate filename for the output text file
filename = 'dnn.txt';

fileID = fopen(filename, 'w');

% write max and min to scale data to file
fprintf(fileID, 'max\n');
fprintf(fileID, '%f ', xscale_max);
fprintf(fileID, '\n');
fprintf(fileID, 'min\n');
fprintf(fileID, '%f ', xscale_min);
fprintf(fileID, '\n');

fclose(fileID);

% assume the NN is under the variable name 'net'
net2txt(net, filename);