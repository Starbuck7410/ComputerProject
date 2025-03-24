# Disk Drive

The SIMP processor is connected to a disk drive of variable size, which is
composed of 64 byte (16 words)  sectors.
There isn't a required "partition table" or "file system" but for my personal convenience I will define SFS (Simp File System) later on in this document.

## Operation
The disk is connected to interrupt number 1, irq1, and uses DMA to copy the sector  contents from memory to the disk or vice versa. The initial contents of the disk drive is given in the input file diskin.txt, and the contents of the disk at the end of the simulation should be written to the output file diskout.txt. The assembly code can check that the disk drive is ready to receive a new command by checking the value of the hardware register diskstatus. Assuming the disk is ready, the assembly code writes to the hardware register disksector the sector number that we wish to read or write, and to the hardware register diskbuffer the memory address of the buffer in main memory. Once those two registers are initialized, a write or read command can be started by writing to the hardware register diskcmd. The service time of the disk drive for a read or write command is 512 clock cycles. During this time, the disk copies, using DMA, the contents of the memory buffer to the disk in case of a write command, or vice versa in case of a read command. As long as 512 clock cycles have not passed since receiving the command, diskstatus will mark that the disk is busy. After 512 clock cycles, in the same cycle diskcmd and diskstatus will be changed to the value 0 to mark that the disk is now free, and the disk will notify an interrupt by turning on irqstatus1.

## SFS
SFS is a primitive file system, based on the idea of linked lists. It's comprised of items, which can be files or directories, which include a reference to the next item, and to it's parent. 


An item starts with a header, which is 1 sectors, or 64 bytes.
the first 12 bytes are called the item head and store info about the item, like its type (file/directory), it's parent's start sector and it's own end sector. the following 52 bytes are for the name (ascii encoded). 

```
|XX|XX|XXXX|XXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
 ^^ Type (Executable, folder, link...)
    ^^ Reserved for future use
       ^^^^ Parent item's head sector
            ^^^^ End sector
       Item name ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 
 
 
```
### Types:
- 0x00 - Null type (Empty space)
- 0x01 - Directory
- 0x02 - Text / data file
- 0x03 - Executable 
- 0x04 - Link


