import argparse
import subprocess
import sys
import os
import platform

class CliVars:
    BUILD_PATH = "./build"
    DEBUG_TYPE = "Debug"
    RELEASE_TYPE = "Release"

    ARCH = "x64"
    ARCH_X86 = "x86"
    ARCH_X64 = "x64"
    ARCH_X86_64 = "x86_64"

    BUILD_FOLDER_FORMAT = "{system}_{arch}_{build_type}_Standalone"

    C_COMPILER = "clang"
    CXX_COMPILER = "clang++"


class CliCommand(object):
    _COMMAND_NAME = None
    _HELP = None

    @classmethod
    def get_command_name(cls):
        return cls._COMMAND_NAME

    @classmethod
    def get_help(cls):
        return cls._HELP

    @staticmethod
    def add_parser(cls, parser):
        command_parser = parser.add_parser(
            cls.get_command_name(), 
            help=cls.get_help()
        )
        command_parser.set_defaults(command=cls)

        return command_parser

    def __init__(self, cmd_args):
        self.cmd_args = cmd_args

    def validate(self):
        return True

    def _validate(self):
        return self.validate()

    def run(self):
        pass

    def _run(self):
        assert self._validate()
        return self.run()


class BuildCommand(CliCommand):
    _COMMAND_NAME = "Build"

    def __init__(self, cmd_args):
        super(BuildCommand, self).__init__(cmd_args)

    @classmethod
    def add_parser(cls, parser):
        command_parser = CliCommand.add_parser(cls, parser)

        command_parser.add_argument(
            "-p", 
            "--build-path", 
            type=str, 
            default=CliVars.BUILD_PATH, 
            help="Build path"
        )
        command_parser.add_argument(
            "-t", 
            "--build-type", 
            type=str, 
            choices=[CliVars.DEBUG_TYPE, CliVars.RELEASE_TYPE], 
            default=CliVars.DEBUG_TYPE,
            help="Build type"
        )
        if platform.system() == "Windows":
            command_parser.add_argument(
                "-a", 
                "--arch", 
                type=str, 
                choices=[CliVars.ARCH_X86, CliVars.ARCH_X64], 
                default=CliVars.ARCH_X64,   
                help="Build architecture"
            )
        else:
            command_parser.add_argument(
                "-a", 
                "--arch", 
                type=str, 
                choices=[CliVars.ARCH_X86_64], 
                default=CliVars.ARCH_X86_64,   
                help="Build architecture"
            )

        return command_parser

    def validate(self):
        return True

    def run(self):
        build_folder = CliVars.BUILD_FOLDER_FORMAT.format(
            system=platform.system(),
            arch=self.cmd_args.arch,
            build_type=self.cmd_args.build_type
        )

        build_path = os.path.join(
            self.cmd_args.build_path,
            build_folder
        )

        if not os.path.exists(build_path):
            os.makedirs(build_path)

        cmd = [
            "cmake", 
            "-S", ".", 
            "-B", build_path, 
            "-DCMAKE_BUILD_TYPE=" + self.cmd_args.build_type, 
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", 
            "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
        ]

        subprocess.run(cmd)

        cmd = [
            "cmake",
            "--build", build_path
        ]

        subprocess.run(cmd)


class RunCommand(CliCommand):
    _COMMAND_NAME = "Run"

    def __init__(self, cmd_args):
        super(RunCommand, self).__init__(cmd_args)

    @classmethod
    def add_parser(cls, parser):
        command_parser = CliCommand.add_parser(cls, parser)

        command_parser.add_argument("-t", "--type", type=str, choices=["Debug", "Release"] , help="Build type")
        return command_parser

    def validate(self):
        return True

    def run(self):
        print("Run")


class CommandRegister(object):
    _STATIC_REGISTER = None

    @classmethod
    def get_register(cls):
        if cls._STATIC_REGISTER is None:
            cls._STATIC_REGISTER = cls()
        return cls._STATIC_REGISTER

    def __init__(self):
        self.commands = {}

    def register(self, command):
        assert issubclass(command, CliCommand)
        assert command.get_command_name() not in self.commands
        self.commands[command.get_command_name()] = command

    def get_command(self, command_name):
        return self.commands[command_name]

    def get_commands(self):
        return self.commands.values()

    def get_command_names(self):
        return self.commands.keys()


def _init_commands():
    register = CommandRegister.get_register()
    register.register(BuildCommand)
    register.register(RunCommand)


def main(*args):
    _init_commands()

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    for command in CommandRegister.get_register().get_commands():
        command.add_parser(subparsers)

    args = parser.parse_args(*args)

    result = args.command(args).run()

    if isinstance(result, int):
        return result
    
    elif isinstance(result, bool):
        return 0 if result else 1
    
    else:
        return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))