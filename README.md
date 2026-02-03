# How to use

## 1. Сборка основного приложения

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

run:
### Linux/MacOS
```bash
./main <path/to/file>
```

### Windows
```shell
.\Debug\main.exe <path/to/file>
```


## 2. Сборка и запуск тестов

(по умолчанию `BUILD_TESTS=ON`)

```bash
cd build
ctest
```

or:
### Linux/MacOS
```bash
./tests/tests
```
### Windows
```shell
.\tests\Debug\tests.exe
```

## 3. Сборка без тестов

```bash
cmake -DBUILD_TESTS=OFF ..
cmake --build .
```

# Formatter and linters

## lint

```bash
make lint
```

## format

```bash
make format
```
