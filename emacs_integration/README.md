## emacs configure
Syntax highlight and LSP config for emacs:

Init environment variables:

``` elisp
;; wolframscript path
;; NOTE: if you launch 'emacs --daemon' from shell, just
;;       set these path in '~/.zshrc' or '~/.bashrc' is ok.
(setenv "PATH" (concat
  (expand-file-name "~/Dependency/wolfram/wstp") path-separator
  "/usr/local/Wolfram/Wolfram/14.3/SystemFiles/Kernel/Binaries/Linux-x86-64"
  path-separator (getenv "PATH"))
)
```


Setup wolfram mode:

``` elisp
;; wolfram language major mode
;; NOTE: 'inhibit-message' spec disables error msg from CAPF showing in minibuffer, works
;;       for both 'company' and 'corfu'. Error message can be seen in '*Message*' buffer.
(defun file-name-only ()
  "Get the current buffer file name without directory."
  (file-name-nondirectory (buffer-name)))
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
```

> Do NOT copy this code to your config without modification !

For more details, please refer to the `lisp` directory.


## launch wolfram kernel
Use command(check project `scripts` folder for this `wstp_daemon` script):

``` shell
wstp_daemon start
```


## compile scripts on the fly
I use [watchexec](https://github.com/watchexec/watchexec), run command below:

``` shell
cd example

watchexec 'wstp "<< "emacs_example.wls'
# or
watchexec -p -w emacs_example.wls -- 'wstp "<< "emacs_example.wls'
```


Or you can use `wolfram-compile` command, which is defined below:

``` elisp
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
```

Add it to your Emacs config, call it by `M-x wolfram-compile RET`, then choose a compiling command in your flavor.


## Working Example

![](example/example_screencast.png)


## Reference

* [WolframResearch LSPServer](https://github.com/WolframResearch/LSPServer)
* [Xah-wolfram-mode](https://github.com/xahlee/xah-wolfram-mode)
* [sci-wolfram](https://github.com/TurbulenceChaos/sci-wolfram)
* [wolfram language mode](https://github.com/transentis/wolfram-language-mode)
* [lsp-wl](https://github.com/kenkangxgwe/lsp-wl)
