# NEO OS

<p align="center">
  <img src="https://img.shields.io/badge/Architecture-x86-blue" alt="Architecture">
  <img src="https://img.shields.io/badge/Mode-32bit-green" alt="32-bit">
  <img src="https://img.shields.io/badge/Language-C%2FAssembly-orange" alt="Language">
</p>

[English](#english) | [中文](#chinese)

---

<a name="english"></a>
## English

### Overview

NEO OS is a **32-bit operating system** developed from scratch, featuring a graphical user interface (GUI), multitasking support, and a complete file system. This project demonstrates the implementation of core OS components including bootloader, kernel, memory management, interrupt handling, device drivers, and a windowing system.

### Features

#### Core System
- **Bootloader**: Custom 16-bit real mode bootloader with A20 line enable, GDT setup, and transition to 32-bit protected mode
- **Kernel**: 32-bit monolithic kernel written in C
- **Memory Management**: 
  - Physical page allocator (4KB pages)
  - Kernel heap allocator (kmalloc/kfree)
- **Interrupt System**: IDT (Interrupt Descriptor Table) with PIC (Programmable Interrupt Controller) configuration
- **System Calls**: Basic syscall interface (exit, read, write)

#### Device Drivers
- **Keyboard**: PS/2 keyboard driver with scancode handling and input buffering
- **Mouse**: PS/2 mouse driver with position tracking and button state
- **Video**: VGA graphics driver supporting 320x200 8-bit color mode with double buffering

#### File System
- **FAT12**: Full FAT12 file system implementation
- **VFS**: Virtual File System layer for abstraction
- **Block Device**: Low-level block device interface for disk I/O

#### GUI System
- **Desktop**: Graphical desktop environment with background rendering
- **Window Manager**: 
  - Up to 16 windows support
  - Window dragging and z-order management
  - Title bars with close buttons
  - Focus management
- **Graphics Shell (GShell)**: Terminal emulator running in a window

#### Multitasking
- **Task Scheduler**: Cooperative multitasking with up to 8 tasks
- **Context Switching**: Basic task switching with stack management
- **ELF Loader**: Executable and Linkable Format support for loading programs

#### User Interface
- **Shell Commands**:
  - `help` - Display available commands
  - `clear` - Clear the screen
  - `echo` - Print text
  - `about` - Display OS information
  - `meminfo` - Show memory statistics
  - `time` - Display system tick count
  - `ls` - List files in directory
  - `cat` - Display file contents

### Architecture

```
┌─────────────────────────────────────────┐
│           User Applications             │
├─────────────────────────────────────────┤
│  GUI Shell │  Desktop │  Window Manager │
├─────────────────────────────────────────┤
│        System Call Interface            │
├─────────────────────────────────────────┤
│  Task │  VFS  │  FAT12 │  ELF Loader   │
├─────────────────────────────────────────┤
│  Keyboard │  Mouse │  Video │  Screen   │
├─────────────────────────────────────────┤
│  Memory │  Heap │  IDT │  PIC │  IRQ    │
├─────────────────────────────────────────┤
│              Kernel Core                │
├─────────────────────────────────────────┤
│           Bootloader (ASM)              │
└─────────────────────────────────────────┘
```

### Project Structure

```
NEO-OS/
├── boot.asm          # 16-bit bootloader
├── entry.asm         # 32-bit kernel entry point
├── isr.asm           # Interrupt service routines
├── linker.ld         # Linker script
├── kernel.c          # Main kernel initialization
├── types.h           # Type definitions
│
├── Memory Management
│   ├── memory.c/h    # Physical page allocator
│   └── heap.c/h      # Kernel heap allocator
│
├── Interrupt System
│   ├── idt.c/h       # Interrupt descriptor table
│   ├── irq.c/h       # IRQ handling
│   └── pic.c/h       # PIC configuration
│
├── Device Drivers
│   ├── keyboard.c/h  # Keyboard driver
│   ├── mouse.c/h     # Mouse driver
│   ├── video.c/h     # VGA graphics driver
│   └── screen.c/h    # Text mode screen driver
│
├── File System
│   ├── vfs.c/h       # Virtual file system
│   ├── fat12.c/h     # FAT12 implementation
│   └── block.c/h     # Block device interface
│
├── Multitasking
│   ├── task.c/h      # Task scheduler
│   └── elf.c/h       # ELF loader
│
├── GUI System
│   ├── desktop.c/h   # Desktop environment
│   ├── window.c/h    # Window manager
│   ├── gshell.c/h    # Graphical shell
│   └── font.c/h      # Bitmap fonts
│
├── Shell
│   ├── shell.c/h     # Command-line shell
│   └── string.c/h    # String utilities
│
└── System Calls
    └── syscall.c/h   # Syscall implementation
```

### Building

#### Prerequisites
- NASM (Netwide Assembler)
- GCC (cross-compiler for i686-elf recommended)
- LD (GNU linker)
- QEMU (for testing)

#### Build on Linux/macOS
```bash
# Compile the OS
bash build.sh

# Run in QEMU
bash run.sh
```

#### Build on Windows
```powershell
# Using PowerShell
.\build.ps1

# Run in QEMU
.\run.bat
```

### Technical Details

- **Architecture**: x86 (i386)
- **Mode**: 32-bit Protected Mode
- **Boot**: Custom bootloader from MBR
- **Memory Layout**: Kernel loaded at 0x100000 (1MB)
- **Graphics**: VGA Mode 0x13 (320x200, 256 colors)
- **File System**: FAT12 on floppy image

### Screenshots

*The OS features a graphical desktop with draggable windows and a command shell.*

---

<a name="chinese"></a>
## 中文

### 概述

NEO OS 是一个从零开始开发的 **32位操作系统**，具有图形用户界面（GUI）、多任务支持和完整的文件系统。本项目展示了核心操作系统组件的实现，包括引导加载程序、内核、内存管理、中断处理、设备驱动程序和窗口系统。

### 功能特性

#### 核心系统
- **引导加载程序**: 自定义16位实模式引导程序，启用A20地址线，设置GDT，并切换到32位保护模式
- **内核**: 使用C语言编写的32位单体内核
- **内存管理**:
  - 物理页面分配器（4KB页面）
  - 内核堆分配器（kmalloc/kfree）
- **中断系统**: IDT（中断描述符表）配合PIC（可编程中断控制器）配置
- **系统调用**: 基本系统调用接口（退出、读取、写入）

#### 设备驱动
- **键盘**: PS/2键盘驱动，支持扫描码处理和输入缓冲
- **鼠标**: PS/2鼠标驱动，支持位置跟踪和按钮状态检测
- **视频**: VGA图形驱动，支持320x200 8位色模式，带双缓冲

#### 文件系统
- **FAT12**: 完整的FAT12文件系统实现
- **VFS**: 虚拟文件系统层，提供抽象接口
- **块设备**: 用于磁盘I/O的底层块设备接口

#### GUI系统
- **桌面**: 图形化桌面环境，支持背景渲染
- **窗口管理器**:
  - 最多支持16个窗口
  - 窗口拖动和Z轴层级管理
  - 带有关闭按钮的标题栏
  - 焦点管理
- **图形终端 (GShell)**: 在窗口中运行的终端模拟器

#### 多任务
- **任务调度器**: 协作式多任务，最多8个任务
- **上下文切换**: 基于栈管理的基本任务切换
- **ELF加载器**: 支持可执行可链接格式（ELF）的程序加载

#### 用户界面
- **Shell命令**:
  - `help` - 显示可用命令
  - `clear` - 清屏
  - `echo` - 打印文本
  - `about` - 显示系统信息
  - `meminfo` - 显示内存统计
  - `time` - 显示系统时钟计数
  - `ls` - 列出目录文件
  - `cat` - 显示文件内容

### 系统架构

```
┌─────────────────────────────────────────┐
│              用户应用程序                │
├─────────────────────────────────────────┤
│  图形终端  │   桌面   │    窗口管理器    │
├─────────────────────────────────────────┤
│              系统调用接口                │
├─────────────────────────────────────────┤
│   任务   │  VFS  │  FAT12 │  ELF加载器  │
├─────────────────────────────────────────┤
│   键盘   │  鼠标  │  视频  │   屏幕显示   │
├─────────────────────────────────────────┤
│  内存管理 │  堆   │ IDT │ PIC │  IRQ    │
├─────────────────────────────────────────┤
│                内核核心                  │
├─────────────────────────────────────────┤
│            引导加载程序 (ASM)            │
└─────────────────────────────────────────┘
```

### 项目结构

```
NEO-OS/
├── boot.asm          # 16位引导加载程序
├── entry.asm         # 32位内核入口点
├── isr.asm           # 中断服务例程
├── linker.ld         # 链接器脚本
├── kernel.c          # 内核主初始化
├── types.h           # 类型定义
│
├── 内存管理
│   ├── memory.c/h    # 物理页面分配器
│   └── heap.c/h      # 内核堆分配器
│
├── 中断系统
│   ├── idt.c/h       # 中断描述符表
│   ├── irq.c/h       # IRQ处理
│   └── pic.c/h       # PIC配置
│
├── 设备驱动
│   ├── keyboard.c/h  # 键盘驱动
│   ├── mouse.c/h     # 鼠标驱动
│   ├── video.c/h     # VGA图形驱动
│   └── screen.c/h    # 文本模式屏幕驱动
│
├── 文件系统
│   ├── vfs.c/h       # 虚拟文件系统
│   ├── fat12.c/h     # FAT12实现
│   └── block.c/h     # 块设备接口
│
├── 多任务
│   ├── task.c/h      # 任务调度器
│   └── elf.c/h       # ELF加载器
│
├── GUI系统
│   ├── desktop.c/h   # 桌面环境
│   ├── window.c/h    # 窗口管理器
│   ├── gshell.c/h    # 图形终端
│   └── font.c/h      # 位图字体
│
├── Shell
│   ├── shell.c/h     # 命令行Shell
│   └── string.c/h    # 字符串工具
│
└── 系统调用
    └── syscall.c/h   # 系统调用实现
```

### 编译构建

#### 前置要求
- NASM (Netwide Assembler)
- GCC (建议使用i686-elf交叉编译器)
- LD (GNU链接器)
- QEMU (用于测试)

#### Linux/macOS 构建
```bash
# 编译操作系统
bash build.sh

# 在QEMU中运行
bash run.sh
```

#### Windows 构建
```powershell
# 使用PowerShell
.\build.ps1

# 在QEMU中运行
.\run.bat
```

### 技术细节

- **架构**: x86 (i386)
- **模式**: 32位保护模式
- **启动**: 从MBR的自定义引导加载程序
- **内存布局**: 内核加载于0x100000 (1MB)
- **图形**: VGA模式0x13 (320x200, 256色)
- **文件系统**: 软盘镜像上的FAT12

### 许可证

本项目仅供学习和教育目的。

---

<p align="center">
  <b>NEO OS - 从零开始构建操作系统</b><br>
  <b>NEO OS - Building an OS from Scratch</b>
</p>
