# usefull commands

# create new empty .img file
# bs    - block size, can be 1M, 2M, 4M, or 16M
# count - number of created blocks, end image size is bs * count
dd if=/dev/zero bs=4M count=200 > binary.img

# create ext4 partition on image file
mkfs.ext4 binary.img

# mount image file 
sudo mount -o loop binary.img mount_directory

# unmount image
sudo umount mount_directory

# display information about ext4 fs
dumpe2fs binary.img

# create file of fixed size
head -c 200M </dev/zero >myfile
