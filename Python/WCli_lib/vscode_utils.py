import json
import os
import re


class VSCWorkspaceManager(object):
    WORKSPACE_EXTENSION = ".code-workspace"

    def _normalize_workspace_path(self, workspace_path):
        dir_name = None 
        if os.path.isdir(workspace_path):
            dir_name = os.path.basename(workspace_path)

        match = re.search(r"([^/]+)/$", workspace_path)
        if match:
            dir_name = match.group(1)

        if dir_name:
            workspace_path = "%s/%s%s" % (
                workspace_path,
                dir_name,
                self.WORKSPACE_EXTENSION
            )

        if not workspace_path.endswith(self.WORKSPACE_EXTENSION):
            workspace_path += self.WORKSPACE_EXTENSION

        return workspace_path

    def __init__(self, workspace_path):
        self.workspace_path = self._normalize_workspace_path(
            workspace_path
        )
        self.data = \
        {
            "folders": [
            ],
            "settings": {
                "files.associations": {
                    "*.ush": "hlsl",
                    "*.usf": "hlsl",
                    "string": "cpp",
                    "iostream": "cpp",
                    "cctype": "cpp",
                    "clocale": "cpp",
                    "cmath": "cpp",
                    "csignal": "cpp",
                    "cstdarg": "cpp",
                    "cstddef": "cpp",
                    "cstdio": "cpp",
                    "cstdlib": "cpp",
                    "cstring": "cpp",
                    "ctime": "cpp",
                    "cwchar": "cpp",
                    "cwctype": "cpp",
                    "any": "cpp",
                    "array": "cpp",
                    "atomic": "cpp",
                    "strstream": "cpp",
                    "bit": "cpp",
                    "*.tcc": "cpp",
                    "bitset": "cpp",
                    "cfenv": "cpp",
                    "charconv": "cpp",
                    "chrono": "cpp",
                    "codecvt": "cpp",
                    "compare": "cpp",
                    "complex": "cpp",
                    "concepts": "cpp",
                    "condition_variable": "cpp",
                    "cstdint": "cpp",
                    "deque": "cpp",
                    "forward_list": "cpp",
                    "list": "cpp",
                    "map": "cpp",
                    "set": "cpp",
                    "unordered_map": "cpp",
                    "unordered_set": "cpp",
                    "vector": "cpp",
                    "exception": "cpp",
                    "algorithm": "cpp",
                    "functional": "cpp",
                    "iterator": "cpp",
                    "memory": "cpp",
                    "memory_resource": "cpp",
                    "numeric": "cpp",
                    "optional": "cpp",
                    "random": "cpp",
                    "ratio": "cpp",
                    "source_location": "cpp",
                    "string_view": "cpp",
                    "system_error": "cpp",
                    "tuple": "cpp",
                    "type_traits": "cpp",
                    "utility": "cpp",
                    "format": "cpp",
                    "fstream": "cpp",
                    "future": "cpp",
                    "initializer_list": "cpp",
                    "iomanip": "cpp",
                    "iosfwd": "cpp",
                    "istream": "cpp",
                    "limits": "cpp",
                    "mutex": "cpp",
                    "new": "cpp",
                    "numbers": "cpp",
                    "ostream": "cpp",
                    "ranges": "cpp",
                    "semaphore": "cpp",
                    "shared_mutex": "cpp",
                    "span": "cpp",
                    "sstream": "cpp",
                    "stdexcept": "cpp",
                    "stdfloat": "cpp",
                    "stop_token": "cpp",
                    "streambuf": "cpp",
                    "thread": "cpp",
                    "cinttypes": "cpp",
                    "typeindex": "cpp",
                    "typeinfo": "cpp",
                    "valarray": "cpp",
                    "variant": "cpp"                
                }
    	    },
            "tasks": {
                "version": "2.0.0",
                "tasks": [
                ]
            },
            "launch": {
                "version": "0.2.0",
                "configurations": [
                ],
                "compounds": []
            },
            "extensions": {
                "recommendations": [
                    "ms-vscode.cpptools",
                    "ms-dotnettools.csharp",
                    "vadimcn.vscode-lldb",
                    "ms-vscode.mono-debug",
                    "dfarley1.file-picker",
                    "twxs.cmake",
                    "ms-vscode.cmake-tools"
                ]
        	},

        }

    def add_folder(self, folder_path):
        self.data["folders"].insert(
            0, 
            {
                "path": os.path.abspath(folder_path), 
                "name": os.path.basename(folder_path)
            }
        )

    def add_task(self, label, command, args, type="shell", problem_matcher="$tsc", presentation={"reveal": "always"}, group="build"):
        task = {
            "label": label,
            "command": command,
            "type": type,
            "args": args,
            "problemMatcher": problem_matcher,
            "presentation": presentation,
            "group": group
        }
        self.data["tasks"]["tasks"].insert(0, task)

    def add_launch(self, name, type="cppdbg", request="launch", program=None, args=[], stop_at_entry=True, cwd="${workspaceFolder}", environment=None, externalConsole=False, MIMode="lldb", pre_launch_task=None):
        launch = {
            "name": name,
            "type": type,
            "request": request,
            "program": program,
            "args": args,
            "stopAtEntry": stop_at_entry,
            "cwd": cwd,
            "environment": [] if environment is None else environment,
            "externalConsole": externalConsole,
            "MIMode": MIMode,
            "preLaunchTask": pre_launch_task
        }

        self.data["launch"]["configurations"].insert(0, launch)

    def save(self):
        if not os.path.exists(os.path.dirname(self.workspace_path)):
            os.makedirs(os.path.dirname(self.workspace_path))

        with open(self.workspace_path, "w") as f:
            json.dump(self.data, f, indent=4)

