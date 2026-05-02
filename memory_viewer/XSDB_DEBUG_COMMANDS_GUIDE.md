# XSDB Debug Commands Guide for Vitis 2023.2

Complete reference for using XSDB (Xilinx System Debugger) console during debug sessions in Vitis Unified IDE.

## Table of Contents
1. [Accessing XSDB Console](#accessing-xsdb-console)
2. [Memory Read Commands](#memory-read-commands)
3. [Memory Write Commands](#memory-write-commands)
4. [Register Operations](#register-operations)
5. [Variable Inspection](#variable-inspection)
6. [Breakpoint Management](#breakpoint-management)
7. [Target Control](#target-control)
8. [Data Logging](#data-logging)
9. [Practical Examples](#practical-examples)
10. [Quick Reference](#quick-reference)

---

## Accessing XSDB Console

### During Active Debug Session
1. Start debugging your application (F5 or Debug button)
2. Look for the **Console** tab at the bottom of Vitis IDE
3. You should see `xsdb%` prompt
4. Type commands directly at this prompt

### If Console Not Visible
- Go to **Window → Show View → Console**
- Switch to the XSDB console tab if multiple consoles are open

---

## Memory Read Commands

### Basic Memory Read (mrd)

#### Read Memory as 32-bit Words (Default)
```tcl
mrd 0x40000000
# Reads 1 word (4 bytes) from address 0x40000000
```

#### Read Multiple Words
```tcl
mrd 0x40000000 10
# Reads 10 words (40 bytes) starting from 0x40000000
```

#### Read with Size Specifier
```tcl
# Read bytes (-size b)
mrd -size b 0x40000000 20
# Reads 20 bytes

# Read half-words/16-bit (-size h)
mrd -size h 0x40000000 10
# Reads 10 half-words (20 bytes)

# Read words/32-bit (-size w) - default
mrd -size w 0x40000000 10
# Reads 10 words (40 bytes)

# Read double-words/64-bit (-size d)
mrd -size d 0x40000000 5
# Reads 5 double-words (40 bytes)
```

#### Read and Display Only Values
```tcl
mrd -value 0x40000000 50
# Displays only the data values, no addresses
# Useful for copying data or piping to files
```

#### Read with Binary Format
```tcl
mrd -bin 0x40000000 10
# Displays values in binary format
```

#### Read to Variable
```tcl
set mydata [mrd -value 0x40000000 10]
puts $mydata
# Stores memory contents in Tcl variable for processing
```

---

## Memory Write Commands

### Basic Memory Write (mwr)

#### Write Single Word (32-bit)
```tcl
mwr 0x40000000 0x12345678
# Writes value 0x12345678 to address 0x40000000
```

#### Write Multiple Sequential Values
```tcl
mwr 0x40000000 0x00000000 0x00000005 0x0000000A 0x0000000F
# Writes multiple values to consecutive addresses
```

#### Write with Size Specifier
```tcl
# Write byte
mwr -size b 0x40000000 0xFF
# Writes 1 byte

# Write half-word (16-bit)
mwr -size h 0x40000000 0x1234
# Writes 2 bytes

# Write word (32-bit) - default
mwr -size w 0x40000000 0x12345678
# Writes 4 bytes

# Write double-word (64-bit)
mwr -size d 0x40000000 0x123456789ABCDEF0
# Writes 8 bytes
```

#### Fill Memory Block
```tcl
# Fill 100 words with value 0
for {set i 0} {$i < 100} {incr i} {
    mwr [expr 0x40000000 + $i*4] 0
}
```

---

## Register Operations

### Read Registers (rrd)

#### Read All General Purpose Registers
```tcl
rrd
# Displays all CPU registers (r0-r15, PC, CPSR, etc.)
```

#### Read Specific Register
```tcl
rrd r0
# Reads register r0

rrd pc
# Reads program counter

rrd sp
# Reads stack pointer

rrd lr
# Reads link register
```

#### Read Multiple Registers
```tcl
rrd r0 r1 r2 r3
# Reads r0, r1, r2, and r3
```

### Write Registers (rwr)

#### Write to Register
```tcl
rwr r0 0x12345678
# Sets r0 to 0x12345678

rwr pc 0x00100000
# Sets program counter (use with caution!)
```

---

## Variable Inspection

### Print Variable Address
```tcl
print &myVariable
# Shows address of variable in your code
```

### Print Variable Value
```tcl
print myVariable
# Shows current value of variable
```

### Print Array Contents
```tcl
print myArray[0]
print myArray[5]
# Access specific array elements
```

### Read Variable from Memory
If you know the variable address:
```tcl
# If variable is at 0x00101234
mrd 0x00101234
```

---

## Breakpoint Management

### Set Breakpoints (bpadd)

#### Set Breakpoint at Address
```tcl
bpadd 0x00100500
# Sets breakpoint at address 0x00100500
```

#### Set Breakpoint at Function
```tcl
bpadd -addr &main
# Sets breakpoint at main function

bpadd -addr &bram_init
# Sets breakpoint at bram_init function
```

#### Set Breakpoint at File:Line
```tcl
bpadd -file helloworld.c -line 24
# Sets breakpoint at line 24 in helloworld.c
```

#### Set Hardware Breakpoint
```tcl
bpadd -type hw 0x00100500
# Uses hardware breakpoint (limited quantity)
```

### List Breakpoints (bplist)
```tcl
bplist
# Shows all active breakpoints with IDs
```

### Remove Breakpoints (bpremove)

#### Remove Specific Breakpoint
```tcl
bpremove 0
# Removes breakpoint with ID 0 (use bplist to see IDs)
```

#### Remove All Breakpoints
```tcl
bpremove -all
# Removes all breakpoints
```

### Enable/Disable Breakpoints
```tcl
bpenable 0
# Enables breakpoint ID 0

bpdisable 0
# Disables breakpoint ID 0 (doesn't delete it)
```

---

## Target Control

### Continue Execution (con)
```tcl
con
# Continues execution until next breakpoint or program end
```

### Step Commands

#### Step Over (nxt)
```tcl
nxt
# Steps over function calls (executes but doesn't enter)
```

#### Step Into (stp)
```tcl
stp
# Steps into function calls
```

#### Step Out (stpi)
```tcl
stpi
# Steps out of current function
```

### Stop Execution (stop)
```tcl
stop
# Pauses/halts the running program
```

### Reset Target
```tcl
rst
# Resets the processor
```

### Get Current State
```tcl
state
# Shows current processor state (running, stopped, etc.)
```

---

## Data Logging

### Log Memory Contents to File

#### Method 1: Redirect Output to File
```tcl
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/data.txt" "w"]
puts $logfile [mrd -value 0x40000000 50]
close $logfile
puts "Data logged to file"
```

#### Method 2: Continuous Logging
```tcl
# Open file for writing
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/data.txt" "w"]

# Write header
puts $logfile "Memory Dump - [clock format [clock seconds]]"
puts $logfile "Address: 0x40000000"
puts $logfile "Length: 50 words"
puts $logfile "================================"

# Write memory contents
set memdata [mrd -value 0x40000000 50]
puts $logfile $memdata

# Close file
close $logfile
puts "Log complete"
```

#### Method 3: Append Mode
```tcl
# Open in append mode to add to existing file
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/data.txt" "a"]
puts $logfile "\n--- New Capture [clock format [clock seconds]] ---"
puts $logfile [mrd -value 0x40000000 50]
close $logfile
```

#### Method 4: Formatted Output
```tcl
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/data.txt" "w"]
puts $logfile "Index\tAddress\t\tValue"
puts $logfile "======================================"

for {set i 0} {$i < 50} {incr i} {
    set addr [format "0x%08X" [expr 0x40000000 + $i*4]]
    set value [mrd -value $addr 1]
    puts $logfile "$i\t$addr\t$value"
}

close $logfile
puts "Formatted log complete"
```

#### Method 5: CSV Format for Excel
```tcl
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/data.csv" "w"]
puts $logfile "Index,Address,Decimal,Hexadecimal"

for {set i 0} {$i < 50} {incr i} {
    set addr [expr 0x40000000 + $i*4]
    set value [mrd -value $addr 1]
    set decimal [format "%d" $value]
    set hex [format "0x%08X" $value]
    puts $logfile "$i,0x[format %08X $addr],$decimal,$hex"
}

close $logfile
puts "CSV log created - open in Excel"
```

### Log Registers to File
```tcl
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/registers.txt" "w"]
puts $logfile "Register Dump - [clock format [clock seconds]]"
puts $logfile "================================"
puts $logfile [rrd]
close $logfile
puts "Registers logged"
```

### Live Monitoring Script
```tcl
# Monitor memory location every second
proc monitor_memory {addr count interval} {
    set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/monitor.txt" "w"]
    puts $logfile "Memory Monitor - Starting [clock format [clock seconds]]"
    puts $logfile "Address: $addr, Count: $count, Interval: ${interval}s"
    puts $logfile "================================\n"

    for {set i 0} {$i < 10} {incr i} {
        puts $logfile "Sample $i - [clock format [clock seconds]]"
        puts $logfile [mrd $addr $count]
        puts $logfile ""
        after [expr $interval * 1000]
    }

    close $logfile
    puts "Monitoring complete - 10 samples captured"
}

# Usage: monitor_memory 0x40000000 10 1
# Captures 10 words every 1 second, 10 times
```

---

## Practical Examples

### Example 1: Verify BRAM Write Operation
```tcl
# Your code writes to BRAM starting at 0x40000000
# Verify the first 10 values written

puts "Reading BRAM contents..."
mrd 0x40000000 10

# Expected output:
# 0x40000000: 0x00000000  (0*5)
# 0x40000004: 0x00000005  (1*5)
# 0x40000008: 0x0000000A  (2*5)
# etc.
```

### Example 2: Check If Loop Completed
```tcl
# Your code writes 50 values
# Check last value to confirm completion

puts "Checking last value written..."
mrd 0x400000C4 1
# Address 0x400000C4 = 0x40000000 + (49*4)
# Should contain 0x000000F5 (49*5 = 245 = 0xF5)
```

### Example 3: Compare Expected vs Actual
```tcl
puts "Verifying BRAM contents..."
set errors 0

for {set i 0} {$i < 50} {incr i} {
    set addr [expr 0x40000000 + $i*4]
    set actual [mrd -value $addr 1]
    set expected [expr $i * 5]

    if {$actual != $expected} {
        puts "ERROR at index $i: expected $expected, got $actual"
        incr errors
    }
}

if {$errors == 0} {
    puts "SUCCESS: All 50 values correct!"
} else {
    puts "FAILED: $errors errors found"
}
```

### Example 4: Find Non-Zero Memory Regions
```tcl
puts "Scanning for written data..."

for {set i 0} {$i < 100} {incr i} {
    set addr [expr 0x40000000 + $i*4]
    set value [mrd -value $addr 1]

    if {$value != 0} {
        puts "Data found at offset $i: [format 0x%08X $value]"
    }
}
```

### Example 5: Monitor Stack Pointer
```tcl
# Check stack usage during execution
puts "Stack Pointer: [rrd sp]"
con
# Run some code...
stop
puts "Stack Pointer after: [rrd sp]"
```

### Example 6: Dump Memory Range to File
```tcl
# Save entire BRAM contents for analysis
proc dump_bram {filename} {
    set logfile [open $filename "w"]

    puts $logfile "BRAM Memory Dump"
    puts $logfile "Base Address: 0x40000000"
    puts $logfile "Size: 8KB (2048 words)"
    puts $logfile "Timestamp: [clock format [clock seconds]]"
    puts $logfile "================================\n"

    # BRAM is 8KB = 2048 words of 32-bit
    set data [mrd 0x40000000 2048]
    puts $logfile $data

    close $logfile
    puts "BRAM dumped to $filename"
}

# Usage:
dump_bram "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/bram_dump.txt"
```

### Example 7: Reset and Verify BRAM
```tcl
# Clear BRAM and verify
puts "Clearing BRAM..."

for {set i 0} {$i < 50} {incr i} {
    mwr [expr 0x40000000 + $i*4] 0
}

puts "Verification..."
set data [mrd -value 0x40000000 50]

if {[regexp {[1-9a-fA-F]} $data]} {
    puts "ERROR: BRAM not fully cleared"
} else {
    puts "SUCCESS: BRAM cleared"
}
```

### Example 8: Capture Data Before and After Function
```tcl
# Set breakpoint before function
bpadd -addr &bram_init

# Wait for breakpoint
con

# Capture initial state
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/comparison.txt" "w"]
puts $logfile "=== BEFORE bram_init ==="
puts $logfile [mrd 0x40000000 10]

# Step through function
nxt

# Capture final state
puts $logfile "\n=== AFTER bram_init ==="
puts $logfile [mrd 0x40000000 10]
close $logfile

puts "Comparison saved"
```

---

## Quick Reference

### Most Useful Commands for Your Project

```tcl
# Read BRAM contents (first 50 values)
mrd 0x40000000 50

# Read BRAM values only (no addresses)
mrd -value 0x40000000 50

# Save BRAM to file
set logfile [open "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/data.txt" "w"]
puts $logfile [mrd -value 0x40000000 50]
close $logfile

# Write test pattern to BRAM
mwr 0x40000000 0x12345678

# Check specific location
mrd 0x400000C4 1

# Continue execution
con

# Stop execution
stop

# Step over
nxt

# View all registers
rrd

# Set breakpoint at main
bpadd -addr &main

# List breakpoints
bplist

# Remove all breakpoints
bpremove -all
```

---

## Command Syntax Summary

| Command | Syntax | Description |
|---------|--------|-------------|
| `mrd` | `mrd [-size b\|h\|w\|d] [-value] [-bin] <addr> [count]` | Memory read |
| `mwr` | `mwr [-size b\|h\|w\|d] <addr> <value> [value...]` | Memory write |
| `rrd` | `rrd [register...]` | Read registers |
| `rwr` | `rwr <register> <value>` | Write register |
| `con` | `con` | Continue execution |
| `stop` | `stop` | Stop execution |
| `stp` | `stp [count]` | Step into |
| `nxt` | `nxt [count]` | Step over |
| `bpadd` | `bpadd [-addr <addr>] [-file <file> -line <line>]` | Add breakpoint |
| `bplist` | `bplist` | List breakpoints |
| `bpremove` | `bpremove <id> \| -all` | Remove breakpoint |
| `state` | `state` | Show target state |
| `rst` | `rst` | Reset processor |

---

## Tips and Tricks

### 1. Command History
- Use **Up Arrow** to recall previous commands
- Use **Down Arrow** to navigate forward in history

### 2. Tab Completion
- Type partial command and press **Tab** for completion
- Works for commands, not addresses

### 3. Hexadecimal vs Decimal
```tcl
# Hexadecimal (with 0x prefix)
mrd 0x40000000

# Decimal (no prefix)
mrd 1073741824

# Expression evaluation
mrd [expr 0x40000000 + 0x100]
```

### 4. Save Commonly Used Commands
Create a Tcl script file with your frequently used commands:

**File: memory_viewer_debug.tcl**
```tcl
# Quick commands for memory_viewer debugging

proc check_bram {} {
    puts "=== BRAM Contents ==="
    mrd 0x40000000 50
}

proc save_bram {} {
    set filename "C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/bram_[clock seconds].txt"
    set logfile [open $filename "w"]
    puts $logfile [mrd 0x40000000 50]
    close $logfile
    puts "Saved to $filename"
}

proc verify_bram {} {
    puts "Verifying BRAM pattern..."
    set errors 0
    for {set i 0} {$i < 50} {incr i} {
        set addr [expr 0x40000000 + $i*4]
        set actual [mrd -value $addr 1]
        set expected [expr $i * 5]
        if {$actual != $expected} {
            puts "ERROR at index $i"
            incr errors
        }
    }
    puts "Verification complete: $errors errors"
}

puts "Custom commands loaded:"
puts "  check_bram  - Display BRAM contents"
puts "  save_bram   - Save BRAM to timestamped file"
puts "  verify_bram - Verify expected pattern"
```

**Load script in XSDB:**
```tcl
source C:/Users/JGEALON/Documents/ZedProjects/memory_viewer/memory_viewer_debug.tcl
check_bram
save_bram
verify_bram
```

### 5. Watch Memory Location Change
```tcl
# Take snapshot
set before [mrd -value 0x40000000 1]
puts "Before: $before"

# Continue execution
con

# Check again
stop
set after [mrd -value 0x40000000 1]
puts "After: $after"

if {$before != $after} {
    puts "Memory changed: $before -> $after"
}
```

### 6. Calculate Addresses
```tcl
# Base address + offset
set base 0x40000000
set offset 49
set addr [expr $base + $offset * 4]
mrd $addr 1
```

---

## Common Errors and Solutions

### Error: "Memory read error"
- **Cause**: Invalid address or target not running
- **Solution**: Verify address is within valid range (0x40000000-0x40001FFF for 8KB BRAM)

### Error: "invalid command name"
- **Cause**: Not in active debug session
- **Solution**: Start debugging first (F5), then try command

### Error: "can't read logfile: no such variable"
- **Cause**: File not opened or already closed
- **Solution**: Check `set logfile [open ...]` was executed

### Commands Do Nothing
- **Cause**: Target might be running
- **Solution**: Execute `stop` first, then retry command

---

## Performance Notes

- Reading large memory regions (>1000 words) can be slow over JTAG
- Use `-value` flag when you only need data, not addresses (faster output)
- Hardware breakpoints are limited (typically 2-4 on Cortex-A9)
- Software breakpoints unlimited but require RAM

---

## Related Documentation

- XSDB User Guide: `<Vitis_Install>/doc/xsdb/xsdb_intro.html`
- Vitis Unified Software Platform Documentation (UG1400)
- Zynq-7000 Technical Reference Manual (UG585)

---

**Document Version**: 1.0
**Last Updated**: 2026-05-02
**Vitis Version**: 2023.2 Unified IDE
**Project**: memory_viewer (ZedBoard Zynq-7000)
