# checkers_ai
This checkers engine uses Alpha-Beta prunning

## Compilation
First, it's necessary to compile the C++ engine:
```
$ cd out/
$ ./compile
$ cd ..
```

Then install the Python's library Tkinter. For Ubuntu:
```
$ sudo apt-get install python3-tk
```

Finally, run the program, with the command:

```
$ python3 checkers_gui.py
```

## Mode
The program is by default in Human vs Machine mode.

To change it to a Machine vs Machine mode, edit line 21 of the file checkers_gui.py:
```
ai_players = ['b']
#ai_players = ['w', 'b']
```
Comment the line 21 and uncomment the 22:
```
#ai_players = ['b']
ai_players = ['w', 'b']
```

Also, it's possible to tune the depth of the Minimax for each player, editing the  lines 29,30 of the file checkers_gui.py:
```
depth_ai_player = {
    'w' : 10,
    'b' : 10,
}
```
