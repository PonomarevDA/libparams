BUILD_PATH=build
GCOV_REPORT_PATH=build/report

all: unit_test

create_build_dir:
	mkdir -p ${BUILD_PATH}

coverage:
	cd tests/ubuntu && $(MAKE) -s coverage

	mkdir -p ${GCOV_REPORT_PATH}
	gcov ${BUILD_PATH}/shell-storage.gcda ${BUILD_PATH}/shell-rom.gcda

unit_test: clean
	cd tests/ubuntu && $(MAKE) -s unit_tests && cd ..

clean:
	rm -rf ${BUILD_PATH}
