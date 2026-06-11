((nil . (
	     (eval . (progn

                   (define-key global-map (kbd "C-c = c")
                               (lambda ()
                                 (interactive)
                                 (compile
                                  (concat "make -k -C "
                                          (project-root (project-current))
                                          " compile-release" ))))


                   (define-key global-map (kbd "C-c = d")
                               (lambda ()
                                 (interactive)
                                 (compile
                                  (concat "make -k -C "
                                   (project-root (project-current))
                                   " compile-debug" ))))

                   (define-key global-map (kbd "C-c = r")
                               (lambda ()
                                 (interactive)
                                 (async-shell-command (concat
                                                       "_run_toolbox_cmd_.sh "
                                                       "bash -c 'cd "
                                                       (project-root (project-current))
                                                       "/Install/Release ; "
                                                       "LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib bin/WSpacers'"))))

                   ;; (setenv "LD_LIBRARY_PATH" "lib")
                   (with-eval-after-load 'dap-mode
                       (progn
                         (dap-register-debug-template
                          "[LLDB][WVulkanTest] Run"
                          (list :type "lldb-vscode"
                                :cwd "${workspaceFolder}/Install/Debug"
                                :request "launch"
                                :program "bin/WVulkanTest" 
                                :name "LLDB::Run"
                                :env '(("LD_LIBRARY_PATH" . "lib"))))

                         (dap-register-debug-template
                          "[LLDB][WSpacers] Run"
                          (list :type "lldb-vscode"
                                :cwd "${workspaceFolder}/Install/Debug"
                                :request "launch"
                                :program "bin/WSpacers" 
                                :name "[LLDB][WSpacers]"
                                :env '(("LD_LIBRARY_PATH" . "lib"))))

                         (dap-register-debug-template
                          "[LLDB][unittest] WEngineObjects"
                          (list :type "lldb-vscode"
                                :cwd "${workspaceFolder}/Install/Debug"
                                :request "launch"
                                :program "bin/WEngineObjects_unittest" 
                                :name "LLDB::Run"
                                :env '(("LD_LIBRARY_PATH" . "lib"))))

                         (dap-register-debug-template
                          "[LLDB][unittest] WCore"
                          (list :type "lldb-vscode"
                                :cwd "${workspaceFolder}/Install/Debug"
                                :request "launch"
                                :program "bin/WCore_unittest" 
                                :name "LLDB::Run"
                                :env '(("LD_LIBRARY_PATH" . "lib"))))
                         ))

		           )))))

