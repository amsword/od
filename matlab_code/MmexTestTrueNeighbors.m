function y = ...
    MmexTestTrueNeighbors(...
    matquery, ...
    mat_data_base_code, ...
    num_candidate,...
    Xtest, ...
    Xbase, ...
    conf, ...
    cri)


% assert(isa(matquery, 'uint8'));
% assert(isa(mat_data_base_code, 'uint8'));

assert(isa(Xbase, 'double') ||isa(Xbase, 'float'));
[result] = mexTestAllPerformance(...
    matquery, ...
    mat_data_base_code, ...
    num_candidate, ...
    Xtest, ...
    Xbase, ...
    conf, ...
    cri);

if ~isempty(result.fixed_topk)
    fixed_topk = double(result.fixed_topk);
    fixed_topk = cumsum(fixed_topk, 1);
    fixed_topk = fixed_topk / size(matquery, 2);
    
    y.precision = bsxfun(@rdivide, fixed_topk, [1 : num_candidate]');
    
    for i = 1 : numel(cri)
        if cri{i}{1} == 0
            topks = double(cri{i}{2});
        end
    end
    
    y.recall = bsxfun(@rdivide, fixed_topk, topks(:)');
end

if ~isempty(result.average_distance_ratio)
    dist_ratio = result.average_distance_ratio;
    dist_ratio = cumsum(dist_ratio);
    dist_ratio = dist_ratio ./ [1 : numel(dist_ratio)]';
    y.dist_ratio = dist_ratio / size(matquery, 2);
end

if ~isempty(result.map)
    y.map = result.map;
end
