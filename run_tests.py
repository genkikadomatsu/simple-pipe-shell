import os

def run_valgrind(file_path, filename):
    command = f"valgrind ./shell < {file_path} > /dev/null 2>&1"
    exit_code = os.system(command)

    if exit_code == 0:
        print(f"SUCCESS {filename}: No memory leaks or errors found.")
    else:
        print(f"FAILED {filename}: Memory leaks or errors detected.")

def main():
    tests_folder = "tests"
    if not os.path.exists(tests_folder):
        print(f"Folder '{tests_folder}' not found.")
        return

    print("Running tests all will fail if valgrind isn't available (e.g. on M1)...")
    for filename in os.listdir(tests_folder):
        file_path = os.path.join(tests_folder, filename)
        if os.path.isfile(file_path):
            run_valgrind(file_path, filename)

if __name__ == "__main__":
    main()
