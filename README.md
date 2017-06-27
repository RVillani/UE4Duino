# UE4Duino

## Unreal Engine 4 plugin for Arduino (COM port) communication on Windows

### How to use

Create a *Plugins* folder in your project directory. Place the folder UE4Duino (the one that contains Source, Resources etc) in the Plugins folder.

You can use the Open Serial Port function and store the result in a variable
![Example 1](UE4Duino/Example.jpg?raw=true "Example 1")

Or create the Serial object yourself and open the port afterwards
![Example 2](UE4Duino/Example2.jpg?raw=true "Example 2")
Either way, **never forget to call ClosePort** on the Serial instance before quitting the game. EndPlayEvent is a good place for that. If you forget to do it, you'll have to unplug the Arduino (or other COM device) from the PC and plug it back.

You can set the type of line endings used with the WriteLine function.
![Example 3](UE4Duino/Example3.JPG?raw=true "Example 3")

There's an example Arduino script in UE4Duino/ArduinoTypesExample. It demonstrates how to send floats, ints and strings to Unreal.
![Example 4](UE4Duino/Example4.JPG?raw=true "Example 4")
![Example 5](UE4Duino/Example5.JPG?raw=true "Example 5")

### Forum threads
[Thread for this version.](https://forums.unrealengine.com/showthread.php?148551-Free-Windows-only-UE4Duino-2-Arduino-(COM-port)-communication&p=728834#post728834)

Original version of the plugin was created by **gryzly32** user on the Unreal Engine forums.  
[Thread for first version.](https://forums.unrealengine.com/showthread.php?68643-UE4Duino-Arduino-to-UE4-plugin-Release!&highlight=ue4duino)
