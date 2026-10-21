# Unix Command Interpreter (Spireslayer)

A C-based shell interpreter with a custom recursive descent parser and backtracking support. Developed for the **CMPE 230 (Systems Programming)** course at **Boğaziçi University**.

---

## Tech Stack

- **Language:** C11
- **Build System:** GNU Make
- **Toolchain:** GCC with `-std=c11 -Wall -Wextra -pedantic`
- **CI:** GitHub Actions (automated grading pipeline)

---

## Features

- Custom recursive descent parser for command interpretation
- Backtracking support for complex command sequences
- Modular architecture separating parsing, state management, and execution
- Strict C11 compliance with comprehensive compiler warnings enabled
- Automated test suite with input/output validation

---

## Project Structure

```
├── main.c              # Entry point and main execution loop
├── parser.c / parser.h # Recursive descent parser implementation
├── state.c / state.h   # Game state management
├── spireslayer.h       # Shared type definitions and constants
├── Makefile            # Build configuration
├── report.pdf          # Technical design report
├── test-cases/         # Input/output test pairs
│   ├── input01.txt ... input07.txt
│   └── output01.txt ... output07.txt
└── test/               # Automated grading scripts
    ├── checker.py
    └── grader.py
```

---

## Build and Run

### Prerequisites
- GCC (with C11 support)
- GNU Make

### Compile
```bash
git clone https://github.com/yigitsarpavci/unix-command-interpreter.git
cd unix-command-interpreter
make
```

### Run
```bash
./spireslayer < test-cases/input01.txt
```

---

## Testing

Run the automated grader against all test cases:

```bash
python3 test/grader.py ./spireslayer test-cases
```

Individual test verification:
```bash
./spireslayer < test-cases/input01.txt > my_output.txt
diff my_output.txt test-cases/output01.txt
```

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
