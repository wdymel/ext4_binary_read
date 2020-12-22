### DESCRIPTION ###
This is code for my university for reading code of a binary image of ext4 partition.
The code works, although it wasn't testes on hash directories as I was unable to get my systems driver to create them.
This also doesn't support Meta Block groups as they seem to only be created for very large file systems.
This code comes with a simple shell that supports ls, cd, and cat commands.

ls - lists contents of current directory displaying file type, inode number, and name
cd - changes directory, does not support relational file names ie. ../dir, or dir/a/b,
     only works on links in current dir (including "." and "..")
cat - displays contents of a file in a classic hexadecimal format with byte index on the left and 16 bytes values on the right
      also displays a mark every sector as a page <number>


### COMPILING ###
To compile under linux use gcc with standard build-essentials package. Make file provided.


### TESTING ###
Testing this code on live devices is not advised (although no write operation ever occur, better be safe than sorry).
Instead it's better to create an image file and create ext4 partition on it.

1. create new empty .img file
    dd if=/dev/zero bs=4M count=200 > binary.img
 where:
 bs    - block size, can be 1M, 2M, 4M, or 16M
 count - number of created blocks
 end image size is bs * count

2. create ext4 partition on image file
    mkfs.ext4 binary.img

3. mount image file under a folder
    sudo mount -o loop binary.img mount_directory

4. After doing so it may also be necessary to change file ownership as this newly default partition will be owned by root user
    sudo chown your_user_name mount_directory

You can now freely create files and directories on the image file and test the program.
Afterward to unmount the image use
    sudo umount mount_directory

To display statistics about created ext4 partitions use
    dumpe2fs binary.img

To create a file of fixed size use
    head -c 200M </dev/zero >myfile

Provided with the code are also some sequential files. Those files consists of N uint64_t numbers (seqN.bin) written info file in order.
They are useful to confirm that files spanning multiple extents are read in correct order.
