ALL_FILES = src/*.cpp include/*.hpp tests/*.cpp

format:
	clang-format --style=file -i $(ALL_FILES)

lint:
	cpplint $(ALL_FILES)
