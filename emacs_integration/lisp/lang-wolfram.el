;;; lang-wolfram.el -*- lexical-binding: t; -*-


;;; Commentary: language config Wolfram Language (aka Mathematica, Wolfram Script.)
;; 1. syntax highlight provided by 'fontlock-wolfram.el'
;; 2. Major mode provided by 'wolfram-mode.el', which also provide LSP config for 'lsp-mode' and 'eglot'


;;; Code:
;; wolfram code compile
(defun wolfram-compile ()
  "Compile(Run) current buffer by wolfram engine"
  (interactive)
  (if (not buffer-file-name)
      (error "No valid file for current buffer")
    (let* (
           (filename (file-name-nondirectory buffer-file-name))
           (cmd-script     (concat "wolframscript -script " filename))
           (cmd-wstp       (concat "wstp '<< \"" filename "\"'"))
           (cmd-wstp-watch (concat "watchexec -p -w " filename " -- '" "wstp \"<< \"" filename "'" ))
           (choices (list cmd-script cmd-wstp cmd-wstp-watch))
           (selected-cmd (completing-read "Choose compile command: " choices nil nil)))
      (compile selected-cmd))))

;; wolfram language major mode
;; NOTE: 'inhibit-message' spec disables error msg from CAPF showing in minibuffer, works
;;       for both 'company' and 'corfu'. Error message can be seen in '*Message*' buffer.
(use-package wolfram-mode
  :custom (wolfram-indent-offset 4)
  :hook
    ;; compile code(relies on 'PATH' variable in 'early-init.el')
    (wolfram-mode  .  (lambda()(progn
        (indent-bars-mode)
        (setq-local tab-width 4
                    inhibit-message t
                    indent-tabs-mode nil)
        (setq-local lsp-enable-text-document-color nil)
        ;; (setq-local compile-command (concat "wolframscript" " -script " (file-name-only)))
        (setq-local compile-command (concat "wstp " "'<< \"" (file-name-only) "\"'"))
        ;; Show diagnostics on-the-fly
        (setq-local lsp-diagnostics-provider :flycheck)
      )
    ))
)


(provide 'lang-wolfram)
;;; lang-wolfram.el ends here
