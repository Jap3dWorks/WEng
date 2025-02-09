((nil . (
	 (eval . (progn

                   (define-key evil-normal-state-map (kbd "C-c = c")
                     (lambda ()
                       (interactive)
                       (ja-execute-shell-script (concat (project-root (project-current)) ".scripts/cmd-compile.sh" ))))

		   )))))

