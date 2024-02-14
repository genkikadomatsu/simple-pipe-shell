# Development
The source code is split up into three logical parts
- `shell.c` - is the entry point of the shell, and implements top level logic.
- `tokenizer.c` - processes user input, tokenizing user input into commands.
- `pipeline.c` - includes logic related to executing pipelines in the shell.

# Testing
`run_tests.py` is a simple script that automates the testing of test cases which
are included in `tests/`.