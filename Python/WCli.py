import argparse
import subprocess
import sys
import os
import platform
from WCli_lib import vscode_utils, project_manager
from WCli_lib.logger import wlogger
import copy


class CliVars:
    DEBUG_TYPE = "Debug"
    RELEASE_TYPE = "Release"

    ARCH = "x64"
    ARCH_X86 = "x86"
    ARCH_X64 = "x64"
    ARCH_X86_64 = "x86_64"

    C_COMPILER = "clang"
    CXX_COMPILER = "clang++"

    WENG = "WEng"

class CommandUtils:
    @staticmethod
    def clean_project_engine_paths(out_cmd_args) -> bool:
        out_cmd_args.engine_path = os.path.abspath(
            out_cmd_args.engine_path
        )
        
        if not out_cmd_args.project_path:
            wlogger.info("Project path not set, Set engine path as project path.")
            out_cmd_args.project_path = out_cmd_args.engine_path
        else:
            out_cmd_args.project_path = os.path.abspath(
                out_cmd_args.project_path
            )

        return True

    @staticmethod
    def is_engine_path(path) -> bool:
        dirname = os.path.basename(path)
        return dirname == CliVars.WENG

    @staticmethod
    def error_message_engine_path(path)->None:
        wlogger.error(
            "Not correct engine path, '{path}'.".format(
                path=path
            )
        )


class CliCommand(object):
    _COMMAND_NAME = None
    _HELP = None

    @classmethod
    def get_command_name(cls) -> str:
        return cls._COMMAND_NAME

    @classmethod
    def get_help(cls) -> str:
        return cls._HELP

    @classmethod
    def _create_parser(cls, parser) -> argparse.ArgumentParser:
        command_parser = parser.add_parser(
            cls.get_command_name(), 
            help=cls.get_help()
        )
        command_parser.set_defaults(command=cls)

        cls.add_parser(command_parser)

        return command_parser

    @staticmethod
    def add_parser(command_parser) -> argparse.ArgumentParser:
        """
        Add parser to the command
        Command classes should override this method.
        """
        pass

    def __init__(self, cmd_args) -> None:
        self.cmd_args = cmd_args

    def validate(self) -> bool:
        return True

    def _validate(self) -> bool:
        return self.validate()

    def run(self) -> int:
        pass

    def _run(self) -> int:
        return self.run()


class BuildCommand(CliCommand):
    _COMMAND_NAME = "Build"

    def __init__(self, cmd_args) -> None:
        super(BuildCommand, self).__init__(cmd_args)

    @staticmethod
    def add_parser(command_parser) -> argparse.ArgumentParser:
        command_parser.add_argument(
            "-t", 
            "--build-type", 
            type=str, 
            choices=[CliVars.DEBUG_TYPE, CliVars.RELEASE_TYPE], 
            default=CliVars.DEBUG_TYPE,
            help="Build type."
        )
        if platform.system() == "Windows":
            command_parser.add_argument(
                "-a", 
                "--arch", 
                type=str, 
                choices=[CliVars.ARCH_X86, CliVars.ARCH_X64], 
                default=CliVars.ARCH_X64,   
                help="Build architecture."
            )
        else:
            command_parser.add_argument(
                "-a", 
                "--arch", 
                type=str, 
                choices=[CliVars.ARCH_X86_64], 
                default=CliVars.ARCH_X86_64,   
                help="Build architecture."
            )

        return command_parser

    def validate(self) -> bool:
        return True

    def run(self) -> int:

        build_path = project_manager.ProjectPaths.get_build_folder(
            self.cmd_args.arch,
            self.cmd_args.build_type,
            project_manager.ProjectPaths.BUILD_PATH     
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

        completed_process = subprocess.run(cmd, stderr=subprocess.STDOUT)

        if completed_process.returncode != 0:
            return completed_process.returncode

        wlogger.info("Build %s" % build_path)

        cmd = [
            "cmake", "--build", build_path
        ]

        completed_process = subprocess.run(cmd, stderr=subprocess.STDOUT)

        if completed_process.returncode != 0:
            return completed_process.returncode

        install_path = project_manager.ProjectPaths.get_bin_folder(
            self.cmd_args.arch,
            self.cmd_args.build_type
        )

        if install_path.endswith("/bin"):
            install_path = install_path[:-4]

        wlogger.info("Install %s" % install_path)
        
        cmd = [
            "cmake", "--install", build_path, "--prefix", install_path, "-v"
        ]

        completed_process = subprocess.run(cmd, stderr=subprocess.STDOUT)

        return completed_process.returncode


class RunCommand(CliCommand):
    _COMMAND_NAME = "Run"

    def __init__(self, cmd_args) -> None:
        super(RunCommand, self).__init__(cmd_args)

    @staticmethod
    def add_parser(command_parser) -> argparse.ArgumentParser:
        command_parser.add_argument(
            "-t", "--type", 
            type=str, 
            choices=[CliVars.DEBUG_TYPE, CliVars.RELEASE_TYPE] ,
            default=CliVars.DEBUG_TYPE, 
            help="Build type."
        )

        if platform.system() == "Windows":
            command_parser.add_argument(
                "-a", "--arch",
                type=str,
                choices=[CliVars.ARCH_X86, CliVars.ARCH_X64],
                default=CliVars.ARCH_X64,
                help="Build architecture."

            )
        else:
            command_parser.add_argument(
                "-a", "--arch",
                type=str,
                choices=[CliVars.ARCH_X86_64],
                default=CliVars.ARCH_X86_64,
                help="Build architecture."

            )

        command_parser.add_argument(
            "-tg", "--target",
            type=str,
            help="Target to run."
        )

        return command_parser

    def validate(self) -> bool:
        return True

    def run(self) -> int:
        target_path = project_manager.ProjectPaths.get_target_bin_path(
            self.cmd_args.arch,
            self.cmd_args.type,
            self.cmd_args.target
        )

        wlogger.info("Run '%s'." % target_path)

        subprocess.run([target_path])

        return 0


class ProjectCommand(CliCommand):
    _COMMAND_NAME = "Project"

    def __init__(self, cmd_args) -> None:
        super().__init__(cmd_args)
        self.project_manager = None

    def add_parser(command_parser) -> argparse.ArgumentParser:
        command_parser.add_argument(
            "-p",
            "--project-path",
            type=str,
            help="Project directory path, Project name is the last folder name."
        )
        command_parser.add_argument(
            "-ep",
            "--engine-path",
            type=str,
            default=os.getcwd(),
            help="Engine directory path."
        )
        command_parser.add_argument(
            "-u",
            "--update",
            action="store_true",
            help="Create or Update project."
        )
        command_parser.add_argument(
            "-m",
            "--modules",
            nargs="+",
            type=str,
            help="Add modules to project."
        )

    def validate(self) -> bool:
        CommandUtils.clean_project_engine_paths(self.cmd_args)
        is_engine = CommandUtils.is_engine_path(self.cmd_args.engine_path)
        if not is_engine:
            CommandUtils.error_message_engine_path(self.cmd_args.engine_path)
            return False

        self.project_manager = project_manager.ProjectManager(
            self.cmd_args.project_path,
            self.cmd_args.engine_path
        )

        return True

    def run(self) -> int:
        if self.cmd_args.update:
            self.project_manager.update_project()
            wlogger.info(
                "Project updated at '{project_path}'".format(
                    project_path=self.cmd_args.project_path
                )
            )
        
        for module in self.cmd_args.modules:
            self.project_manager.update_module(module)
            wlogger.info(
                "Module '{module}' updated at '{project_path}'".format(
                    module=module,
                    project_path=self.cmd_args.project_path
                )
            )

        return 0


class VSCEnvCommand(CliCommand):
    _COMMAND_NAME = "VSCEnv"

    def __init__(self, cmd_args) -> None:
        super(VSCEnvCommand, self).__init__(cmd_args)

    @staticmethod
    def add_parser(command_parser) -> argparse.ArgumentParser:
        command_parser.add_argument(
            "-p",
            "--project-path",
            type=str,
            help="Project directory path."
        )
        command_parser.add_argument(
            "-ep",
            "--engine-path",
            type=str,
            default=os.getcwd(),
            help="Engine directory path."
        )

    def validate(self) -> bool:
        CommandUtils.clean_project_engine_paths(self.cmd_args)

        is_engine = CommandUtils.is_engine_path(self.cmd_args.engine_path)
        if not is_engine:
            CommandUtils.error_message_engine_path(self.cmd_args.engine_path)
            return False

        return True

    def run(self) -> int:
        workspace_path = self.cmd_args.project_path or self.cmd_args.engine_path
        
        workspace_manager = vscode_utils.VSCWorkspaceManager(
            workspace_path
        )

        workspace_manager.add_folder(self.cmd_args.engine_path)
        if self.cmd_args.project_path != self.cmd_args.engine_path:
            workspace_manager.add_folder(self.cmd_args.project_path)
        
        workspace_manager.add_task(
            "Build X86_64 Debug",
            "WCli",
            [
                "Build", "-t", CliVars.DEBUG_TYPE, "-a", CliVars.ARCH_X86_64
            ]
        )

        workspace_manager.add_task(
            "Build X86_64 Release",
            "WCli",
            [
                "Build", "-t", CliVars.RELEASE_TYPE, "-a", CliVars.ARCH_X86_64
            ]
        )

        # TODO: set the engine and project launch jobs
        if CommandUtils.is_engine_path(self.cmd_args.project_path):
            workspace_manager.add_launch(
                "WSandBox X86_64 Debug",
                program=project_manager.ProjectPaths.get_target_bin_path(
                    CliVars.ARCH_X86_64, 
                    CliVars.DEBUG_TYPE, 
                    "WSandBox"),
                args=[],
                pre_launch_task="Build X86_64 Debug",
                MIMode=None
            )

            workspace_manager.add_launch(
                "WSandBox X86_64 Release",
                program=project_manager.ProjectPaths.get_target_bin_path(
                    CliVars.ARCH_X86_64, 
                    CliVars.RELEASE_TYPE, 
                    "WSandBox"),
                args=[],
                pre_launch_task="Build X86_64 Release",
                MIMode=None
            )

        workspace_manager.save()

        vscode_path = os.path.join(workspace_path, ".vscode")
        if not os.path.exists(vscode_path):
            os.makedirs(vscode_path)
        
        wlogger.info("VSC environment created at '{workspace_path}'.".format(
            workspace_path=workspace_path
            )
        )

        return 0


class CommandRegister(object):
    _STATIC_REGISTER = None

    @classmethod
    def get_register(cls) -> "CommandRegister":
        if cls._STATIC_REGISTER is None:
            cls._STATIC_REGISTER = cls()
        return cls._STATIC_REGISTER

    def __init__(self) -> None:
        self.commands = {}

    def register(self, command) -> None:
        assert issubclass(command, CliCommand)
        assert command.get_command_name() not in self.commands
        self.commands[command.get_command_name()] = command

    def get_command(self, command_name) -> CliCommand:
        return self.commands[command_name]

    def get_commands(self) -> list:
        return self.commands.values()

    def get_command_names(self) -> list:
        return self.commands.keys()


def _init_commands() -> None:
    register = CommandRegister.get_register()
    register.register(BuildCommand)
    register.register(RunCommand)
    register.register(VSCEnvCommand)
    register.register(ProjectCommand)


def main(*args) -> int:
    _init_commands()

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    for command in CommandRegister.get_register().get_commands():
        command._create_parser(subparsers)

    args = parser.parse_args(*args)
    command = args.command(args)

    assert command._validate()
    result = command._run()

    if isinstance(result, int):
        return result
    
    elif isinstance(result, bool):
        return 0 if result else 1
    
    else:
        return 0


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        args = ["-h"]
    else:
        args = sys.argv[1:]

    sys.exit(main(args))
