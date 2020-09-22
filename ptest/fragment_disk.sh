N=500

M=170


rm -r dummy_files
mkdir dummy_files
touch dummy_files/dummy_file.txt
dd if=/dev/random of=dummy_files/dummy_file.txt bs=1024 count=10000
for v in {1..500}; do cp dummy_files/dummy_file.txt dummy_files/dummy_file$v.txt; done
for v in {1..150}; do rm dummy_files/dummy_file$v.txt; done
