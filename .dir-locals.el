((nil . (
	 (eval . (progn

                   (define-key evil-normal-state-map (kbd "C-c = c")
                     (lambda ()
                       (interactive)
                       (my-script-runner (concat (project-root (project-current)) ".scripts/cmd-compile.sh" ))))

		   )))))

