# INSTRUCTIONS

## Dependencies:
1. SystemC 2.3.1a (make sure to install the exact version)
2. SystemC AMS compiled against SystemC 2.3.1a
3. (Optional) GNU Octave


## Execution

1. Set SYSTEMC_PREFIX and SYSTEMC_AMS_PREFIX environment variables
with the SystemC and SystemC AMS installation paths, as in the following example:

```console
foo@bar:~$ export SYSTEMC_PREFIX=/opt/systemc-2.3.1.a
foo@bar:~$ export SYSTEMC_AMS_PREFIX=/opt/systemc_ams
```

2. Execute make to compile:
```console
foo@bar:~$ make
```

3. Execute the simulation:
```console
foo@bar:~$ iss/run.x
```

4. (Optional) Run the Octave script to display the results:
```console
foo@bar:~$ octave iss/sim.m 
```