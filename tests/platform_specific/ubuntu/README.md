# Ubuntu example

This is the simplest Ubuntu example with CMake.

## Purpose

The application is inbtended simply to check that the library can be builded with corresponded compiler for given plathorm without Warnings and Errors.

## Usage


```bash
cd libparams
make ubuntu
```

It will generate an executable file in `libparams/build` folder and execute it.

The expected output can be as shown below:

```
Flash driver: load data from /home/nex/software/libparams/tests/platform_specific/ubuntu/default_params.yaml...
               uavcan.node.id: (offset=0) 27
          uavcan.can.baudrate: (offset=4) 125000
     uavcan.pub.baro.press.id: (offset=8) 2000
      uavcan.pub.baro.temp.id: (offset=12) 2001
            uavcan.pub.mag.id: (offset=16) 2002
          uavcan.pub.mag.type: (offset=1992) mag.type.is.const.anyway
    uavcan.pub.baro.temp.type: (offset=1936) baro.temp.type.is.const.anyway
   uavcan.pub.baro.press.type: (offset=1880) baro.press.type.is.const.anyway
           system.description: (offset=1824) description.is.here
                               
Integer parameters:
- uavcan.node.id: 27
- uavcan.can.baudrate: 1000000
- uavcan.pub.baro.press.id: 2000
- uavcan.pub.baro.temp.id: 2001
- uavcan.pub.mag.id: 2002
String parameters:
- system.description: description.is.here
- uavcan.pub.baro.press.type: uavcan.si.sample.pressure.Scalar
- uavcan.pub.baro.temp.type: uavcan.si.sample.temperature.Scalar
- uavcan.pub.mag.type: uavcan.si.sample.magnetic_field_strength.Vector3
```

## Notes

- Since Ubuntu flash memory drover emulates only a single 2 Kbyte page, we call `romInit(0, 1)`.
- Related workflow: [![build](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml/badge.svg)](https://github.com/PonomarevDA/libparams/actions/workflows/build.yml)
