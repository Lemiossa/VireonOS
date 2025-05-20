# VireonOS

## About the project
**VireonOS** is a **real mode(16-bit)** operating system made entirely from scratch, using **C and Assembly**. I created it out of pure curiosity, without even knowing how to use pointers in C when i started, lol. It was also a lot of fun to learn the hard way

## Why did i do this?
I started VireonOS without any ral experience with operating systems or low-level programming. I just wanted to understand how everything worked behing the scenes of what we use on daily basis. And man... I struggled a lot. But now that i've managed to do some basic things, i decided to share it so that all this effort woldn't go to waste.

## What doews it do?
So far, VireonOS already has s few things working:

- **Basic file system (FAT16) support** - this part was a real headache
- **Basic Timer**
- **Basic text management on the screen(VGA)**

## Dependencies
- nasm
- bcc, ld86, ar86... only the **dev86**
- QEMU
- Make
- mtools
- dosfstools

## How to test?
You can test using:

```bash
make
make run
```

# How to install
It doesn't have an installer yet, but you can run it on a USB stick

You can install **on a USB stick** using:
```bash
make
make install DEVICE=/dev/sdb
```
replace '/dev/sdb' with your USB stick/disk

I apologize for th poor organization, especially regarding commits. I was also initially going to name it BitOS, but i discovered that there is already a project with that name
