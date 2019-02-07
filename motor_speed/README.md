# INSTRUCTIONS

## Dependencies:
1. SystemC 2.3.1a (make sure to install the exact version)
2. SystemC AMS compiled against SystemC 2.3.1a
3. (Optional) GNU Octave


## Execution

1. Set SYSTEMC_PREFIX and SYSTEMC_AMS_PREFIX environment variables
with the SystemC and SystemC AMS installation paths.

```console
foo@bar:~$ export SYSTEMC_PREFIX=/opt/systemc-2.3.1.a
foo@bar:~$ export SYSTEMC_AMS_PREFIX=/opt/systemc_ams
```

2. Go to the desired folder (de_controller, eln_controller, lsf_controller or tdf_controller), compile, execute and displ: 

```console
foo@bar:~$ cd de_controller
```

3. Execute make to compile:
```console
foo@bar:~$ make 
```

4. Execute the simulation:
```console
foo@bar:~$ ./main
```
5. (Optional) Run the Octave script to display the results:
```console
foo@bar:~$ octave sim.m 
```