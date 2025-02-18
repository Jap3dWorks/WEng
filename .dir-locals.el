((nil . (
	 (eval . (progn

                   (define-key evil-normal-state-map (kbd "C-c = c")
                     (lambda ()
                       (interactive)
                       (ja-execute-shell-script (concat (project-root (project-current)) ".scripts/cmd-compile.sh" ))))


                   (define-key evil-normal-state-map (kbd "C-c = d")
                     (lambda ()
                       (interactive)
                       (ja-execute-shell-script (concat (project-root (project-current)) ".scripts/cmd-compile-debug.sh" ))))

                   (setenv "LD_LIBRARY_PATH" "Install/Linux_x86_64_Debug_Standalone/lib")

                (dap-register-debug-template
                "[LLDB][WSpacers] Run"
                (list :type "lldb-vscode"
                        :cwd "${workspaceFolder}"
                        :request "launch"
                        :program "Install/Linux_x86_64_Debug_Standalone/bin/WSpacers" 
                        :name "LLDB::Run"
                        :env '()))

		   )))))

