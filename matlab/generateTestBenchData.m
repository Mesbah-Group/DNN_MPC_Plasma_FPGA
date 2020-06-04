%% script to generate test bench data for HLS
% load NN from file
load('DNN_training.mat', 'net')

in = [1 1 1]';
out = sim(net, in);

filename = 'out.txt';
sprintf('The output of the NN was')
out
sprintf('and was written to a file named %s', filename)
fh = fopen(filename, 'w');
fprintf(fh, '%f ', out);
