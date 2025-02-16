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


                (dap-register-debug-template
                "WSpacers LLDB::Run"
                (list :type "lldb-vscode"
                        :cwd nil
                        :request "launch"
                        :program (concat (project-root (project-current)) "Install/Linux_x86_64_Debug_Standalone/bin/WSpacers" )
                        :name "LLDB::Run"))

		   )))))

