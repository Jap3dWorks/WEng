((nil . (
	     (eval . (progn

                   (define-key global-map (kbd "C-c = c")
                               (lambda ()
                                 (interactive)
                                 (ja-execute-shell-script (concat (project-root (project-current)) "Scripts/cmd-compile.sh" ))))


                   (define-key global-map (kbd "C-c = d")
                               (lambda ()
                                 (interactive)
                                 (ja-execute-shell-script (concat (project-root (project-current)) "Scripts/cmd-compile-debug.sh" ))))

                   (define-key global-map (kbd "C-c = r")
                               (lambda ()
                                 (interactive)
                                 (ja-execute-shell-script (concat (project-root (project-current)) "Scripts/cmd-launch-spacers.sh"))))

                   (setenv "LD_LIBRARY_PATH" "lib")

                   (dap-register-debug-template
                    "[LLDB][WVulkanTest] Run"
                    (list :type "lldb-vscode"
                          :cwd "${workspaceFolder}/Install/Linux_x86_64_Debug_Standalone"
                          :request "launch"
                          :program "bin/WVulkanTest" 
                          :name "LLDB::Run"
                          :env '()))

                   (dap-register-debug-template
                    "[LLDB][WSpacers] Run"
                    (list :type "lldb-vscode"
                          :cwd "${workspaceFolder}/Install/Linux_x86_64_Debug_Standalone"
                          :request "launch"
                          :program "bin/WSpacers" 
                          :name "LLDB::Run"
                          :env '()))

                   (dap-register-debug-template
                    "[LLDB][unittest] WEngineObjects"
                    (list :type "lldb-vscode"
                          :cwd "${workspaceFolder}/Install/Linux_x86_64_Debug_Standalone"
                          :request "launch"
                          :program "bin/WEngineObjects_unittest" 
                          :name "LLDB::Run"
                          :env '()))

                   (dap-register-debug-template
                    "[LLDB][unittest] WCore"
                    (list :type "lldb-vscode"
                          :cwd "${workspaceFolder}/Install/Linux_x86_64_Debug_Standalone"
                          :request "launch"
                          :program "bin/WCore_unittest" 
                          :name "LLDB::Run"
                          :env '()))


		           )))))

