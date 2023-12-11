import json
import os
import re


class VSCWorkspaceManager(object):
    WORKSPACE_EXTENSION = ".code-workspace"

    def _normalize_workspace_path(self, workspace_path):
        dir_name = None 
        if os.path.isdir(workspace_path):
            dir_name = os.path.basename(workspace_path)

        match = re.search(r"([^/]+)/$")
        if match:
            dir_name = match.group(1)

        if dir_name:
            workspace_path = "%s/%s%s" % (
                workspace_path,
                match.group(1),
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
                    "iostream": "cpp"
                }
	    }
        }

    def add_folder(self, folder_path):
        self.folder["folders"].insert(
            0, 
            {
                "path": os.path.abspath(folder_path), 
                "name": os.path.basename(folder_path)
            }
        )

    def add_task(self, task_name, task):
        pass

    def save(self):
        if not os.path.exists(os.path.dirname(self.workspace_path)):
            os.makedirs(os.path.dirname(self.workspace_path))

        with open(self.workspace_path, "w") as f:
            json.dump(self.data, f, indent=4)

