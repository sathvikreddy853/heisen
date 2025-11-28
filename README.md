# Heisen: Programming Beyond Classical Limits
A quantum programming language compiler.

## Prerequisites

- CMake 3.22.1 or higher
- C++17 compatible compiler (clang++ or g++)
- Flex (lexer generator)
- Bison (parser generator)
- LLVM (optional, required for code generation)

## Building with CMake

> **Current Recommendation**: For now, please build with `CODEGEN=OFF` and `BUILD_TESTS=OFF` as the code generation feature is still under development.
> ```shell
> cmake -B build -DCODEGEN=OFF -DBUILD_TESTS=OFF
> cmake --build build
> ```

### 1. Configure the Build

Create and configure the build directory:

```shell
cmake -B build
```

#### Build Options

- **CODEGEN** (default: ON): Enable LLVM-based code generation
  ```shell
  cmake -B build -DCODEGEN=OFF  # Semantic analysis only
  cmake -B build -DCODEGEN=ON   # With code generation
  ```

- **BUILD_TESTS** (default: OFF): Build unit tests
  ```shell
  cmake -B build -DBUILD_TESTS=ON
  ```

### 2. Build the Compiler

```shell
cmake --build build
```

This generates the `heisen` executable in the `build/` directory.

### 3. Run the Compiler

#### Using CMake

```shell
cmake --build build --target run < examples/testcase.hsn
```

#### Direct Execution

```shell
./build/heisen [options] <input-file>
```

Or with stdin:
```shell
./build/heisen [options] < examples/testcase.hsn
```

## Compiler Options
- `--dump-ast`: Dump the AST after parsing
- `--dump-symtab`: Dump the symbol table after. semantic analysis
- `-s, --stats`: Print AST statistics
- `-o, --output <file>`: Specify output file
- `-h, --help`: Show help message

### Examples

Generate QIR output:
```shell
./build/heisen -o output.ll examples/testcase.hsn
```

Dump AST with statistics:
```shell
./build/heisen --dump-ast -s examples/testcase.hsn
```

Dump symbol table:
```shell
./build/heisen --dump-symtab examples/testcase.hsn
```

## Building with Make (Legacy)

> **Note**: The Makefile provides semantic analysis only. For full code generation, use CMake.

Build the compiler:
```shell
make
```

Run the compiler (semantic analysis only):
```shell
make run < examples/testcase.hsn
```

Clean build artifacts:
```shell
make clean
```

## Installation
Install the compiler to your system:

```shell
cmake --install build --prefix /usr/local
```

This installs the `heisen` binary to `/usr/local/bin`.

## Testing
Build and run unit tests:

```shell
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

## Project Structure

- `src/` - Source files for lexer, parser, semantic analysis, and code generation
- `include/` - Header files
- `examples/` - Example Heisen programs
- `test/` - Test cases
- `unittests/` - Unit tests
