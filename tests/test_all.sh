THIS_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $THIS_SCRIPT_DIR

cd params_generator_c   && ./test.sh && cd ..
cd params_generator_cpp && ./test.sh && cd ..
cd stm32f103_build      && make clean && make -s && cd ..
cd stm32g0b1_build      && make clean && make -s && cd ..
cd ubuntu               && make clean && make -s && cd ..

cd $THIS_SCRIPT_DIR
echo "Everything is fine!"