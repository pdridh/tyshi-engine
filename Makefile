BUILD_DIR = build
BUILD_WIN_DIR = ${BUILD_DIR}/windows
BUILD_LINUX_DIR = ${BUILD_DIR}/linux
BIN_DIR = bin
BIN_WIN_DIR = ${BIN_DIR}/windows
BIN_LINUX_DIR = ${BIN_DIR}/linux
TOOLCHAIN_FILE = CMake/mingw-toolchain.cmake

all: dev

dev:
	cmake -S . -B ${BUILD_LINUX_DIR} -DCMAKE_BUILD_TYPE=Debug
	cmake --build ${BUILD_LINUX_DIR} --target install
	cd ${BIN_LINUX_DIR} && ./tyshi-engine-test

release:
	cmake -S . -B ${BUILD_LINUX_DIR} -DCMAKE_BUILD_TYPE=Release
	cmake --build ${BUILD_LINUX_DIR}

win-dev:
	cmake -S . -B ${BUILD_WIN_DIR} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
	cmake --build ${BUILD_WIN_DIR} --target install

win-release:
	cmake -S . -B ${BUILD_WIN_DIR} -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}
	cmake --build ${BUILD_WIN_DIR}

clean: clean-build clean-bin

clean-build:
	rm -rf ${BUILD_DIR}

clean-bin:
	rm -rf ${BIN_DIR}

.PHONY: lib dev release win-lib win-dev win-release clean clean-build clean-bin