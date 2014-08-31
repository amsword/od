
num_point = 60 * 10^3;
num_query = 100;
dim = 128;
topks = round(0.02 * num_point);

rng(1);
Xtraining = rand(dim, num_point);
Xtest = rand(dim, num_query);
%% get the ground truth based on the Euclidean distance
R = Xtest' * Xtraining;
sqrXtest = sum(Xtest .^ 2);
sqrXtraining = sum(Xtraining .^ 2);
dist = bsxfun(@plus, sqrXtest', sqrXtraining);
dist = real(sqrt(dist-2*R));
[s_dist, gnd] = sort(dist, 2);
gnd = gnd(:, 1 : max(topks));
gnd = int32(gnd);
gnd = gnd';
gnd = gnd - 1;