# RF32 FILESYSTEM (aka RANDOM FILESYSTEM 32 BIT)

## Basic overview

The RF32 Filesystem allocates 0x200000 (aka 2 MiB) bytes for each file, no more, no less. The file pointers start at precisely 0x100000 (aka 1 MiB) in the disk.
Theres 1 root directory, and ONLY 1.
Each and every directory has a limit of 8 subdirectories, named sub{N}, where N is an integer 0 - 7. 
Example:
root:
    sub0,
    sub1,
    sub2,
    sub3,
    sub4,
    sub5,
    sub6,
    sub7

In the hard disk, one directory gets 4 * 8 bytes allocated for it. Each pair of 4 bytes is a file entry. If the 4 bytes (`uint32_t`) equal 1, it's a subdirectory. If its 0, it's empty. If anything else, it's a file pointer.


## What are file pointers stored like?

Each file pointer points to an LBA in the disk. At the start of every file is a null-terminated string specifying the file name. After that, is the file data. The file data is terminated by the magic number 0x12785634.
