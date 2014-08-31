function cb = compactbit(b)
wordsize = 8;
[nbits nSamples] = size(b);
nwords = ceil(nbits/wordsize);
cb = zeros([nwords nSamples], 'uint8');

for j = 1:nbits
  w = ceil(j/wordsize);
  cb(w,:) = bitset(cb(w,:), mod(j-1,wordsize)+1, b(j,:));
end
