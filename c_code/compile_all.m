mex mexComputeG.cpp ...
    BinaryCodePartition.cpp ...
    -output  ../matlab_code/mexComputeG

mex mexComputeE.cpp ...
    BinaryCodePartition.cpp ...
    -output  ../matlab_code/mexComputeE

%%
mex mexTestAllPerformance.cpp ...
    Criterion.cpp ...
    AverageDistanceRatioCriterion.cpp ...
    BinaryCodePartition.cpp ...
    DenseDistance.cpp ...
    DistanceCalculator.cpp ...
    FixedTopksCriterion.cpp ...
    HammingDistance.cpp ...
    QueryOptDistance.cpp ...
    mAPCriterion.cpp ...
    SearchEngine.cpp ...
    -output ../matlab_code/mexTestAllPerformance ...
	CXXFLAGS="\$CXXFLAGS -fopenmp" LDFLAGS="\$LDFLAGS -fopenmp"
