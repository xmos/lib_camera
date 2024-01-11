import subprocess

commands = """
cmake -G "Unix Makefiles" -S take_picture_downsample -B take_picture_downsample/build
xmake -C take_picture_downsample/build

cmake -G "Unix Makefiles" -S take_picture_local -B take_picture_local/build
xmake -C take_picture_local/build

cmake -G "Unix Makefiles" -S take_picture_raw -B take_picture_raw/build
xmake -C take_picture_raw/build
"""

# Split the commands into a list of individual commands
command_list = [command.strip() for command in commands.split('\n') if command.strip()]

# Run each command
for command in command_list:
    subprocess.run(command, shell=True)
