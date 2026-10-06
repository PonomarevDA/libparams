# Ubuntu example

A minimal SITL application: it generates parameters from [params.yaml](params.yaml), loads them through the [ubuntu flash driver](../../../platform_specific/ubuntu/README.md) and prints them.

```bash
make ubuntu
```

Expected output starts with:

```
[libparams-ubuntu] [INFO]: Params dir: <repo>/build/tests/platform_specific/ubuntu
[libparams-ubuntu] [INFO]: Read params from default file: initial_params_0.yaml
Integer parameters:
...
String parameters:
...
```
