To successfully run the code, you must instruct the system to search for dynamic libraries in the current directory. This can be achieved by setting the `LD_LIBRARY_PATH` environment variable in your terminal. 

To do it follow the steps below:

1. Open your terminal and set the `LD_LIBRARY_PATH` to the current directory by running the following command:

```bash
export LD_LIBRARY_PATH=.
```

2. After setting the LD_LIBRARY_PATH, compile the code by running `make`.

<hr>

Please note that if you skip setting the LD_LIBRARY_PATH, the system might not be able to locate the required dynamic libraries, resulting in an error similar to the one shown below:

`error while loading shared libraries: libpoisson.so: cannot open shared object file: No such file or directory`