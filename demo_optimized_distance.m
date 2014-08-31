addpath('matlab_code/');
fprintf('Compiling the c codes\n');
cd c_code
compile_all;
cd ..

%% configure
code_length = 32;
num_partition = 3;

fprintf('load data\n');
%% Use mnist as an example. Please replace them if a specific dataset is required.
gen_mnist_dataset;
% gen_random_dataset;

dim = size(Xtraining, 1);
% how many nearest points are retrieved for each query. Setting it as a
% small value can speed up the retrieval time, 
% e.g. (0.1 * size(Xtraining, 2))
num_candidate = size(Xtraining, 2); 

%% compute the ground truth based on Euclidean distance, which is the focus
% of our paper. 

fprintf('compute the ground truth based on Euclidean distance\n');
topks = round(0.02 * size(Xtraining, 2));
sqrXtraining = sum(Xtraining .^ 2);

batch_size = 100;
batch_num = ceil(size(Xtest, 2) / batch_size);
for batch_idx = 1 : batch_num
    fprintf('%f/%f\n', batch_idx, batch_num);
    idx_start = (batch_idx - 1) * batch_size + 1;
    idx_end = idx_start + batch_size - 1;
    idx_end = min(idx_end, size(Xtest, 2));
    if idx_start > idx_end
        break;
    end
    subXtest = Xtest(:, idx_start : idx_end);
    R = subXtest' * Xtraining;
    sqrXtest = sum(subXtest .^ 2);
    dist = bsxfun(@plus, sqrXtest', sqrXtraining);
    dist = real(sqrt(dist-2*R));
    [s_dist, sub_gnd] = sort(dist, 2);
    if batch_idx == 1
        gnd = zeros(max(topks), size(Xtest, 2), 'int32');
    end
    gnd(:, idx_start : idx_end) = int32(sub_gnd(:, 1 : max(topks))');
end
gnd = gnd - 1; % index start from 0;


%% random projection to generate the binary codes. 
% please replace these codes if other encoding algorithm is used. 
fprintf('generate binary codes\n');
mean_value = mean(Xtraining, 2);
W1T = randn(code_length, dim);
W2T = -W1T * mean_value;
W = [W1T'; W2T'];

c = bsxfun(@ge, W(1 : end - 1, :)' * Xtraining, -W(end, :)');
base_binary_code = compactbit(c);

c = bsxfun(@ge, W(1 : end - 1, :)' * Xtest, -W(end, :)');
query_binary_code = compactbit(c);

%% criteria to evaluate the performance
clear all_cri;
k = 1;
all_cri{k} = {0, int32(topks), gnd}; k = k + 1; % precision and recall
all_cri{k} = {4, int32(topks), gnd};  k = k + 1; % map
all_cri{k} = {2, gnd};  k = k + 1; % average distance ratio

%% pre-compute the matrices from the raw data and the binary codes.
% see 
%
fprintf('learning\n');
[D_dense, aux, centers, errors] = ...
    merge_opt2(...
    base_binary_code, ...
    num_partition, ...
    Xtraining);

%% distance information. Three distances are used in this demo
clear all_dist;
k = 1;
% Hamming distance
all_dist{k} = {0}; k = k + 1; 

% optimized symmetric distance
all_dist{k} = {2, num_partition, D_dense}; k = k + 1; 

% for optimized asymmetric distance
all_dist{k} = {3, num_partition, aux, centers, errors}; k = k + 1;
%% test the performance
clear all_perf;
fprintf('testing\n');
for i = 1 : numel(all_dist)
    i
    all_perf{i} = MmexTestTrueNeighbors(...
        query_binary_code, ...
        base_binary_code, ...
        num_candidate, ...
        Xtest, ...
        Xtraining, ...
        all_dist{i}, ...
        all_cri);
end
%%
line = 'mAP: ';
fprintf('%s\n', line);
line = ['Optimized Asymmetric Distance: ' num2str(all_perf{3}.map)];
fprintf('%s\n', line);
line = ['Optimized Symmetric Distance: ' num2str(all_perf{2}.map)];
fprintf('%s\n', line);
line = ['Hamming Distance: ' num2str(all_perf{1}.map)];
fprintf('%s\n', line);


% figure;
% hold on;
% plot(all_perf{3}.recall(1 : 10^3), 'r', 'linewidth', 1.5);
% plot(all_perf{2}.recall(1 : 10^3), 'c', 'linewidth', 1.5);
% plot(all_perf{1}.recall(1 : 10^3), 'b', 'linewidth', 1.5);
% set(gca, 'fontsize', 14);
% grid on;
% legend('OAD', 'OSD', 'HM', 'Location', 'NorthWest');
% xlabel('Number of Retrieved Points');
% ylabel('Recall');
%
