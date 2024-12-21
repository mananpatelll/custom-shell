# Custom Shell

A lightweight and custom shell written in C, designed to execute commands with support for piping, input/output redirection, and directory navigation.

## Features

- **Command Parsing:**
  - Splits user input into commands and arguments.
- **Redirection Support:**
  - Input redirection (`<`) and output redirection (`>`).
- **Pipe Handling:**
  - Executes piped commands (e.g., `ls | grep txt`).
- **Built-in Commands:**
  - `cd` for directory navigation.
  - `exit` to quit the shell.
- **Custom Prompt:**
  - Displays the current working directory dynamically.
- **Subprocess Execution:**
  - Forks and executes standard Linux commands.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/mananpatelll/custom-shell
   cd myshell
   ```

2. Compile the program:
   ```bash
   gcc myshell.c -o myshell
   ```

3. Run the shell:
   ```bash
   ./myshell
   ```

## Usage

### Starting the Shell
Launch the shell by running `./myshell`. The prompt will display the current working directory.

### Commands
- **Standard Commands:**
  Enter commands like `ls`, `pwd`, `echo`, etc., as you would in any shell.
  
- **Piping:**
  Use the pipe symbol (`|`) to combine commands.
  ```bash
  ls | grep txt
  ```

- **Redirection:**
  Redirect input or output using `<` or `>`.
  ```bash
  cat < input.txt
  echo "Hello, World!" > output.txt
  ```

- **Built-in Commands:**
  - `cd <directory>`: Change the current directory.
  - `exit`: Exit the shell.

### Examples
```bash
mananshell:~> ls
mananshell:~> ls | grep txt
mananshell:~> echo "Hello World" > hello.txt
mananshell:~> cat < hello.txt
Hello World
mananshell:~> cd /path/to/directory
```

## Contributing

Feel free to submit issues or pull requests to enhance this shell. Contributions are welcome!

## License

This project is licensed under the [MIT License](LICENSE).
