function images = read_mnist(file_name)

fp = fopen(file_name, 'rb');

fread(fp, 1, 'int32');

num_image = fread(fp, 1, 'int32=>int32');
num_image = swapbytes(num_image);

num_rows = fread(fp, 1, 'int32=>int32');
num_rows = swapbytes(num_rows);

num_cols = fread(fp, 1, 'int32=>int32');
num_cols = swapbytes(num_cols);

images = fread(fp, [double(num_cols) * double(num_rows), double(num_image)], 'uint8=>uint8');


fclose(fp);
