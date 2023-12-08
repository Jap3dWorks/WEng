import argparse
import subprocess
import sys


class CliCommand(object):
    _COMMAND_NAME = None
    _HELP = None

    @classmethod
    def get_command_name(cls):
        return cls.COMMAND_NAME

    @classmethod
    def get_help(cls):
        return cls._HELP

    @classmethod
    def add_parser(cls, parser):
        command_parser = parser.add_subparsers(cls._COMMAND_NAME)
        command_parser.add_default(command=cls._COMMAND_NAME)

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
        command_parser = super(BuildCommand, cls).add_parser(parser)
        command_parser.add_argument("-t", "--type", type=str, choices=["Debug", "Release"] , help="Build type")
        return command_parser

    def validate(self):
        return True

    def run(self):
        print("Build")


class RunCommand(CliCommand):
    _COMMAND_NAME = "Run"

    def __init__(self, cmd_args):
        super(RunCommand, self).__init__(cmd_args)

    @classmethod
    def add_parser(cls, parser):
        command_parser = super(BuildCommand, cls).add_parser(parser)
        command_parser.add_argument("-t", "--type", type=str, choices=["Debug", "Release"] , help="Build type")
        return command_parser

    def validate(self):
        return True

    def run(self):
        print("Run")


class CommandRegister(object):
    _STATIC_REGISTER = None

    @classmethod
    def get_static_register(cls):
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
    register = CommandRegister.get_static_register()
    register.register(BuildCommand)
    register.register(RunCommand)


def main(*args):
    _init_commands()

    parser = argparse.ArgumentParser()

    for command in CommandRegister.get_static_register().get_commands():
        command.add_parser(parser)

    args = parser.parse_args(*args)

    result = CommandRegister.get_static_register().get_command(args.command).run(args)

    if isinstance(result, int):
        return result
    
    elif isinstance(result, bool):
        return 0 if result else 1
    
    else:
        return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))