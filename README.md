```sh
make all
make hello_world
make clean
make TARGET=esp32s3 all
```

```sh
cd examples/hello_world
get_idf
idf.py -p /dev/ttyACM0 flash monitor
```

Press `Ctrl` + `5` to exit the monitor.
