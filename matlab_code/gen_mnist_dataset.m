str_dir = 'data/';
str_training = [str_dir  'train-images-idx3-ubyte'];
str_test = [str_dir  't10k-images-idx3-ubyte'];

mkdir(str_dir);

if ~exist(str_training, 'file')
    str_training_gz = [str_training '.gz'];
    urlwrite('http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz', ...
        str_training_gz);
    gunzip(str_training_gz);
    delete(str_training_gz);
end
if ~exist(str_test, 'file')
    str_test_gz = [str_test '.gz'];
    urlwrite('http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz', ...
        str_test_gz);
    gunzip(str_test_gz);    
    delete(str_test_gz);
end

Xtraining = read_mnist(str_training);
Xtraining = double(Xtraining);
Xtest = read_mnist(str_test);
Xtest = double(Xtest);
