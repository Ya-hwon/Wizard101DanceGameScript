# Wizard101DanceGameScript
This is a script for the pet dance game in Wizard101.

Windows only, I'm not 100% sure how well this scales with different display sizes/resolutions/display colours.

Just compile with any semi-recent cpp compiler (im using /std:c++latest), should work without any additional dependencies.

I made this for my own convenience and botched it together in a short time, so the quality of this code is suboptimal (though I've not experienced any issues on my machine), but if anyone should ever unexpectedly experience any issues feel free to contact me, add an issue or fix it yourself.

## Usage

The program starts in locked mode, to change this type 'unlock' in the console, now a red circly should be displayed where the cursor is placed. Move this circle on top of the inner black circle in which the arrows of the dance game are displayed (this is used as a reference to know which pixel values to read, the significant pixels are coloured differently than the circle), if the size of the circle differs from that of the inner circle use 'size+x' or 'size-x' to adjust the size of the aim help, once it is placed properly lock it with the 'lock' command.

Now just start a game and it should automatically complete.

Exit the program either via closing the vindow or via the 'exit' command.

If something should somehow go wrong either try the 'reset' command or restart the program.
