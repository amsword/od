function matE = ConstructMat(E)
    num_partitions = size(E, 1);
    assert(size(E, 2) == num_partitions);

    num_total_bucket = 0;
    for i = 1 : num_partitions
        num_total_bucket = num_total_bucket + size(E{i, 1}, 1);
    end
    
    matE = zeros(num_total_bucket, num_total_bucket);

    idx_row = 1;
    for i = 1 : num_partitions
        idx_col = 1;
        for j = 1 : num_partitions
            num_row = size(E{i, j}, 1);
            num_col = size(E{i, j}, 2);
            
            matE(idx_row : idx_row + num_row - 1, ...
                idx_col : idx_col + num_col - 1) ...
                = E{i, j};
            
            idx_col = idx_col + num_col;
        end
        idx_row = idx_row + num_row;
    end
    
end