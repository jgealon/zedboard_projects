# 📚 SCU Watchdog Timer Documentation - START HERE

Welcome! This document helps you navigate all the SCU Watchdog Timer documentation.

---

## 🎯 Choose Your Path

### 👉 I'm New to SCU Watchdog Timer
**Read in this order:**
1. **README_CORRECTED.md** (5 min) - Overview and file guide
2. **UART_COMPARISON.md** (10 min) - Understand PS UART vs UARTLite
3. **QUICK_REFERENCE.md** (5 min) - Essential info at a glance
4. **HOW_TO_USE_EXAMPLES.md** (30 min) - Step-by-step tutorial
5. **SCU_WDT_CORRECTED_EXAMPLES.md** - Copy working code

### 👉 I Just Want Working Code
**Go directly to:**
- **SCU_WDT_CORRECTED_EXAMPLES.md** - Copy/paste source code
- **QUICK_REFERENCE.md** - Essential code snippets
- **HOW_TO_USE_EXAMPLES.md** - Build and run instructions

### 👉 I Need Quick Reference
**Use:**
- **QUICK_REFERENCE.md** - Code snippets, troubleshooting, checklists

### 👉 I Have a Specific Question
**Find answers:**
- **UART not working?** → UART_COMPARISON.md
- **Build errors?** → HOW_TO_USE_EXAMPLES.md (Troubleshooting)
- **Expected output?** → HOW_TO_USE_EXAMPLES.md (Each example section)
- **Timeout calculation?** → QUICK_REFERENCE.md
- **Understanding concepts?** → SCU_WDT_EXAMPLES.md (ignore UART code)

---

## 📖 Complete Documentation Index

### ⭐ Essential Documents (Start Here)

#### 1. **README_CORRECTED.md**
**Purpose:** Project overview and navigation
**Read Time:** 5 minutes
**When to Use:** First time setup, understanding file structure

**Contains:**
- ✅ Correction notice (PS UART vs UARTLite)
- ✅ Document guide and purpose
- ✅ Quick comparison table
- ✅ Hardware requirements
- ✅ What to do next

---

#### 2. **QUICK_REFERENCE.md** ⭐ BOOKMARK THIS
**Purpose:** Quick lookup for common tasks
**Read Time:** 5 minutes (reference as needed)
**When to Use:** During development, debugging

**Contains:**
- ✅ Hardware checklist
- ✅ Serial terminal settings
- ✅ Example summaries
- ✅ Code snippets (copy-paste ready)
- ✅ Timeout calculations
- ✅ Quick troubleshooting
- ✅ Success criteria checklists

---

#### 3. **HOW_TO_USE_EXAMPLES.md** ⭐ MAIN TUTORIAL
**Purpose:** Complete step-by-step guide
**Read Time:** 30-60 minutes (hands-on)
**When to Use:** Implementing examples, troubleshooting issues

**Contains:**
- ✅ Prerequisites checklist
- ✅ Hardware setup instructions
- ✅ Software setup (Vitis, terminal)
- ✅ **4 Complete example tutorials** with:
  - Implementation steps
  - Build instructions
  - Expected results
  - Verification checklists
- ✅ Detailed troubleshooting (7 common issues)
- ✅ Performance tips

**Example Tutorials:**
1. Simple UART Echo (15 min)
2. Command Parser (20 min)
3. Data Logger (15 min)
4. Reset Detection (20 min)

---

#### 4. **SCU_WDT_CORRECTED_EXAMPLES.md** ⭐ SOURCE CODE
**Purpose:** Working source code for all examples
**Read Time:** Reference as needed
**When to Use:** Copy code into your Vitis project

**Contains:**
- ✅ 4 complete, tested examples
- ✅ Uses PS UART (XUartPs) - CORRECT
- ✅ Detailed code comments
- ✅ API comparison table
- ✅ Key differences from UARTLite

**Examples:**
1. **Example 1:** Simple UART Echo with Watchdog
2. **Example 2:** UART Command Parser with Watchdog
3. **Example 3:** Data Logger with Watchdog
4. **Example 4:** Reset Detection and Recovery

---

### 📚 Background Information

#### 5. **UART_COMPARISON.md**
**Purpose:** Understand UART options in Zynq
**Read Time:** 15 minutes
**When to Use:** Confused about PS UART vs UARTLite, choosing UART

**Contains:**
- ✅ Why UARTLite was wrong
- ✅ Three UART options comparison:
  - PS UART (XUartPs) ✅ Recommended
  - AXI UARTLite (XUartLite)
  - AXI UART 16550
- ✅ Code comparison (wrong vs correct)
- ✅ When to use each type
- ✅ Detailed feature comparison table

**Key Takeaway:**
> Use **PS UART (XUartPs)** for pvt_wdt because it's already in the Processing System, uses no FPGA resources, and has better performance.

---

#### 6. **SCU_WDT_EXAMPLES.md** ⚠️ REFERENCE ONLY
**Purpose:** SCU Watchdog concepts and theory
**Read Time:** 20 minutes
**When to Use:** Understanding watchdog theory, timing calculations

**⚠️ IMPORTANT WARNING:**
- Contains **incorrect UART code** (uses XUartLite)
- **DO NOT copy UART code** from this file
- **DO use** for understanding SCU WDT concepts

**Good For:**
- ✅ What is SCU WDT
- ✅ Use cases
- ✅ How it works (watchdog vs timer mode)
- ✅ API function reference
- ✅ Timing calculations
- ✅ Best practices

**Bad For:**
- ❌ UART code (uses wrong driver)
- ❌ GPIO examples (may not match your hardware)

---

## 🗺️ Usage Scenarios

### Scenario 1: First Time User
```
1. Read README_CORRECTED.md (overview)
   ↓
2. Read UART_COMPARISON.md (understand UART choice)
   ↓
3. Open QUICK_REFERENCE.md (keep open as reference)
   ↓
4. Follow HOW_TO_USE_EXAMPLES.md → Example 1
   ↓
5. Copy code from SCU_WDT_CORRECTED_EXAMPLES.md
   ↓
6. Build, run, verify results
   ↓
7. Repeat for other examples
```

### Scenario 2: Need Quick Example
```
1. Open SCU_WDT_CORRECTED_EXAMPLES.md
   ↓
2. Copy example code
   ↓
3. Check QUICK_REFERENCE.md for setup info
   ↓
4. Build and run
   ↓
5. If issues → HOW_TO_USE_EXAMPLES.md (Troubleshooting)
```

### Scenario 3: Debugging Issue
```
1. Check QUICK_REFERENCE.md (Quick Troubleshooting)
   ↓
2. Still stuck? → HOW_TO_USE_EXAMPLES.md (Detailed Troubleshooting)
   ↓
3. Still stuck? → Add debug messages, check hardware
```

### Scenario 4: Understanding Concepts
```
1. Read SCU_WDT_EXAMPLES.md (theory)
   ↓
2. Read UART_COMPARISON.md (UART details)
   ↓
3. See practical implementation in SCU_WDT_CORRECTED_EXAMPLES.md
```

---

## 📂 File Overview Table

| File | Status | Purpose | Priority |
|------|--------|---------|----------|
| **START_HERE.md** | ⭐ | Navigation guide | Read first |
| **README_CORRECTED.md** | ✅ Correct | Project overview | High |
| **QUICK_REFERENCE.md** | ✅ Correct | Quick lookup | High |
| **HOW_TO_USE_EXAMPLES.md** | ✅ Correct | Tutorial & troubleshooting | High |
| **SCU_WDT_CORRECTED_EXAMPLES.md** | ✅ Correct | Working source code | High |
| **UART_COMPARISON.md** | ✅ Correct | UART options explained | Medium |
| **SCU_WDT_EXAMPLES.md** | ⚠️ Partial | WDT theory (UART code wrong) | Low |

---

## 🎯 Quick Start (5 Minutes)

### Absolute Minimum to Get Started:

1. **Hardware:** Connect Zedboard (JTAG + UART cables)

2. **Terminal:** Open serial terminal (115200 baud, find COM port)

3. **Code:** Copy Example 1 from **SCU_WDT_CORRECTED_EXAMPLES.md**

4. **Build:** Create application in Vitis, paste code, build

5. **Run:** Launch on hardware, see output in terminal

6. **Verify:** Type characters → should echo back

**Done!** If it works, try other examples. If not, see troubleshooting.

---

## ✅ Essential Information Checklist

Before starting, make sure you understand:

**Hardware:**
- ⬜ Zedboard connections (JTAG = J17, UART = J14)
- ⬜ COM port identification
- ⬜ Serial terminal settings (115200, 8-N-1)

**Software:**
- ⬜ PS UART vs UARTLite difference
- ⬜ Use `xuartps.h` NOT `xuartlite.h`
- ⬜ Timeout calculation (333333333 counts = 1 second)
- ⬜ How to service watchdog (`XScuWdt_RestartWdt()`)

**Concepts:**
- ⬜ Watchdog mode vs Timer mode
- ⬜ System reset on timeout
- ⬜ Reset detection with `IsWdtExpired()`

---

## 🔍 Finding Information

### "How do I...?"

| Question | Document | Section |
|----------|----------|---------|
| ...set up hardware? | HOW_TO_USE_EXAMPLES.md | Hardware Setup |
| ...initialize watchdog? | QUICK_REFERENCE.md | Essential Code Snippets |
| ...calculate timeout? | QUICK_REFERENCE.md | Timeout Calculations |
| ...detect reset? | SCU_WDT_CORRECTED_EXAMPLES.md | Example 4 |
| ...send UART? | QUICK_REFERENCE.md | Send/Receive UART |
| ...control LEDs? | SCU_WDT_CORRECTED_EXAMPLES.md | Any example |
| ...log data? | SCU_WDT_CORRECTED_EXAMPLES.md | Example 3 |

### "Why...?"

| Question | Document |
|----------|----------|
| ...use PS UART not UARTLite? | UART_COMPARISON.md |
| ...did system reset? | HOW_TO_USE_EXAMPLES.md (Troubleshooting) |
| ...isn't UART working? | HOW_TO_USE_EXAMPLES.md (Issue 1) |
| ...do I need watchdog? | SCU_WDT_EXAMPLES.md (Use Cases) |

### "What...?"

| Question | Document |
|----------|----------|
| ...is expected output? | HOW_TO_USE_EXAMPLES.md (Each example) |
| ...is SCU WDT? | SCU_WDT_EXAMPLES.md (What is SCU WDT) |
| ...are the differences? | UART_COMPARISON.md (Comparison Table) |

---

## 🚦 Status Legend

- ✅ **Correct** - Use this file, code is accurate
- ⚠️ **Partial** - Some info good, some outdated (check notes)
- ⭐ **Essential** - Priority reading
- 📋 **Reference** - Keep open while working

---

## 💡 Pro Tips

1. **Keep QUICK_REFERENCE.md open** while coding - saves time!

2. **Start with Example 1** even if you want advanced features - validates setup

3. **Always check expected output** in HOW_TO_USE_EXAMPLES.md when testing

4. **Use longer timeouts** (10+ sec) during development, reduce later

5. **Test watchdog timeout** (Example 2, HANG command) to verify it works

6. **Add checkpoint messages** (`xil_printf("Checkpoint X\r\n")`) when debugging

7. **Clean build** after any hardware changes (platform → application)

---

## 📞 Still Need Help?

### Troubleshooting Checklist:
1. ✅ Read QUICK_REFERENCE.md (Quick Troubleshooting section)
2. ✅ Read HOW_TO_USE_EXAMPLES.md (Troubleshooting section)
3. ✅ Verify hardware connections (cables, power, switches)
4. ✅ Check serial terminal settings (115200, correct COM port)
5. ✅ Try Example 1 first (simplest, validates setup)
6. ✅ Add debug messages to find where code stops
7. ✅ Check Console in Vitis for build errors

### Common Solutions:
- **No output:** Wrong COM port or baud rate
- **Immediate reset:** Timeout too short (increase to 10+ sec)
- **Build errors:** Rebuild platform, clean application
- **No echo:** Terminal local echo should be OFF

---

## 🎓 Learning Objectives

After completing all examples, you will:

✅ Understand SCU Watchdog Timer operation
✅ Know how to prevent system hangs
✅ Be able to detect and recover from watchdog resets
✅ Use PS UART for communication
✅ Implement command interfaces
✅ Log sensor data reliably
✅ Control GPIO/LEDs
✅ Debug embedded systems effectively

---

## 📚 Document Cross-Reference

### If you're reading...

**README_CORRECTED.md:**
→ Next: UART_COMPARISON.md (understand UART)
→ Then: HOW_TO_USE_EXAMPLES.md (hands-on)

**UART_COMPARISON.md:**
→ Next: QUICK_REFERENCE.md (code snippets)
→ Then: SCU_WDT_CORRECTED_EXAMPLES.md (full code)

**QUICK_REFERENCE.md:**
→ Next: HOW_TO_USE_EXAMPLES.md (detailed steps)
→ Reference: SCU_WDT_CORRECTED_EXAMPLES.md (copy code)

**HOW_TO_USE_EXAMPLES.md:**
→ Copy code from: SCU_WDT_CORRECTED_EXAMPLES.md
→ Quick lookup: QUICK_REFERENCE.md

**SCU_WDT_CORRECTED_EXAMPLES.md:**
→ Build instructions: HOW_TO_USE_EXAMPLES.md
→ Troubleshooting: HOW_TO_USE_EXAMPLES.md or QUICK_REFERENCE.md

---

## 🎯 Success Metrics

You'll know you're successful when:

1. ✅ Example 1: Characters echo correctly, no resets
2. ✅ Example 2: All commands work, HANG causes reset
3. ✅ Example 3: Data logs continuously at 1 Hz
4. ✅ Example 4: Reset detection works, recovery executes

**Goal:** Understand watchdog timer well enough to integrate into your own application!

---

## 📝 Quick Navigation

- 🏠 [START_HERE.md](#) (you are here)
- ⭐ [QUICK_REFERENCE.md](QUICK_REFERENCE.md) - Essential reference
- 📖 [HOW_TO_USE_EXAMPLES.md](HOW_TO_USE_EXAMPLES.md) - Tutorial
- 💻 [SCU_WDT_CORRECTED_EXAMPLES.md](SCU_WDT_CORRECTED_EXAMPLES.md) - Source code
- 🔌 [UART_COMPARISON.md](UART_COMPARISON.md) - UART guide
- 📋 [README_CORRECTED.md](README_CORRECTED.md) - Overview

---

**Ready to start? → Open HOW_TO_USE_EXAMPLES.md and begin with Example 1!**

*Documentation created: 2026-04-23*
*Project: pvt_wdt (SCU Watchdog Timer with PS UART)*
