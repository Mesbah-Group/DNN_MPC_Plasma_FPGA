%% script to generate test bench data for HLS
% load NN from file
load('DNN_training.mat', 'net')

in = [-0.3 0.5 0.7]';
in = [0.329648 0.721062 0.496215]';
out = sim(net, in);

filename = 'out.txt';
sprintf('The output of the NN was')
out
sprintf('and was written to a file named %s', filename)
fh = fopen(filename, 'w');
fprintf(fh, '%f ', out);

% copied from casadiDNN.m from Angelo's code
% extract weights and biases from neural network
% L = 5;
% W = cell(L+1,1);
% b = cell(L+1,1);
% W{1} = net.IW{1};
% b{1} = net.b{1};
% for i = 1:L
%     W{i+1} = net.LW{i+1,i};
%     b{i+1} = net.b{i+1};
% end
% z = max(W{1}*in+b{1},0);
% for k = 1:L-1
%     z = max(W{k+1}*z+b{k+1},0);
% end
% us = W{L+1}*z+b{L+1}

in = mapminmax('apply', in, net.inputs{1}.processSettings{1});
z = net.IW{1} * in + net.b{1};
for i = 1:net.numLayers-1
    z = poslin(z,0);
    z = net.LW{i+1,i} * z + net.b{i+1};
end
z = mapminmax('reverse', z, net.outputs{end}.processSettings{1})